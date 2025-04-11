#include "Dampener.hpp"

Dampener::Dampener(uint16_t controlFrequency, double maxAccPerSec) : controlFrequency(controlFrequency)
{
  this->maxAccPerSec = maxAccPerSec;
  this->maxAccPerPeriod = ((double) maxAccPerSec) / controlFrequency;
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