#ifndef LOVEYOULISTENER_HPP_
#define LOVEYOULISTENER_HPP_

#include <Arduino.h>

#include "SerialUnitProcessor.hpp"

class LoveYouListener : public SerialUnitListener
{
public:
  LoveYouListener() : SerialUnitListener(TRIG) {}
  void consume(ISerialUnit* unit) override
  {
    Serial.println("Consuming LoveYouListener");
    TriggerSerialUnit triggerUnit = *((TriggerSerialUnit*)unit);
    Serial.println("Type: " + triggerUnit.getType());
    if (triggerUnit.getType() == "FEELING_LOVE")
    {
      Serial.println("I love you too!");
    }
  }
};

#endif // LOVEYOULISTENER_HPP_