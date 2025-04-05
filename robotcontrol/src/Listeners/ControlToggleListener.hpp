#ifndef ControlToggleListener_hpp_
#define ControlToggleListener_hpp_

#include <Arduino.h>

#include "../SerialUnits/SerialUnitProcessor.hpp"
#include "../Control.hpp"
#include "../Motors.hpp"

class ControlToggleListener : public SerialUnitListener
{
private:
  Control *control;
  Motors *motors;
public:
  ControlToggleListener(Control *control, Motors* motors) : control(control), motors(motors), SerialUnitListener(SerialUnitAlias::MOTTOG) {}
  void consume(ISerialUnit *unit) override
  {
    if (unit->getAlias() == SerialUnitAlias::MOTTOG) {
      auto ctrlSerialUnit = static_cast<MotorToggleSerialUnit *>(unit);
      if (ctrlSerialUnit->getState() == 1)
      {
        this->control->reset();
        this->control->enable();
        this->motors->enable();
        Serial.println("Motors and Control enabled.");
      }
      else
      {
        this->control->disable();
        this->motors->disable();
        Serial.println("Motors and Control disabled.");
      }
    }
  }

};

#endif