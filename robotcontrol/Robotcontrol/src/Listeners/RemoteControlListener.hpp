#ifndef CONTROLLISTENER_HPP_
#define CONTROLLISTENER_HPP_

#include <Arduino.h>

#include "../SerialUnits/SerialUnitProcessor.hpp"
#include "../SerialUnits/SerialUnits.hpp"
#include "../Control.hpp"
#include "../MotorOutputFilters/SteeringMotorOutputFilter.hpp"

class RemoteControlListener : public SerialUnitListener
{
private:
  Control *control;
  SteeringMotorOutputFilter* steeringMotorOutputFilter;
public:
  RemoteControlListener(Control *control, SteeringMotorOutputFilter* steeringFilter) : control(control), steeringMotorOutputFilter(steeringFilter), SerialUnitListener(SerialUnitAlias::CTRL) {}
  void consume(ISerialUnit *unit) override;
};

#endif