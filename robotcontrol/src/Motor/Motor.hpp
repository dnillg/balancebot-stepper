#include <TMCStepper.h>
#include <AccelStepper.h>

#include "config.h"
#include "MotorPosition.hpp"

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

struct TMC5160Config {
  uint8_t toff = MOTOR_TOFF;
  uint16_t rms_current = MOTOR_RMS_CURRENT;
  uint8_t ihold = MOTOR_IHOLD;
  uint8_t irun = MOTOR_IRUN;
  uint8_t iholddelay = MOTOR_IHOLDDELAY;
  bool pwm_autoscale = MOTOR_PWM_AUTOSCALE;
  bool pwm_mode = MOTOR_PWM_MODE;
  float max_speed = MOTOR_MAX_SPEED;
  float max_acceleration = MOTOR_MAX_ACCELERATION;
};

class Motor
{
private:
  MotorPosition position;
  void (*timerFunction)(void);
  hw_timer_t * stepTimer = NULL;
  uint8_t enablePin;
  Direction direction;
public:
  TMC5160Stepper driver; //TODO: private
  AccelStepper stepper;  //TODO: private
  Motor(MotorPins pins, void (*timerFunction)(void), uint8_t timerIdx, Direction direction, MotorPosition motorPos);
  void init(const TMC5160Config& config);
  void applyConfig(const TMC5160Config& config);
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