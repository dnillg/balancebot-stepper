#include <TMCStepper.h>
#include <AccelStepper.h>

#include "config.h"

#ifndef MOTOR_H_
#define MOTOR_H_

struct MotorConfig
{
  public:
    float rSense = MOTOR_R_SENSE;
    uint8_t toff = MOTOR_TOFF;
    uint16_t rmsCurrent = MOTOR_RMS_CURRENT;
    bool pwmAutoscale = MOTOR_PWM_AUTOSCALE;
    bool pwmMode = MOTOR_PWM_MODE;
    uint16_t maxSpeed = MOTOR_MAX_SPEED;
    uint16_t maxAcceleration = MOTOR_MAX_ACCELERATION;
};

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

class Motor
{
private:
  void (*timerFunction)(void);
  hw_timer_t * stepTimer = NULL;
  uint8_t enablePin;
public:
  TMC5160Stepper driver; //TODO: private
  AccelStepper stepper;  //TODO: private
  Motor(MotorConfig config, MotorPins pins, void (*timerFunction)(void), uint8_t timerIdx);
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