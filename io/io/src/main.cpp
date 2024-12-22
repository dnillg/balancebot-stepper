#include <Wire.h>
#include <Arduino.h>
#include <BluetoothSerial.h>

#include "config.h"
#include "SerialUnitRouter.hpp"
#include "SerialUnitReader.hpp"
#include "CommandExecutor.hpp"

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

struct GlobalState
{
  HardwareSerial controlSerial = HardwareSerial(2);
  BluetoothSerial serialBT;
  SerialUnitRouter router = SerialUnitRouter(serialBT, controlSerial);
  SerialUnitReader reader = SerialUnitReader();
  CommandExecutor executor = CommandExecutor();
};
GlobalState gstate;

// ----------------------------------------------------------------------------
// Setup
// ----------------------------------------------------------------------------

void setup()
{
  gstate.controlSerial.begin(CONTROL_SERIAL_BAUD);
  Serial.begin(921600);
  if (!gstate.serialBT.begin(BLUETOOTH_NAME))
  {
    Serial.println("Failed to initialize Bluetooth!");
    halt();
  }
  Serial.println("Bluetooth device started, ready to pair with name: " + String(BLUETOOTH_NAME));

  xTaskCreatePinnedToCore(
      readBluetoothSerial,
      "bluetoothSerial",
      1024,
      NULL,
      2,
      NULL,
      1);

  xTaskCreatePinnedToCore(
      readControlSerial,
      "controlSerial",
      1024,
      NULL,
      2,
      NULL,
      1);

  xTaskCreatePinnedToCore(
      executorTask,
      "executor",
      1024,
      NULL,
      2,
      NULL,
      1);
}

void loop()
{
  // Nothing to do here
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
      String line = gstate.controlSerial.readStringUntil('\n');
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
    vTaskDelay(portTICK_PERIOD_MS);
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