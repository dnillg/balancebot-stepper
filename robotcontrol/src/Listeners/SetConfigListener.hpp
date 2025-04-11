#ifndef SETCONFIGLISTENER_HPP_
#define SETCONFIGLISTENER_HPP_

#include <Arduino.h>
#include "SerialUnits/SerialUnitProcessor.hpp"
#include "Motor/Motor.hpp"
#include "Motor/Motors.hpp"

class SetConfigListener : public SerialUnitListener
{
private:
  HardwareSerial *serial;
  Motors *motors;
  Control *control;

public:
  SetConfigListener(HardwareSerial *serial, Motors *m, Control *c) : serial(serial), motors(m), control(c), SerialUnitListener(SerialUnitAlias::SETCONF) {}
  void consume(ISerialUnit *unit) override
  {
    if (unit->getAlias() == SerialUnitAlias::SETCONF)
    {
      auto setConfigSerialUnit = static_cast<SetConfigSerialUnit *>(unit);
      setConfig(setConfigSerialUnit->getName(), setConfigSerialUnit->getValue());
      // Serial.println(response);
      // this->serial->println(response);
    }
  }
  String setConfig(String name, String value)
  {
    if (name == "TMC_TOFF")
    {
      motors->setTOFF(value.toInt());
    }
    else if (name == "TMC_RMS_CURRENT")
    {
      motors->setRMSCurrent(value.toInt());
    }
    else if (name == "TMC_IHOLD")
    {
      motors->setIHOLD(value.toInt());
    }
    else if (name == "TMC_IRUN")
    {
      motors->setIRUN(value.toInt());
    }
    else if (name == "TMC_IHOLDDELAY")
    {
      motors->setIHOLDD(value.toInt());
    }
    else if (name == "TMC_PWM_AUTOSCALE")
    {
      motors->setPWMAutoscale(toBoolean(value));
    }
    else if (name == "TMC_PWM_MODE")
    {
      motors->setPWMMode(toBoolean(value));
    }
    else if (name == "MAX_MOT_SPEED")
    {
      motors->setMaxSpeed(value.toFloat());
    }
    else if (name == "MAX_MOT_ACC")
    {
      motors->setMaxAcceleration(value.toFloat());
    }
    else if (name == "BALANCE_ROLL")
    {
      control->setBalanceRoll(value.toFloat());
    }
    else if (name == "MAX_TARGET_SPEED")
    {
      control->setMaxTargetSpeed(value.toFloat());
    }
    else if (name == "MAX_STEER_OFFSET")
    {
      control->setMaxSteerOffset(value.toFloat());
    }
    else if (name == "ROLL_PID_OUTPUT_LIMIT")
    {
      control->setRollPidOutputLimit(value.toFloat());
    }
    else if (name == "SPEED_PID_OUTPUT_LIMIT")
    {
      control->setSpeedPidOutputLimit(value.toFloat());
    }
    else if (name == "ROLL_PID_OUTPUT_ACC")
    {
      control->setMaxRollPidOutputAcceleration(value.toFloat());
    }
    else if (name == "SPEED_PID_OUTPUT_ACC")
    {
      control->setMaxSpeedPidOutputAcceleration(value.toFloat());
    }

    return "UNKNOWN";
  }
  static bool toBoolean(String value)
  {
    if (value == "true" || value == "1")
    {
      return true;
    }
    else if (value == "false" || value == "0")
    {
      return false;
    }
    else
    {
      Serial.println("Invalid boolean value: " + value);
      return false; // Default to false for invalid input
    }
  }
};

#endif