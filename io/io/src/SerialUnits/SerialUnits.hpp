#ifndef SERIALUNITS_HPP_
#define SERIALUNITS_HPP_

#include <Arduino.h>
#include <map>
#include <string>

// Enum for Serial Unit Endpoints
enum SerialUnitEndpoint
{
  CONTROL,
  IO,
  BLUETOOTH
};

// Enum for Serial Unit Types
enum SerialUnitAlias
{
  UNKNOWN_SERIAL_UNIT, // Unknown serial unit
  PARSE_ERROR,         // Error parsing a message
  DIAG,                // Periodic diagnostic message
  CTRL,                // Control message for acceleration and steering
  GETPID,              // Get PID values
  GETPIDRSP,           // Response to GETPID
  SETPID,              // Set PID values
  MOTTOG,              // Toggle motor power
  TRIG,                // Trigger an event
  NOOP                 // No operation, not a serial unit
};

// Mapping Enum to String
extern const std::map<SerialUnitAlias, std::string> SerialUnitAliasMap;

// Base Interface for Serial Units
class ISerialUnit
{
public:
  virtual SerialUnitAlias getAlias() = 0;
  virtual String toString() = 0;
};

// Derived Classes
class NoopSerialUnit : public ISerialUnit
{
public:
  SerialUnitAlias getAlias() override { return NOOP; }
  String toString() override { return "NOOP"; }
};

class UnknownSerialUnit : public ISerialUnit
{
private:
  String value;

public:
  static const String LINE_PREFIX;
  UnknownSerialUnit(String value);
  SerialUnitAlias getAlias() override;
  String toString() override;
};

class ParseErrorSerialUnit : public ISerialUnit
{
private:
  String line;

public:
  static const String LINE_PREFIX;
  ParseErrorSerialUnit(String line);
  SerialUnitAlias getAlias() override;
  String toString() override;
};

class DiagSerialUnit : public ISerialUnit
{
private:
  uint16_t sequenceNumber;
  float currentRoll, targetRoll, currentSpeed, targetSpeed;
  float motorLeftOutput, motorRightOutput, steerInput, speedInput;

public:
  static const String LINE_PREFIX;
  DiagSerialUnit(uint16_t, float, float, float, float, float, float, float, float);
  SerialUnitAlias getAlias() override;
  String toString() override;
  static ISerialUnit *fromLine(String line);
  uint16_t getSequenceNumber() { return sequenceNumber; }
  float getCurrentRoll() { return currentRoll; }
  float getTargetRoll() { return targetRoll; }
  float getCurrentSpeed() { return currentSpeed; }
  float getTargetSpeed() { return targetSpeed; }
  float getMotorLeftOutput() { return motorLeftOutput; }
  float getMotorRightOutput() { return motorRightOutput; }
  float getSteerInput() { return steerInput; }
  float getSpeedInput() { return speedInput; }
};

class DG1SerialUnit : public ISerialUnit
{
private:
  uint16_t sequenceNumber;
  float currentRoll, targetRoll;
public:
  static const String LINE_PREFIX;
  DG1SerialUnit(uint16_t, float, float);
  SerialUnitAlias getAlias() override;
  String toString() override;
  static ISerialUnit *fromLine(String line);
  uint16_t getSequenceNumber() { return sequenceNumber; }
  float getCurrentRoll() { return currentRoll; }
  float getTargetRoll() { return targetRoll; }
};

class ControlSerialUnit : public ISerialUnit
{
private:
  float speed, steer;

public:
  static const String LINE_PREFIX;
  ControlSerialUnit(float, float);
  SerialUnitAlias getAlias() override;
  String toString() override;
  static ISerialUnit *fromLine(String line);
  float getSpeed() { return speed; }
  float getSteer() { return steer; }
};

class TriggerSerialUnit : public ISerialUnit
{
private:
  String type, userData;

public:
  static const String LINE_PREFIX;
  TriggerSerialUnit(String, String);
  SerialUnitAlias getAlias() override;
  String toString() override;
  static ISerialUnit *fromLine(String line);
  String getType() { return type; }
  String getUserData() { return userData; }
};

class GetPidSerialUnit : public ISerialUnit
{
private:
  String type;

public:
  static const String LINE_PREFIX;
  GetPidSerialUnit(String);
  SerialUnitAlias getAlias() override;
  String toString() override;
  static ISerialUnit *fromLine(String line);
  String getType() { return type; }
};

class GetPidResponseSerialUnit : public ISerialUnit
{
private:
  String type;
  float p, i, d;

public:
  static const String LINE_PREFIX;
  GetPidResponseSerialUnit(String, float, float, float);
  SerialUnitAlias getAlias() override;
  String toString() override;
  static ISerialUnit *fromLine(String line);
  String getType() { return type; }
  float getP() { return p; }
  float getI() { return i; }
  float getD() { return d; }
};

class SetPidSerialUnit : public ISerialUnit
{
private:
  String type;
  float p, i, d;

public:
  static const String LINE_PREFIX;
  SetPidSerialUnit(String, float, float, float);
  SerialUnitAlias getAlias() override;
  String toString() override;
  static ISerialUnit *fromLine(String line);
  String getType() { return type; }
  float getP() { return p; }
  float getI() { return i; }
  float getD() { return d; }  
};

class MotorToggleSerialUnit : public ISerialUnit
{
private:
  int state;

public:
  static const String LINE_PREFIX;
  MotorToggleSerialUnit(int);
  SerialUnitAlias getAlias() override;
  String toString() override;
  static ISerialUnit *fromLine(String line);
  int getState() { return state; }
};

class SerialUnitFactory
{
public:
  static ISerialUnit *fromLine(const String &line);
  static SerialUnitAlias readAlias(const String &line);
};

#endif // SERIALUNITS_HPP_