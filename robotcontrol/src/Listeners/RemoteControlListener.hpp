#ifndef CONTROLLISTENER_HPP_
#define CONTROLLISTENER_HPP_

#include <Arduino.h>

#include "SerialUnits/SerialUnitProcessor.hpp"
#include "SerialUnits/SerialUnits.hpp"
#include "Control.hpp"

class RemoteControlListener : public SerialUnitListener
{
private:
  Control *control;
public:
  RemoteControlListener(Control *control) : control(control), SerialUnitListener(SerialUnitAlias::CTRL) {}
  void consume(ISerialUnit *unit) override;
};

#endif