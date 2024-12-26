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
#include "SerialUnitReader.hpp"
#include "CommandExecutor.hpp"
#include "Soul.hpp"

// ----------------------------------------------------------------------------
// Function Declarations
// ----------------------------------------------------------------------------

void halt();

// Tasks
void readBluetoothSerial(void *pvParameters);
void readControlSerial(void *pvParameters);
void executorTask(void *pvParameters);

// ----------------------------------------------------------------------------
// Global Variables
// ----------------------------------------------------------------------------

constexpr EspSoftwareSerial::Config swSerialConfig = EspSoftwareSerial::SWSERIAL_8N1;

typedef DFMiniMp3<EspSoftwareSerial::UART, DfMp3NoCallback> DfMp3;

struct GlobalState
{
  HardwareSerial controlSerial;
  BluetoothSerial serialBT;
  SerialUnitRouter router;
  SerialUnitReader reader;
  CommandExecutor executor;
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
        reader(),
        executor(),
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
  gstate.controlSerial.begin(CONTROL_SERIAL_BAUD);
  Serial.begin(921600);
  delay(100);
  if (!gstate.serialBT.begin(BLUETOOTH_NAME))
  {
    Serial.println("Failed to initialize Bluetooth!");
    halt();
  }
  Serial.println("Bluetooth device started, ready to pair with name: " + String(BLUETOOTH_NAME));

  SPISD.begin(PIN_CARD_SCK, PIN_CARD_MISO, PIN_CARD_MOSI, PIN_CARD_CS);
  Serial.println("HSPI initialized");
  if (!SD.begin(PIN_CARD_CS, SPISD, 4000000))
  {
    Serial.println("SD card initialization failed!");
    return;
  }
  Serial.println("SD card initialized.");

  gstate.tft.init();
  Serial.println("TFT initialized");

  gstate.display.playGif(gifPath);
  gstate.tft.fillScreen(TFT_BLACK);
  Serial.println("GIF Opened...");
  gstate.tft.setSwapBytes(true);

  xTaskCreatePinnedToCore(
      readBluetoothSerial,
      "bluetoothSerial",
      4048,
      NULL,
      4,
      NULL,
      0);

  xTaskCreatePinnedToCore(
      readControlSerial,
      "controlSerial",
      4048,
      NULL,
      4,
      NULL,
      0);

  xTaskCreatePinnedToCore(
      executorTask,
      "executor",
      6096,
      NULL,
      2,
      NULL,
      1);

  gstate.mp3Serial.begin(9600, swSerialConfig, PIN_MP3_SERIAL_RX, PIN_MP3_SERIAL_TX);
  // gstate.dfplayer.begin();
  // gstate.dfplayer.reset(); 
  // gstate.dfplayer.setPlaybackSource(DfMp3_PlaySource_Sd);
  // gstate.dfplayer.setPlaybackMode(DfMp3_PlaybackMode_SingleRepeat);
  //gstate.dfplayer.start();

  // gstate.dfplayer.setVolume(100);
  // //gstate.dfplayer.enableDac();

  // uint16_t count = gstate.dfplayer.getTotalTrackCount(DfMp3_PlaySource_Sd);
  // Serial.print("files ");
  // Serial.println(count);

  // uint16_t mode = gstate.dfplayer.getPlaybackMode();  
  // Serial.print("playback mode ");
  // Serial.println(mode);
  // gstate.dfplayer.playRandomTrackFromAll();

}

uint32_t counter = 0;

void loop()
{
  //gstate.dfplayer.loop();
  //Serial.println(gstate.dfplayer.getVolume());
}

// ----------------------------------------------------------------------------
// Tasks
// ----------------------------------------------------------------------------

void processLine(SerialUnitEndpoint source, String &line)
{
  SerialUnitAlias alias = gstate.reader.readAlias(line);
  gstate.router.route(source, alias, line);
  ISerialUnit unit = gstate.reader.readUnit(alias, line);
  gstate.executor.process(alias, unit);
}

void readBluetoothSerial(void *pvParameters)
{
  while (true)
  {
    while (gstate.serialBT.available())
    {
      String line = gstate.serialBT.readStringUntil('\n');
      processLine(BLUETOOTH, line);
      Serial.print("BT #");
      Serial.println(line);
    }
    vTaskDelay(portTICK_PERIOD_MS);
  }
}
void readControlSerial(void *pvParameters)
{
  while (true)
  {
    while (gstate.controlSerial.available())
    {
      String line = gstate.controlSerial.readStringUntil('\n');
      processLine(CONTROL, line);
      Serial.print("CT #");
      Serial.println(line);
    }
    vTaskDelay(portTICK_PERIOD_MS);
  }
}

void executorTask(void *pvParameters)
{
  while (true)
  {
    gstate.executor.execute();
    gstate.display.update();
    taskYIELD();
  }
}

void halt()
{
  while (true)
  {
    Serial.println("Halting...");
    delay(1000);
  }
}