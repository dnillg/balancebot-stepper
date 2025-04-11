#ifndef MOTORS_HPP_
#define MOTORS_HPP_

#include "Motor.hpp"

class Motors {
private:
    Motor *leftMotor;
    Motor *rightMotor;
    TMC5160Config config;
  public:
    Motors(Motor *leftMotor, Motor *rightMotor) : leftMotor(leftMotor), rightMotor(rightMotor) {}
    void init() {
      leftMotor->init(config);
      rightMotor->init(config);
    }
    void applyConfig(const TMC5160Config& config) {
      leftMotor->applyConfig(config);
      rightMotor->applyConfig(config);
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
    void setTOFF(uint8_t toff) {
      config.toff = toff;
      applyConfig(config);
    }
    uint8_t getTOFF() {
      return config.toff;
    }
    void setRMSCurrent(uint16_t rms_current) {
      config.rms_current = rms_current;
      applyConfig(config);
    }
    uint16_t getRMSCurrent() {
      return config.rms_current;
    }
    void setIHOLD(uint8_t ihold) {
      config.ihold = ihold;
      applyConfig(config);
    }
    uint8_t getIHOLD() {
      return config.ihold;
    }
    void setIRUN(uint8_t irun) {
      config.irun = irun;
      applyConfig(config);
    }
    uint8_t getIRUN() {
      return config.irun;
    }
    void setIHOLDD(uint8_t iholddelay) {
      config.iholddelay = iholddelay;
      applyConfig(config);
    }
    uint8_t getIHOLDD() {
      return config.iholddelay;
    }
    void setPWMAutoscale(bool pwm_autoscale) {
      config.pwm_autoscale = pwm_autoscale;
      applyConfig(config);
    }
    bool getPWMAutoscale() {
      return config.pwm_autoscale;
    }
    void setPWMMode(bool pwm_mode) {
      config.pwm_mode = pwm_mode;
      applyConfig(config);
    }
    bool getPWMMode() {
      return config.pwm_mode;
    }
    void setMaxSpeed(uint16_t max_speed) {
      config.max_speed = max_speed;
      applyConfig(config);
    }
    uint16_t getMaxSpeed() {
      return config.max_speed;
    }
    void setMaxAcceleration(uint16_t max_acceleration) {
      config.max_acceleration = max_acceleration;
      applyConfig(config);
    }
    uint8_t getMaxAcceleration() {
      return config.max_acceleration;
    }    
};

#endif // MOTORS_HPP_0