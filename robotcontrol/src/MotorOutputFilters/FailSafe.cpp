#include "FailSafe.hpp"

FailSafeMotorOutputFilter::FailSafeMotorOutputFilter(uint16_t ms)
{
  this->thresholdMs = ms;
  this->lastHeartBeat = millis();
  this->currentRoll = -1000.0; // Initialize to an invalid value
}

bool FailSafeMotorOutputFilter::hadRecentHeartBeat()
{
  auto current = millis();
  return (current - this->lastHeartBeat) <= this->thresholdMs || current < this->lastHeartBeat; // Handle overflow
}

void FailSafeMotorOutputFilter::heartBeat()
{
  this->lastHeartBeat = millis();
}

bool FailSafeMotorOutputFilter::isRollValid()
{
  return (this->currentRoll < (CONTROL_TARGET_ROLL + FAILSAFE_MAX_ROLL_OFFSET)) && (this->currentRoll > (CONTROL_TARGET_ROLL - FAILSAFE_MAX_ROLL_OFFSET));
}

double FailSafeMotorOutputFilter::filter(double input, MotorPosition motorPos)
{
  bool status = hadRecentHeartBeat() && isRollValid();

  if (status && !lastStatus)
  {
    Serial.println("FailSafe: OK");
  }
  else if (!status && lastStatus)
  {
    Serial.println("FailSafe: FAIL");
  }
  lastStatus = status;

  if (status)
  {
    return input;
  }
  else
  {
    return 0.0; // or some other default value
  }
}