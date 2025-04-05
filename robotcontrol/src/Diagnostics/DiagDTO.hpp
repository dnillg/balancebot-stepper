#ifndef DIAGDTO_HPP_
#define DIAGDTO_HPP_

#include <Arduino.h>

struct DiagDTO
{
  uint16_t cycleNo = 0;
  uint16_t millis = 0;
  double roll = 0.0;
  double rollSetpoint = 0.0;
  double speed = 0.0;
  double speedSetpoint = 0.0;
  double steerOffset = 0.0;
  int16_t step16SpeedLeft = 0;
  int16_t step16SpeedRight = 0;
};

#endif