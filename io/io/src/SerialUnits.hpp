#ifndef SERIALUNITS_HPP_
#define SERIALUNITS_HPP_

#include <Arduino.h>

#include <map>

enum SerialUnitEndpoint
{
  CONTROL,
  IO,
  BLUETOOTH
};

enum SerialUnitAlias
{
  UNKNOWN_SERIAL_UNIT,
  DIAG, // Periodic diagnostic message
  CTRL, // Control message for acceleration and steering
  GETPID, // Get PID values
  GETPIDRSP, // Response to GETPID
  SETPID, // Set PID values
  MOTTOG, // Toggle motor power
  TRIG, // Trigger an event
  
};

class ISerialUnit
{
};

struct ControlSerialUnit : public ISerialUnit
{
  float speed = 0.0f;
  float steer = 0.0f;
};

struct TriggerSerialUnit : public ISerialUnit
{
  String type = "";
};

class NoopSerialUnit : public ISerialUnit
{
};

#endif 