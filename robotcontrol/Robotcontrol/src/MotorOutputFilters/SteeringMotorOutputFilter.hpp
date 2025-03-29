#ifndef STEERINGMOTOROUTPUTFILTER_HPP_
#define STEERINGMOTOROUTPUTFILTER_HPP_

#include <Arduino.h>

#include "MotorOutputFilter.hpp"

class SteeringMotorOutputFilter : public MotorOutputFilter
{
private:
  double steeringIntensity = 0.0;
  double steeringOffset;
public:
  SteeringMotorOutputFilter(double steeringOffset) : steeringOffset(steeringOffset) {}
  double filter(double input, MotorPosition motorPos) override;
  void setSteeringIntensity(double intensity);
};

#endif