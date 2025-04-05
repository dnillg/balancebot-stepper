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
  DiagTriggerListener(DiagSender *ds) : SerialUnitListener(SerialUnitAlias::TRIG), diagSender(ds) {}
  void consume(ISerialUnit *unit) override
  {
    if (unit->getAlias() == SerialUnitAlias::TRIG)
    {
      auto diagSerialUnit = static_cast<TriggerSerialUnit *>(unit);
      auto diagMode = diagSerialUnit->getUserData();
      diagMode.trim();
      diagMode.toUpperCase();
      if (diagMode == "ROLL")
      {
        diagSender->setMode(DiagMode::DIAG_MODE_ROLL);
      } else if(diagMode == "SPEED")
      {
        diagSender->setMode(DiagMode::DIAG_MODE_SPEED);
      } else if(diagMode == "OFF")
      {
        diagSender->setMode(DiagMode::DIAG_MODE_OFF);
      } else {
        Serial.println("Unknown Diag Trigger Type: " + diagMode);
      }
    }
  }
};

#endif