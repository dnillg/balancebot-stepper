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

struct PidParams
{
  double kp;
  double ki;
  double kd;
};

struct ControlConfig {
  double balanceRoll = CONTROL_TARGET_ROLL;
  double speedPidOutputLimit = CONTROL_MAX_SPEED_ROLL_RAD_OFFSET;
  uint16_t maxTargetSpeed = CONTROL_MAX_TARGET_STEP16_SPEED;
  double maxSteerOffset = CONTROL_MAX_STEER_STEP16_OFFSET;
  double balanceOutputLimit = CONTROL_MAX_STEP16_SPEED;
};

class Control
{
private:
  ControlConfig config;
  bool enabled = false;
  bool resetFlag = false;
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
  double threshold(double value, double threshold) {
    if (value >= threshold) {
      return value;
    }
    if (value <= -threshold) {
      return value;
    }
    return 0;
  }

public:
  Control();
  void enable() {
    this->enabled = true;
  }
  void disable() {
    this->enabled = false;
  }
  void reset() {
    this->resetFlag = true;
  }
  void setRollPidParams(double kp, double ki, double kd);
  void setRollSetpoint(double rollSetpoint);
  void setSpeedPidParams(double kp, double ki, double kd);
  void setRollPidParams(const PidParams& params) {
    setRollPidParams(params.kp, params.ki, params.kd);
  }
  void setSpeedPidParams(const PidParams& params) {
    setSpeedPidParams(params.kp, params.ki, params.kd);
  }
  PidParams getRollPidParams() {
    PidParams params;
    params.kp = rollPID.GetKp();
    params.ki = rollPID.GetKi();
    params.kd = rollPID.GetKd();
    return params;
  }
  PidParams getSpeedPidParams() {
    PidParams params;
    params.kp = speedPID.GetKp();
    params.ki = speedPID.GetKi();
    params.kd = speedPID.GetKd();
    return params;
  }
  // <Configuration>
  void setBalanceRoll(double roll) {
    this->config.balanceRoll = roll;
  }
  double getBalanceRoll() {
    return this->config.balanceRoll;
  }
  void setSpeedPidOutputLimit(double rads) {
    this->config.speedPidOutputLimit = rads;
    this->speedPID.SetOutputLimits(-rads, rads);
  }
  double getSpeedPidOutputLimit() {
    return this->config.speedPidOutputLimit;
  }
  void setMaxTargetSpeed(double speed) {
    this->config.maxTargetSpeed = speed;
  }
  double getMaxTargetSpeed() {
    return this->config.maxTargetSpeed;
  }
  void setMaxSteerOffset(double offset) {
    this->config.maxSteerOffset = offset;
  }
  double getMaxSteerOffset() {
    return this->config.maxSteerOffset;
  }
  double getMaxRollPidOutputAcceleration() {
    return this->rollPidOutputDampener.getMaxAccPerSec();
  }
  void setMaxRollPidOutputAcceleration(double maxAcc) {
    this->rollPidOutputDampener.setMaxAccPerSec(maxAcc);
  }
  double getMaxSpeedPidOutputAcceleration() {
    return this->speedPidOutputDampener.getMaxAccPerSec();
  }
  void setMaxSpeedPidOutputAcceleration(double maxAcc) {
    this->speedPidOutputDampener.setMaxAccPerSec(maxAcc);
  }
  void setRollPidOutputLimit(double limit) {
    this->config.balanceOutputLimit = limit;
    this->rollPID.SetOutputLimits(-limit, limit);
  }
  double getRollPidOutputLimit() {
    return this->config.balanceOutputLimit;
  }
  // </Configuration>
  inline void setRemoteControlInput(double speed, double steer)
  {
    this->input.targetSpeedProportion = speed;
    this->input.steerProportion = steer;
    this->input.step16SpeedSetpoint = threshold(speed, 0.15) * config.maxTargetSpeed;
    this->input.steerOffset = threshold(steer, 0.25) * config.maxSteerOffset;
  }
  const ControlInput& getInput() const {
    return input;
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
  double antioscillation(double step16Speed) {
    if (step16Speed < 25) {
      return step16Speed * 0.25;
    } else if (step16Speed < 50) {
      return step16Speed * 0.5;
    } else if (step16Speed < 75) {
      return step16Speed * 0.75;
    } else if (step16Speed < 100) {
      return step16Speed * 0.85;
    } 
    return step16Speed;
  }
};

#endif // CONTROL_HPP_