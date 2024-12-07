#include "Dampener.hpp"

Dampener::Dampener(uint16_t controlFrequency, double maxAccPerSec)
{
  this->controlPeriodMillis = 1000 / controlFrequency;
  this->maxAccPerSec = maxAccPerSec;
  this->maxAccPerPeriod = (uint32_t) maxAccPerSec / controlFrequency;
}

double Dampener::dampen(double prev, double current)
{
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