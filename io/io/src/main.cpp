#include <Wire.h>
#include <Arduino.h>
#include <BluetoothSerial.h>
#include <SoftwareSerial.h>
#include <SD.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <AnimatedGIF.h>
#include <DFPlayer.h>
#include <DFMiniMp3.h>

#include "config.h"
#include "SerialUnitRouter.hpp"
#include "SerialUnitProcessor.hpp"
#include "Soul.hpp"
#include "Listeners/LoveYouListener.hpp"
#include "esp_system.h"
#include "esp_err.h"

// ----------------------------------------------------------------------------
// Function Declarations
// ----------------------------------------------------------------------------

void halt();

// Tasks
void bluetoothSerialReaderTask(void *pvParameters);
void controlSerialReaderTask(void *pvParameters);
void runnerTask(void *pvParameters);

// ----------------------------------------------------------------------------
// Global Variables
// ----------------------------------------------------------------------------

typedef DFMiniMp3<EspSoftwareSerial::UART, DfMp3NoCallback> DfMp3;

struct GlobalState
{
  HardwareSerial controlSerial;
  BluetoothSerial serialBT;
  SerialUnitRouter router;
  SerialUnitProcessor serialUnitProcessor;
  EspSoftwareSerial::UART mp3Serial;
  TFT_eSPI tft;
  RobotDisplay display;
  Soul soul;
  //DFPlayer dfplayer;
  DfMp3 dfplayer;

  GlobalState()
      : controlSerial(2),
        serialBT(),
        router(serialBT, controlSerial),
        serialUnitProcessor(),
        tft(TFT_eSPI(240, 240)),
        mp3Serial(PIN_MP3_SERIAL_RX, PIN_MP3_SERIAL_TX),
        display(tft),
        soul(&mp3Serial, &display, &SD),
        dfplayer(mp3Serial)
  {
  }
};
GlobalState gstate;

// ----------------------------------------------------------------------------
// Setup
// ----------------------------------------------------------------------------

SPIClass SPISD(HSPI);
SPIClass SPITFT(VSPI);

const char *gifPath = "/carma-240.gif";

void setup()
{
  setCpuFrequencyMhz(160);
  delay(10);
  esp_log_level_set("*", ESP_LOG_INFO);  // Set log level for verbosity
  delay(10);

  gstate.controlSerial.begin(CONTROL_SERIAL_BAUD);
  Serial.begin(SERIAL_BAUD);
  if (!gstate.serialBT.begin(BLUETOOTH_NAME))
  {
    Serial.println("FATAL_ERROR: Failed to initialize Bluetooth!");
    halt();
  }
  Serial.println("Bluetooth device started, ready to pair with name: " + String(BLUETOOTH_NAME));

  SPISD.begin(PIN_CARD_SCK, PIN_CARD_MISO, PIN_CARD_MOSI, PIN_CARD_CS);
  Serial.println("HSPI (Card) initialized");
  if (!SD.begin(PIN_CARD_CS, SPISD, 4000000))
  {
    Serial.println("ERROR: SD card initialization failed!");
    return;
  }
  Serial.println("SD card initialized.");

  gstate.tft.init();
  Serial.println("TFT initialized");

  gstate.display.playGif(gifPath);
  Serial.println("GIF Opened...");
  gstate.tft.fillScreen(TFT_BLACK);
  gstate.tft.setSwapBytes(true);

  xTaskCreatePinnedToCore(
      bluetoothSerialReaderTask,
      "bluetoothSerial",
      4048,
      NULL,
      4,
      NULL,
      0);

  xTaskCreatePinnedToCore(
      controlSerialReaderTask,
      "controlSerial",
      4048,
      NULL,
      4,
      NULL,
      0);

  xTaskCreatePinnedToCore(
      runnerTask,
      "executor",
      6096,
      NULL,
      2,
      NULL,
      1);

  gstate.mp3Serial.begin(9600, EspSoftwareSerial::SWSERIAL_8N1, PIN_MP3_SERIAL_RX, PIN_MP3_SERIAL_TX);
  gstate.dfplayer.begin();
  gstate.dfplayer.reset(); 
  gstate.dfplayer.setPlaybackSource(DfMp3_PlaySource_Sd);
  gstate.dfplayer.setPlaybackMode(DfMp3_PlaybackMode_SingleRepeat);
  gstate.dfplayer.start();

  gstate.dfplayer.setVolume(2);
  // //gstate.dfplayer.enableDac();

  uint16_t count = gstate.dfplayer.getTotalTrackCount(DfMp3_PlaySource_Sd);
  Serial.print("files ");
  Serial.println(count);

  // uint16_t mode = gstate.dfplayer.getPlaybackMode();  
  // Serial.print("playback mode ");
  // Serial.println(mode);
  gstate.dfplayer.playRandomTrackFromAll();

  gstate.serialUnitProcessor.addListener(new LoveYouListener());
}

// ----------------------------------------------------------------------------
// Loop
// ----------------------------------------------------------------------------

void loop()
{
}

// ----------------------------------------------------------------------------
// Function Definitions
// ----------------------------------------------------------------------------

void processSerialUnit(SerialUnitEndpoint source, String &line)
{
  SerialUnitAlias alias = SerialUnitFactory::readAlias(line);
  gstate.router.route(source, alias, line);
  gstate.serialUnitProcessor.process(alias, line);
}

void halt()
{
  while (true)
  {
    Serial.println("Halting...");
    delay(1000);
  }
}

// ----------------------------------------------------------------------------
// Task Definitions
// ----------------------------------------------------------------------------

void bluetoothSerialReaderTask(void *pvParameters)
{
  while (true)
  {
    while (gstate.serialBT.available())
    {
      try {
        String line = gstate.serialBT.readStringUntil('\n');
        processSerialUnit(BLUETOOTH, line);
        Serial.print("BT: ");
        Serial.println(line);
      } catch (const std::exception& e) {
        Serial.println("ERROR: Could not read bluetooth serial. Reason: " + String(e.what()));
      }
    }
    vTaskDelay(portTICK_PERIOD_MS);
  }
}


void controlSerialReaderTask(void *pvParameters)
{
  while (true)
  {
    while (gstate.controlSerial.available())
    {
      String line = gstate.controlSerial.readStringUntil('\n');
      processSerialUnit(CONTROL, line);
      if (!line.startsWith("ALIVE>")) {
        Serial.print("CT: ");
        Serial.println(line);
      }
    }
    vTaskDelay(portTICK_PERIOD_MS);
  }
}

void runnerTask(void *pvParameters)
{
  while (true)
  {
    gstate.display.run();
    gstate.dfplayer.loop();
    taskYIELD(); // Does not delay, but allows other tasks to run
  }
}