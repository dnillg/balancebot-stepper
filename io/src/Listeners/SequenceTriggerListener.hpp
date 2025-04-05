#ifndef SEQUENCE_TRIGGER_LISTENER_HPP_
#define SEQUENCE_TRIGGER_LISTENER_HPP_

#include <Arduino.h>

#include "../SerialUnits/SerialUnitProcessor.hpp"
#include "Soul.hpp"

class SequenceTriggerListener : public SerialUnitListener
{
private:
  Soul *soul;
public:
  SequenceTriggerListener(Soul *soul);
  void consume(ISerialUnit *unit) override;
};

#endif // SEQUENCE_TRIGGER_LISTENER_HPP_