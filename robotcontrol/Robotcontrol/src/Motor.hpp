#include <TMCStepper.h>
#include <AccelStepper.h>

#include "config.h"

#ifndef MOTOR_H_
#define MOTOR_H_

struct MotorPins
{
  uint8_t dir;
  uint8_t stp;
  uint8_t clk;
  uint8_t miso;
  uint8_t cs;
  uint8_t sck;
  uint8_t mosi;
  uint8_t en;
};

enum Direction {
  FORWARD = 1,
  BACKWARD = -1
};

class Motor
{
private:
  void (*timerFunction)(void);
  hw_timer_t * stepTimer = NULL;
  uint8_t enablePin;
  Direction direction;
public:
  TMC5160Stepper driver; //TODO: private
  AccelStepper stepper;  //TODO: private
  Motor(MotorPins pins, void (*timerFunction)(void), uint8_t timerIdx, Direction direction);
  void init();
  void run() {
    stepper.runSpeed();
  };
  void setSpeed(int16_t speed);
  void disable();
  void enable();
  uint32_t getStatus();
  uint8_t getEncStatus();
  void logStatus();
};

#endif // MOTOR_H_