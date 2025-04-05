#ifndef GETPIDLISTENER_HPP_
#define GETPIDLISTENER_HPP_

#include <Arduino.h>

#include "SerialUnits/SerialUnitProcessor.hpp"
#include "SerialUnits/SerialUnits.hpp"
#include "Control.hpp"

class SetPidListener : public SerialUnitListener
{
private:
  Control *control;
public:
  SetPidListener(Control *control, HardwareSerial* serial) : SerialUnitListener(SerialUnitAlias::SETPID), control(control) {}
  void consume(ISerialUnit *unit) override
  {
    auto setPidSerialUnit = static_cast<SetPidSerialUnit*>(unit);
    PidParams params = {setPidSerialUnit->getP(), setPidSerialUnit->getI(), setPidSerialUnit->getD()};
    String type = setPidSerialUnit->getType();
    if (type == "ROLL")
    {
      control->setRollPidParams(params);
    } else if(type == "SPEED") {
      control->setSpeedPidParams(params);
    } else {
      Serial.println("Unknown GETPID type: " + type);
    }
  }
};

#endif