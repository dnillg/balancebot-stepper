#include "Control.hpp"
#include "config.h"

Control::Control() 
: rollPID(&input.rollRadAngle, &rollOutput, &rollSetpoint, CONTROL_ROLL_PID_KP, CONTROL_ROLL_PID_KI, CONTROL_ROLL_PID_KD, DIRECT),
  speedPID(&input.speedStepAvg, &speedPIDOutput, &targetStep16Speed, CONTROL_SPEED_PID_KP, CONTROL_SPEED_PID_KI, CONTROL_SPEED_PID_KD, DIRECT),
  stepperDampener(CONTROL_FREQUENCY, CONTROL_MAX_ACCERATION)
 {
    speedPID.SetMode(AUTOMATIC);
    speedPID.SetOutputLimits(-CONTROL_MAX_TILT_RAD_OFFSET, CONTROL_MAX_TILT_RAD_OFFSET);
    speedPID.SetSampleTime(1000 / CONTROL_FREQUENCY);

    rollPID.SetMode(AUTOMATIC);
    rollPID.SetOutputLimits(-CONTROL_MAX_STEP16_SPEED, CONTROL_MAX_STEP16_SPEED);
    rollPID.SetSampleTime(1000 / CONTROL_FREQUENCY);
}

void Control::setRollParams(double kp, double ki, double kd) {
    rollPID.SetTunings(kp, ki, kd);
}

void Control::setSpeedParams(double kp, double ki, double kd) {
    rollPID.SetTunings(kp, ki, kd);
}

void Control::compute() {
    // input.speedStepAvg avg speed is already set
    speedPID.Compute();
    rollSetpoint = speedPIDOutput + CONTROL_TARGET_ROLL;
    rollPID.Compute();
    motorOutput.speedLeft = stepperDampener.getDampenedSpeed(motorOutput.speedLeft, rollOutput - input.steerProportion * CONTROL_MAX_STEER_STEP16_OFFSET, MOTOR_MAX_MICROSTEPPING);
    motorOutput.speedRight = stepperDampener.getDampenedSpeed(motorOutput.speedRight, rollOutput - input.steerProportion * CONTROL_MAX_STEER_STEP16_OFFSET, MOTOR_MAX_MICROSTEPPING);
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

void Control::setInputTargetSpeedProportion(double speed) {
    input.targetSpeedProportion = speed;
    targetStep16Speed = speed * CONTROL_MAX_STEP16_SPEED;
}

void Control::setInputSteerProportion(double steer) {
    input.steerProportion = steer;
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

double Control::normalizeStep16Speed(double speed) {
    if (MOTOR_MAX_MICROSTEPPING == 16) {
        return speed;
    }
    return speed * 16 / MOTOR_MAX_MICROSTEPPING;
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