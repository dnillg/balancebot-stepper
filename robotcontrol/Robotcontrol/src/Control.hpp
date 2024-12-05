#ifndef CONTROL_HPP_
#define CONTROL_HPP_

#include <Arduino.h>
#include <PID_v1.h>

#include "models/RemoteControl.hpp"
#include "models/ControlInput.hpp"
#include "models/MotorOutput.hpp"

class Control
{
private:
  ControlInput input;
  MotorOutput motorOutput;
  // Speed PID
  PID speedPID;
  double targetStep16Speed;
  double speedPIDOutput;
  // Roll PID
  PID rollPID;
  double rollSetpoint;
  double rollOutput;
  double normalizeStep16Speed(double speed);
  uint16_t cycleNo = 0;
public:
  Control();
  void setRollParams(double kp, double ki, double kd);
  void setSpeedParams(double kp, double ki, double kd);
  void setInputAngleRad(double angle);
  void setInputSpeedAvg(double speed);
  void setInputTargetSpeedProportion(double speed);
  void setInputSteerProportion(double steer);
  void compute();
  int16_t getSteps16Left();
  int16_t getSteps16Right();
  double getRollSetpoint();
  double getRollOutput();
  double getSpeedPIDOutput();
  uint16_t getCycleNo();
};

#endif // CONTROL_HPP_