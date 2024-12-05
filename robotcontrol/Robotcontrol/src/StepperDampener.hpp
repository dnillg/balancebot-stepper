#ifndef STEPPERDAMPENER_HPP_
#define STEPPERDAMPENER_HPP_

#include <Arduino.h>

class StepperDampener
{
private:
  uint16_t controlPeriodMillis;
  uint16_t maxAccPerSec;
  uint16_t maxAccPerPeriod;

public:
  StepperDampener(uint16_t controlFrequency, uint16_t maxAccPerSec);
  double getDampenedSpeed(double prev, double current, uint16_t microStepping);
  uint16_t getMaxAccPerPeriod() {
    return maxAccPerPeriod;
  }
};

#endif // STEPPERDAMPENER_HPP_