#ifndef STATIONARYCUTOFF_HPP_
#define STATIONARYCUTOFF_HPP_

#include <Arduino.h>
#include "SamplesWindow.hpp"

class StationaryCutoff
{
private:
  double multiplier = 1.0;
  SamplesWindow rollWindow;
  SamplesWindow motorSpeedWindow;
  uint8_t counter;
  uint8_t takeEveryNth;
  double multiplierIncrement = 0.01;
  double rollThreshold;
  double motorSpeedThreshold;

public:
  StationaryCutoff(uint8_t windowSize, uint8_t takeEveryNth, double rollThreshold, double motorSpeedThreshold) : rollWindow(windowSize), motorSpeedWindow(windowSize) {
    this->takeEveryNth = takeEveryNth;
    this->counter = 0;
    this->rollThreshold = rollThreshold;
    this->motorSpeedThreshold = motorSpeedThreshold;
  }

  inline void consumeRollAndMotorSpeed(double roll, double speed)
  {
    counter++;
    if (counter % takeEveryNth == 0)
    {
      // Serial.print("StationaryCutoff: Roll: ");
      // Serial.print(roll, 4);
      // Serial.print(", Speed: ");
      // Serial.println(speed);
      counter = 0;
      rollWindow.consume(roll);
      motorSpeedWindow.consume(speed);
    }
    adjustMultiplier();
  }

  inline void adjustMultiplier()
  {
    double multiplierBefore = multiplier;
    double rollRange = rollWindow.calcRange();

    if (rollRange > rollThreshold)
    {
      multiplier += multiplierIncrement;
    }
    else if (motorSpeedWindow.calcRange() < motorSpeedThreshold && !(motorSpeedWindow.getMin() < 0 && motorSpeedWindow.getMax() > 0))
    {
      multiplier -= multiplierIncrement;
    }

    if (multiplier < 0.0)
    {
      multiplier = 0.0;
    }
    if (multiplier > 1.0)
    {
      multiplier = 1.0;
    }

    if (multiplierBefore >= 1.0 && multiplier < 1.0)
    {
      Serial.println("StationaryCutoff: Starting cutoff.");
    }
    if (multiplierBefore <= 0.0 && multiplier > 0.0)
    {
      Serial.println("StationaryCutoff: Stopping cutoff.");
    }

  }

  inline double filter(double value)
  {
    return value * multiplier;
  }
};

#endif // STATIONARYCUTOFF_HPP_