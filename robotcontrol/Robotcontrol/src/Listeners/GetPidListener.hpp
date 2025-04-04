#ifndef GETPIDLISTENER_HPP_
#define GETPIDLISTENER_HPP_

#include <Arduino.h>

#include "SerialUnits/SerialUnitProcessor.hpp"
#include "SerialUnits/SerialUnits.hpp"
#include "Control.hpp"

class GetPidListener : public SerialUnitListener
{
private:
  Control *control;
  HardwareSerial *serial;
public:
  GetPidListener(Control *control, HardwareSerial* serial) : SerialUnitListener(SerialUnitAlias::GETPID), control(control), serial(serial) {}
  void consume(ISerialUnit *unit) override
  {
    if (unit->getAlias() == SerialUnitAlias::GETPID)
    {
      auto getPidSerialUnit = static_cast<GetPidSerialUnit *>(unit);
      String type = getPidSerialUnit->getType();
      if (type == "ROLL")
      {
        auto params = control->getRollPidParams();
        GetPidResponseSerialUnit serialUnit = GetPidResponseSerialUnit(type, params.kp, params.ki, params.kd);
        serial->println(serialUnit.toString());
      } else if(type == "SPEED") {
        auto params = control->getSpeedPidParams();
        GetPidResponseSerialUnit serialUnit = GetPidResponseSerialUnit(type, params.kp, params.ki, params.kd);
        serial->println(serialUnit.toString());
      } else {
        Serial.println("Unknown GETPID type: " + type);
      }
    }
  }
};

#endif