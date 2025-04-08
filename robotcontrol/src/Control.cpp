#include "Control.hpp"
#include "config.h"

Control::Control()
    : rollPID(&input.rollRadAngle, &rollOutput, &rollSetpoint, CONTROL_ROLL_PID_KP, CONTROL_ROLL_PID_KI, CONTROL_ROLL_PID_KD, DIRECT),
      speedPID(&input.speedStepAvg500, &speedPIDOutput, &input.step16SpeedSetpoint, CONTROL_SPEED_PID_KP, CONTROL_SPEED_PID_KI, CONTROL_SPEED_PID_KD, DIRECT),
      rollPidOutputDampener(CONTROL_FREQUENCY, CONTROL_MAX_ACCELERATION),
      speedPidOutputDampener(CONTROL_FREQUENCY, CONTROL_MAX_SPEED_PID_OUTPUT_ACCELERATION)
{
  speedPID.SetMode(AUTOMATIC);
  speedPID.SetOutputLimits(-CONTROL_MAX_SPEED_ROLL_RAD_OFFSET, CONTROL_MAX_SPEED_ROLL_RAD_OFFSET);
  rollPID.SetMode(AUTOMATIC);
  rollPID.SetOutputLimits(-CONTROL_MAX_STEP16_SPEED, CONTROL_MAX_STEP16_SPEED);
  rollPID.SetSampleTime(1000 / CONTROL_FREQUENCY);
  speedPID.SetSampleTime(1000 / CONTROL_FREQUENCY);
}

void Control::setRollPidParams(double kp, double ki, double kd)
{
  rollPID.SetTunings(kp, ki, kd);
  rollPID.resetOutputSum();
}

void Control::setSpeedPidParams(double kp, double ki, double kd)
{
  speedPID.SetTunings(kp, ki, kd);
  speedPID.resetOutputSum();
}

constexpr uint16_t millisPerCycle = 1000 / CONTROL_FREQUENCY;

void Control::compute()
{
  if (!enabled) {
    return;
  }
  if (resetFlag) {
    rollPID.resetOutputSum();
    speedPID.resetOutputSum();
    resetFlag = false;
  }
  
  speedPID.Compute();
  this->rollSetpoint = speedPidOutputDampener.dampen(rollSetpoint, CONTROL_TARGET_ROLL - speedPIDOutput);
  rollPID.Compute();
  motorOutput.speedLeft = rollPidOutputDampener.dampen(motorOutput.speedLeft, rollOutput + input.steerOffset);
  motorOutput.speedRight = rollPidOutputDampener.dampen(motorOutput.speedRight, rollOutput - input.steerOffset);

  cycleNo++;
  if (cycleNo >= 1000)
  {
    cycleNo = 0;
  }
  millis += millisPerCycle;
  if (millis >= 1000)
  {
    millis -= 1000;
  }
}

void Control::setInputAngleRad(double angle)
{
  input.rollRadAngle = angle;
}

void Control::setInputSpeedAvg250(double speed)
{
  input.speedStepAvg250 = normalizeStep16Speed(speed);
}

void Control::setInputSpeedAvg500(double speed)
{
  input.speedStepAvg500 = antioscillation(normalizeStep16Speed(speed));
}

int16_t Control::getSteps16Left()
{
  return motorOutput.speedLeft;
}

int16_t Control::getSteps16Right()
{
  return motorOutput.speedRight;
}

MotorOutput &Control::getMotorOutput()
{
  return motorOutput;
}

double Control::getRollSetpoint()
{
  return rollSetpoint;
}

double Control::getRollOutput()
{
  return rollOutput;
}

double Control::getSpeedPIDOutput()
{
  return speedPIDOutput;
}

uint16_t Control::getCycleNo()
{
  return cycleNo;
}

uint16_t Control::getMillis()
{
  return millis;
}

void Control::setRollSetpoint(double rollSetpoint)
{
  this->rollSetpoint = rollSetpoint;
}

void Control::setMaxSpeedPidOutputRad(double offset)
{
  speedPID.SetOutputLimits(-offset, offset);
}

void Control::printPidValues()
{
  Serial.print("Roll PID initialized with: ");
  Serial.print(rollPID.GetKp(), 6);
  Serial.print(", ");
  Serial.print(rollPID.GetKi(), 6);
  Serial.print(", ");
  Serial.println(rollPID.GetKd(), 6);
  Serial.print("Speed PID initialized with: ");
  Serial.print(speedPID.GetKp(), 6);
  Serial.print(", ");
  Serial.print(speedPID.GetKi(), 6);
  Serial.print(", ");
  Serial.println(speedPID.GetKd(), 6);
}

void Control::setControlMode(ControlMode mode)
{
  this->mode = mode;
  // Set Speed PID
}