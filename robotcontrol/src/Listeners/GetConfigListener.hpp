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
public:
  GetConfigListener(HardwareSerial *serial, Motors *m) : serial(serial), motors(m), SerialUnitListener(SerialUnitAlias::GETCONF) {}
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
  String getConfig(String name) {
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
    else if (name == "TMC_IHOLDD")
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
    else if (name == "MAX_MOT_ACCELERATION")
    {
      return String(motors->getMaxAcceleration());
    }
    
    return "UNKNOWN";
  }
};

#endif