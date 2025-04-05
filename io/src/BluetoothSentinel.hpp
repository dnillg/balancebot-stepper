#ifndef BLUETOOTHSENTINEL_HPP_
#define BLUETOOTHSENTINEL_HPP_

#include <Arduino.h>
#include <BluetoothSerial.h>

#include "SerialUnits/SerialUnitProcessor.hpp"

class BluetoothSentinel {
private:
  bool btConnected = false;
  BluetoothSerial &serialBT;
  SerialUnitProcessor& serialUnitProcessor;
  HardwareSerial& controlSerial;
public:
  BluetoothSentinel(BluetoothSerial &serialBT, SerialUnitProcessor& serialUnitProcessor, HardwareSerial& controlSerial);

  void run();

};

#endif // BLUETOOTHSENTINEL_HPP_