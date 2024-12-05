#ifndef MOTOROUTPUT_HPP_
#define MOTOROUTPUT_HPP_

#include <Arduino.h>
#include <config.h>

class MotorOutput {
  public:
    int32_t speedLeft = 0;
    int32_t speedRight = 0;
    int32_t speed() {
      return (speedLeft + speedRight) / 2;
    }
};

#endif // MOTOROUTPUT_HPP_