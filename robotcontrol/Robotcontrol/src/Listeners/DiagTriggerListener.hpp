#ifndef DIAGTRIGGERLISTENER_HPP_
#define DIAGTRIGGERLISTENER_HPP_

#include <Arduino.h> 

#include "SerialUnits/SerialUnits.hpp"
#include "Diagnostics/DiagSender.hpp"
#include "SerialUnits/SerialUnitProcessor.hpp"


class DiagTriggerListener : public SerialUnitListener
{
private:
  DiagSender *diagSender;
public:
  DiagTriggerListener(DiagSender *DiagSerialUnit) : SerialUnitListener(SerialUnitAlias::TRIG), diagSender(diagSender) {}
  void consume(ISerialUnit *unit) override
  {
    if (unit->getAlias() == SerialUnitAlias::TRIG)
    {
      auto diagSerialUnit = static_cast<TriggerSerialUnit *>(unit);
      Serial.println("Diag Trigger: " + diagSerialUnit->getType());
      auto type = diagSerialUnit->getType();
      if (type == "ROLL")
      {
        diagSender->setMode(DiagMode::DIAG_MODE_ROLL);
      } else if(type == "SPEED")
      {
        diagSender->setMode(DiagMode::DIAG_MODE_SPEED);
      } else if(type == "OFF")
      {
        diagSender->setMode(DiagMode::DIAG_MODE_OFF);
      } else
      {
        Serial.println("Unknown Diag Trigger Type: " + type);
      }
    }
  }
};

#endif