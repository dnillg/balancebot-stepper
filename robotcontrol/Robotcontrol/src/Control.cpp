#include "Control.hpp"
#include "config.h"

Control::Control() 
: rollPID(&input.rollRadAngle, &rollOutput, &rollSetpoint, CONTROL_ROLL_PID_KP, CONTROL_ROLL_PID_KI, CONTROL_ROLL_PID_KD, DIRECT),
  speedPID(&input.speedStepAvg, &speedPIDOutput, &input.step16SpeedSetpoint, CONTROL_SPEED_PID_KP, CONTROL_SPEED_PID_KI, CONTROL_SPEED_PID_KD, DIRECT),
  rollPidOutputDampener(CONTROL_FREQUENCY, CONTROL_MAX_ACCELERATION),
  speedPidOutputDampener(CONTROL_FREQUENCY, CONTROL_MAX_SPEED_PID_OUTPUT_ACCELERATION)
 {
    speedPID.SetMode(AUTOMATIC);
    speedPID.SetOutputLimits(-CONTROL_MAX_SPEED_TILT_RAD_OFFSET, CONTROL_MAX_SPEED_TILT_RAD_OFFSET);
    speedPID.SetSampleTime(1000 / CONTROL_FREQUENCY);

    rollPID.SetMode(AUTOMATIC);
    rollPID.SetOutputLimits(-CONTROL_MAX_STEP16_SPEED, CONTROL_MAX_STEP16_SPEED);
    rollPID.SetSampleTime(1000 / CONTROL_FREQUENCY);
}

void Control::setRollParams(double kp, double ki, double kd) {
    rollPID.SetTunings(kp, ki, kd);
}

void Control::setSpeedParams(double kp, double ki, double kd) {
    speedPID.SetTunings(kp, ki, kd);
}

void Control::compute() {
    speedPID.Compute();
    this->rollSetpoint = speedPidOutputDampener.dampen(rollSetpoint, CONTROL_TARGET_ROLL - speedPIDOutput);
    rollPID.Compute();
    motorOutput.speedLeft = rollPidOutputDampener.dampen(motorOutput.speedLeft, rollOutput + input.steerOffset);
    motorOutput.speedRight = rollPidOutputDampener.dampen(motorOutput.speedRight, rollOutput - input.steerOffset);

    cycleNo++;
    if (cycleNo >= 1000) {
        cycleNo = 0;
    }
}

void Control::setInputAngleRad(double angle) {
    input.rollRadAngle = angle;
}

void Control::setInputSpeedAvg(double speed) {
    input.speedStepAvg = normalizeStep16Speed(speed);
}

int16_t Control::getSteps16Left() {
    return motorOutput.speedLeft;
}

int16_t Control::getSteps16Right() {
    return motorOutput.speedRight;
}

MotorOutput& Control::getMotorOutput() {
    return motorOutput;
}

double Control::getRollSetpoint() {
    return rollSetpoint;
}

double Control::getRollOutput() {
    return rollOutput;
}

double Control::getSpeedPIDOutput() {
    return speedPIDOutput;
}

uint16_t Control::getCycleNo() {
    return cycleNo;
}

void Control::setRollSetpoint(double rollSetpoint) {
    this->rollSetpoint = rollSetpoint;
}

void Control::setMaxSpeedPidOutputRad(double offset) {
    speedPID.SetOutputLimits(-offset, offset);
}