#ifndef MOTORS_HPP_
#define MOTORS_HPP_

#include "Motor.hpp"

class Motors {
private:
    Motor *leftMotor;
    Motor *rightMotor;
  public:
    Motors(Motor *leftMotor, Motor *rightMotor) : leftMotor(leftMotor), rightMotor(rightMotor) {}
    void init() {
      leftMotor->init();
      rightMotor->init();
    }
    void run() {
      leftMotor->run();
      rightMotor->run();
    }
    void setSpeed(int16_t leftSpeed, int16_t rightSpeed) {
      leftMotor->setSpeed(leftSpeed);
      rightMotor->setSpeed(rightSpeed);
    }
    void enable() {
      leftMotor->enable();
      rightMotor->enable();
    }
    void disable() {
      leftMotor->disable();
      rightMotor->disable();
    }
};

#endif // MOTORS_HPP_0