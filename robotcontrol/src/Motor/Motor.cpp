#include "Motor.hpp"

Motor::Motor(MotorPins pins, void (*timerFunction)(void), uint8_t timerIdx, Direction direction, MotorPosition motorPos) : driver(pins.cs, MOTOR_R_SENSE, pins.mosi, pins.miso, pins.sck),
                                                                                                                           stepper(1, pins.stp, pins.dir),
                                                                                                                           position(motorPos)
{
  this->enablePin = pins.en;
  this->timerFunction = timerFunction;
  this->stepTimer = timerBegin(timerIdx, 80, true); // 80 cycles: 1us, TODO: check if this is correct
  this->direction = direction;
}

void Motor::init(const TMC5160Config& config)
{
  driver.begin();
  driver.microsteps(MOTOR_MAX_MICROSTEPPING);
  stepper.setPinsInverted(direction == BACKWARD, false, true);
  applyConfig(config);
  timerAttachInterrupt(this->stepTimer, timerFunction, true);
  timerAlarmWrite(this->stepTimer, 1000, true); // 1000: 1ms
  timerAlarmEnable(this->stepTimer);
}

void Motor::applyConfig(const TMC5160Config& config)
{
  //  TOFF: 3-5 recommended, 0-15
  //  high: stable, lower power, low: noisy
  driver.toff(config.toff);
  driver.rms_current(config.rms_current);
  driver.irun(config.irun);
  driver.ihold(config.ihold);
  driver.iholddelay(config.iholddelay);
  // driver.TCOOLTHRS(0);
  // driver.TPWMTHRS(500);
  // driver.THIGH(500);

  driver.pwm_autoscale(config.pwm_autoscale);

  driver.en_pwm_mode(config.pwm_mode); // Toggle stealthChop

  stepper.setMaxSpeed(config.max_speed);
  stepper.setAcceleration(config.max_acceleration);
}

void Motor::setSpeed(int16_t speed)
{
  if (speed == stepper.speed())
  {
    return;
  }

  stepper.setSpeed(speed);
  if (speed == 0)
  {
    timerAlarmWrite(stepTimer, 1000, true); // 1ms
    return;
  }
  // Adjusting the timer according to the new speed
  uint32_t ticks = 1000000;
  ticks /= abs(speed);
  ticks /= 10; // Triggering 10 times per step
  if (ticks < 66)
  {
    ticks = 66;
  }
  timerAlarmWrite(stepTimer, ticks, true);
}

void Motor::disable()
{
  digitalWrite(enablePin, HIGH);
}

void Motor::enable()
{
  if (!CONTROL_DRY_RUN)
  {
    digitalWrite(enablePin, LOW);
  }
}

uint32_t Motor::getStatus()
{
  return driver.DRV_STATUS();
}

uint8_t Motor::getEncStatus()
{
  driver.ENC_STATUS();
}

void Motor::logStatus()
{
  Serial.print("SG_RESULT: ");
  Serial.println(driver.sg_result());

  Serial.print("FS_ACTIVE: ");
  Serial.println(driver.fsactive());

  Serial.print("CS_ACTUAL: ");
  Serial.println(driver.cs_actual());

  Serial.print("STALLGUARD: ");
  Serial.println(driver.stallguard());

  Serial.print("Overtemp (OT): ");
  Serial.println(driver.ot());

  Serial.print("Overtemp Warning (OTPW): ");
  Serial.println(driver.otpw());

  Serial.print("Short to Ground A (S2GA): ");
  Serial.println(driver.s2ga());

  Serial.print("Short to Ground B (S2GB): ");
  Serial.println(driver.s2gb());

  Serial.print("Open Load A (OLA): ");
  Serial.println(driver.ola());

  Serial.print("Open Load B (OLB): ");
  Serial.println(driver.olb());

  Serial.print("Standstill (STST): ");
  Serial.println(driver.stst());
}