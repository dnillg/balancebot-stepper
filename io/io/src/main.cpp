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

#include "DfConfig.h"
#include "config.h"
#include "SerialUnits/SerialUnitProcessor.hpp"
#include "Soul.hpp"
#include "Listeners/SequenceTriggerListener.hpp"
#include "esp_system.h"
#include "esp_err.h"
#include "BluetoothSentinel.hpp"
//#include <WiFi.h>

// ----------------------------------------------------------------------------
// Function Declarations
// ----------------------------------------------------------------------------

void halt();

// Tasks
void bluetoothSerialReaderTask(void *pvParameters);
void controlSerialReaderTask(void *pvParameters);
void runnerTask(void *pvParameters);
void bluetoothSentinelTask(void *pvParameters);

// ----------------------------------------------------------------------------
// Global Variables
// ----------------------------------------------------------------------------

struct GlobalState
{
  HardwareSerial controlSerial;
  BluetoothSerial serialBT;

  SerialUnitProcessor serialUnitProcessor;
  EspSoftwareSerial::UART mp3Serial;
  TFT_eSPI tft;
  RobotDisplay display;
  Soul soul;
  DfMp3 dfplayer;
  BluetoothSentinel btSentinel;

  GlobalState()
      : controlSerial(2),
        serialBT(),
        serialUnitProcessor(),
        tft(TFT_eSPI(240, 240)),
        mp3Serial(PIN_MP3_SERIAL_RX, PIN_MP3_SERIAL_TX),
        display(tft),
        dfplayer(mp3Serial),
        soul(&dfplayer, &display),
        btSentinel(serialBT, serialUnitProcessor, controlSerial)
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
const char *gifPath2 = "/pedro-4x.gif";

void setup()
{
  setCpuFrequencyMhz(160);
  delay(10);
  // esp_log_level_set("*", ESP_LOG_INFO);  // Set log level for verbosity
  delay(10);
  //WiFi.mode(WIFI_OFF);  

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

  Serial.println("Initializing DFPlayer.");
  gstate.mp3Serial.begin(9600, EspSoftwareSerial::SWSERIAL_8N1, PIN_MP3_SERIAL_RX, PIN_MP3_SERIAL_TX);
  delay(100);
  gstate.dfplayer.begin();
  gstate.dfplayer.reset(); 
  gstate.dfplayer.setPlaybackSource(DfMp3_PlaySource_Sd);
  gstate.dfplayer.setPlaybackMode(DfMp3_PlaybackMode::DfMp3_PlaybackMode_Repeat);
  gstate.dfplayer.setVolume(12);
  Serial.println("DFPlayer initialized.");

  gstate.tft.init();
  Serial.println("TFT initialized");

  gstate.tft.fillScreen(TFT_BLACK);
  gstate.tft.setSwapBytes(true);

  gstate.serialUnitProcessor.addListener(new SequenceTriggerListener(&gstate.soul));
  gstate.soul.setSequence(SequenceType::ROBOT_FACE_STANDARD);

  xTaskCreatePinnedToCore(
      bluetoothSerialReaderTask,
      "bluetoothSerial",
      4096,
      NULL,
      6,
      NULL,
      1);

  xTaskCreatePinnedToCore(
      controlSerialReaderTask,
      "controlSerial",
      4096,
      NULL,
      6,
      NULL,
      1);

  xTaskCreatePinnedToCore(
      runnerTask,
      "executor",
      6096,
      NULL,
      2,
      NULL,
      1);

  xTaskCreatePinnedToCore(
      bluetoothSentinelTask,
      "bluetoothSentinel",
      2048,
      NULL,
      4,
      NULL,
      1);

  Serial.println("Setup complete.");
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
        gstate.controlSerial.println(line);
        processSerialUnit(BLUETOOTH, line);
        #if LOG_BT_REC == true
        Serial.print("BT-REC: ");
        Serial.println(line);
        #endif
      } catch (const std::exception& e) {
        Serial.println("ERROR: Could not read bluetooth serial. Reason: " + String(e.what()));
      }
    }
    vTaskDelay(portTICK_PERIOD_MS);
  }
}

void controlSerialReaderTask(void *pvParameters)
{
  uint8_t buffer[64];
  while (true)
  {
    while (gstate.controlSerial.available())
    {
      //String line = gstate.controlSerial.readStringUntil('\n');
      //processSerialUnit(CONTROL, line);
      size_t read = gstate.controlSerial.readBytes(buffer, 64);
      gstate.serialBT.write(buffer, read);
    }
    vTaskDelay(portTICK_PERIOD_MS);
  }
}

void runnerTask(void *pvParameters)
{
  while (true)
  {
    gstate.serialUnitProcessor.run();
    gstate.soul.run();
    gstate.display.run();
    gstate.dfplayer.loop();
    taskYIELD(); // Does not delay, but allows other tasks to run
  }
}

void bluetoothSentinelTask(void *pvParameters)
{
  while (true) {
    gstate.btSentinel.run();
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}