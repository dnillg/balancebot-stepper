#include "BluetoothSentinel.hpp"

BluetoothSentinel::BluetoothSentinel(BluetoothSerial &serialBT, SerialUnitProcessor &serialUnitProcessor, HardwareSerial &controlSerial)
    : serialBT(serialBT), serialUnitProcessor(serialUnitProcessor), controlSerial(controlSerial) {}

void BluetoothSentinel::run()
{
  if (serialBT.hasClient())
  {
    if (!btConnected)
    {
      Serial.println("Bluetooth connected");
      btConnected = true;
    }
  }
  else
  {
    if (btConnected)
    {
      Serial.println("Bluetooth disconnected");
      btConnected = false;
      ControlSerialUnit ctrl(0, 0);
      controlSerial.println(ctrl.toString());
      
    }
  }
}