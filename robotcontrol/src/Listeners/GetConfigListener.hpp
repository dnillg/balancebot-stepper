#ifndef GETCONFIGLISTENER_HPP_
#define GETCONFIGLISTENER_HPP_

#include <Arduino.h>
#include "SerialUnits/SerialUnitProcessor.hpp"
#include "Motor/Motor.hpp"
#include "Motor/Motors.hpp"

class GetConfigListener : public SerialUnitListener
{
private:
  HardwareSerial *serial;
  Motors *motors;
  Control *control;

public:
  GetConfigListener(HardwareSerial *serial, Motors *m, Control *c)
      : serial(serial), motors(m), control(c), SerialUnitListener(SerialUnitAlias::GETCONF) {}
  void consume(ISerialUnit *unit) override
  {
    if (unit->getAlias() == SerialUnitAlias::GETCONF)
    {
      auto getConfigSerialUnit = static_cast<GetConfigSerialUnit *>(unit);
      String response = GetConfigResponseSerialUnit::LINE_PREFIX + getConfigSerialUnit->getName() + "," + getConfig(getConfigSerialUnit->getName());
      Serial.println(response);
      this->serial->println(response);
    }
  }
  String getConfig(String name)
  {
    if (name == "TMC_TOFF")
    {
      return String(motors->getTOFF());
    }
    else if (name == "TMC_RMS_CURRENT")
    {
      return String(motors->getRMSCurrent());
    }
    else if (name == "TMC_IHOLD")
    {
      return String(motors->getIHOLD());
    }
    else if (name == "TMC_IRUN")
    {
      return String(motors->getIRUN());
    }
    else if (name == "TMC_IHOLDDELAY")
    {
      return String(motors->getIHOLDD());
    }
    else if (name == "TMC_PWM_AUTOSCALE")
    {
      return String(motors->getPWMAutoscale());
    }
    else if (name == "TMC_PWM_MODE")
    {
      return String(motors->getPWMMode());
    }
    else if (name == "MAX_MOT_SPEED")
    {
      return String(motors->getMaxSpeed());
    }
    else if (name == "MAX_MOT_ACC")
    {
      return String(motors->getMaxAcceleration());
    }
    else if (name == "BALANCE_ROLL")
    {
      return String(control->getBalanceRoll(), 10);
    }
    else if (name == "MAX_TARGET_SPEED")
    {
      return String(control->getMaxTargetSpeed(), 10);
    }
    else if (name == "MAX_STEER_OFFSET")
    {
      return String(control->getMaxSteerOffset(), 10);
    }
    else if (name == "ROLL_PID_OUTPUT_LIMIT")
    {
      return String(control->getRollPidOutputLimit(), 10);
    }
    else if (name == "SPEED_PID_OUTPUT_LIMIT")
    {
      return String(control->getSpeedPidOutputLimit(), 10);
    }
    else if (name == "ROLL_PID_OUTPUT_ACC")
    {
      return String(control->getMaxRollPidOutputAcceleration(), 10);
    }
    else if (name == "SPEED_PID_OUTPUT_ACC")
    {
      return String(control->getMaxSpeedPidOutputAcceleration(), 10);
    }

    return "UNKNOWN";
  }
};

#endif