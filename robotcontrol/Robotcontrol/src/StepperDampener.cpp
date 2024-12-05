#include "StepperDampener.hpp"

StepperDampener::StepperDampener(uint16_t controlFrequency, uint16_t maxAccPerSec)
{
  this->controlPeriodMillis = 1000 / controlFrequency;
  this->maxAccPerSec = maxAccPerSec;
  this->maxAccPerPeriod = (uint32_t) maxAccPerSec / controlFrequency;
}

double StepperDampener::getDampenedSpeed(double prev, double current, uint16_t microStepping)
{
  //TODO: Implement microstepping
  if (prev < current && current - prev > maxAccPerPeriod)
  {
    current = prev + maxAccPerPeriod;
  }
  else if (prev > current && prev - current > maxAccPerPeriod)
  {
    current = prev - maxAccPerPeriod;
  }
  return current;
}