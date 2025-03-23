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
    UnknownSerialUnit(String value);
    SerialUnitAlias getAlias() override;
    String toString() override;
};

class ParseErrorSerialUnit : public ISerialUnit
{
private:
    String line;

public:
    ParseErrorSerialUnit(String line);
    SerialUnitAlias getAlias() override;
    String toString() override;
};

class DiagSerialUnit : public ISerialUnit
{
private:
    float currentRoll, targetRoll, currentSpeed, targetSpeed;
    float motorLeftOutput, motorRightOutput, steerInput, speedInput;

public:
    DiagSerialUnit(float, float, float, float, float, float, float, float);
    SerialUnitAlias getAlias() override;
    String toString() override;
    static ISerialUnit* fromLine(String line);
};

class ControlSerialUnit : public ISerialUnit
{
private:
    float speed, steer;

public:
    ControlSerialUnit(float, float);
    SerialUnitAlias getAlias() override;
    String toString() override;
    static ISerialUnit* fromLine(String line);
};

class TriggerSerialUnit : public ISerialUnit
{
private:
    String type, userData;

public:
    TriggerSerialUnit(String, String);
    const String& getType();
    const String& getUserData();
    SerialUnitAlias getAlias() override;
    String toString() override;
    static ISerialUnit* fromLine(String line);
};

class GetPidSerialUnit : public ISerialUnit
{
private:
    String type;

public:
    GetPidSerialUnit(String);
    SerialUnitAlias getAlias() override;
    String toString() override;
    static ISerialUnit* fromLine(String line);
};

class GetPidResponseSerialUnit : public ISerialUnit
{
private:
    String type;
    float p, i, d;

public:
    GetPidResponseSerialUnit(String, float, float, float);
    SerialUnitAlias getAlias() override;
    String toString() override;
    static ISerialUnit* fromLine(String line);
};

class SetPidSerialUnit : public ISerialUnit
{
private:
    String type;
    float p, i, d;

public:
    SetPidSerialUnit(String, float, float, float);
    SerialUnitAlias getAlias() override;
    String toString() override;
    static ISerialUnit* fromLine(String line);
};

class MotorToggleSerialUnit : public ISerialUnit
{
  private:
    int state;
  public:
    MotorToggleSerialUnit(int);
    SerialUnitAlias getAlias() override;
    String toString() override;
    static ISerialUnit* fromLine(String line);
};

class SerialUnitFactory
{
public:
    static ISerialUnit* fromLine(const String& line);
    static SerialUnitAlias readAlias(const String& line);
};

#endif // SERIALUNITS_HPP_
