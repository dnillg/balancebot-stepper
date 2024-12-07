#ifndef DAMPENER_HPP_
#define DAMPENER_HPP_

#include <Arduino.h>

class Dampener
{
private:
  uint16_t controlPeriodMillis;
  double maxAccPerSec;
  double maxAccPerPeriod;

public:
  Dampener(uint16_t controlFrequency, double maxAccPerSec);
  double dampen(double prev, double current);
  uint16_t getMaxAccPerPeriod() {
    return maxAccPerPeriod;
  }
};

#endif // DAMPENER_HPP_