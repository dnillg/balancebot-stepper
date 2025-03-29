#ifndef CONTROL_HPP_
#define CONTROL_HPP_

#include <Arduino.h>
#include <PID_v1.h>

#include "models/ControlInput.hpp"
#include "models/MotorOutput.hpp"
#include "Dampener.hpp"

enum ControlMode
{
  REMOTE_CONTROL,
  IDLE
};

class Control
{
private:
  bool enabled = false;
  ControlMode mode = IDLE;
  ControlInput input;
  MotorOutput motorOutput;
  Dampener speedPidOutputDampener;
  Dampener rollPidOutputDampener;
  // Speed PID
  PID speedPID;
  double speedPIDOutput;
  // Roll PID
  PID rollPID;
  double rollSetpoint = CONTROL_TARGET_ROLL;
  double rollOutput;
  inline double normalizeStep16Speed(double speed) {
    if (MOTOR_MAX_MICROSTEPPING == 16) {
      return speed;
    }
    return speed * 16 / MOTOR_MAX_MICROSTEPPING;
  }
  uint16_t cycleNo = 0;
  uint16_t millis = 0;

public:
  Control();
  void enable() {
    this->enabled = true;
  }
  void disable() {
    this->enabled = false;
  }
  void setRollParams(double kp, double ki, double kd);
  void setRollSetpoint(double rollSetpoint);
  void setSpeedParams(double kp, double ki, double kd);
  inline void setRemoteControlInput(double speed, double steer)
  {
    this->input.targetSpeedProportion = speed;
    this->input.steerProportion = steer;
    this->input.step16SpeedSetpoint = speed * CONTROL_MAX_STEP16_SPEED;
    this->input.steerOffset = steer * CONTROL_MAX_STEER_STEP16_OFFSET;
  }
  void setInputAngleRad(double angle);
  void setInputSpeedAvg250(double speed);
  void setInputSpeedAvg500(double speed);
  void setMaxSpeedPidOutputRad(double offset);
  void compute();
  int16_t getSteps16Left();
  int16_t getSteps16Right();
  double getRollSetpoint();
  double getRollOutput();
  double getSpeedPIDOutput();
  MotorOutput &getMotorOutput();
  uint16_t getCycleNo();
  uint16_t getMillis();
  void printPidValues();
  void setControlMode(ControlMode mode);
};

#endif // CONTROL_HPP_