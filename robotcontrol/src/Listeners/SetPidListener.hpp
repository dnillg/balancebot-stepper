#ifndef SETPIDLISTENER_HPP_
#define SETPIDLISTENER_HPP_

#include <Arduino.h>

#include "SerialUnits/SerialUnitProcessor.hpp"
#include "SerialUnits/SerialUnits.hpp"
#include "Control.hpp"

class SetPidListener : public SerialUnitListener
{
private:
  Control *control;
public:
  SetPidListener(Control *control) : SerialUnitListener(SerialUnitAlias::SETPID), control(control) {}
  void consume(ISerialUnit *unit) override
  {
    auto setPidSerialUnit = static_cast<SetPidSerialUnit*>(unit);
    PidParams params = {setPidSerialUnit->getP(), setPidSerialUnit->getI(), setPidSerialUnit->getD()};
    String type = setPidSerialUnit->getType();
    if (type == "ROLL")
    {
      control->setRollPidParams(params);
      Serial.println("Set Roll PID: " + String(params.kp) + ", " + String(params.ki) + ", " + String(params.kd));
    } else if(type == "SPEED") {
      control->setSpeedPidParams(params);
      Serial.println("Set Speed PID: " + String(params.kp) + ", " + String(params.ki) + ", " + String(params.kd));
    } else {
      Serial.println("Unknown GETPID type: " + type);
    }
  }
};

#endif