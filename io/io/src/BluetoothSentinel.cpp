#include "BluetoothSentinel.hpp"

BluetoothSentinel::BluetoothSentinel(BluetoothSerial &serialBT, SerialUnitProcessor &serialUnitProcessor, SerialUnitRouter &serialUnitRouter)
    : serialBT(serialBT), serialUnitProcessor(serialUnitProcessor), serialUnitRouter(serialUnitRouter) {}

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
      String ctrlLine = ctrl.toString();
      serialUnitProcessor.process(SerialUnitAlias::CTRL, ctrlLine);
      serialUnitRouter.route(SerialUnitEndpoint::BLUETOOTH, SerialUnitAlias::CTRL, ctrlLine);
    }
  }
}