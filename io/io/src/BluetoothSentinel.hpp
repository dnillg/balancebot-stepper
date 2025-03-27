#ifndef BLUETOOTHSENTINEL_HPP_
#define BLUETOOTHSENTINEL_HPP_

#include <Arduino.h>
#include <BluetoothSerial.h>

#include "SerialUnitProcessor.hpp"
#include "SerialUnitRouter.hpp"

class BluetoothSentinel {
private:
  bool btConnected = false;
  BluetoothSerial &serialBT;
  SerialUnitProcessor& serialUnitProcessor;
  SerialUnitRouter& serialUnitRouter;
public:
  BluetoothSentinel(BluetoothSerial &serialBT, SerialUnitProcessor& serialUnitProcessor, SerialUnitRouter& serialUnitRouter);

  void run();

};

#endif // BLUETOOTHSENTINEL_HPP_