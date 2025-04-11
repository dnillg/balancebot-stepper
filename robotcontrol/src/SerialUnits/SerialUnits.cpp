#include "SerialUnits.hpp"
#include <cstdio>
#include "Utils.hpp"

// Mapping Enum to String
const std::map<SerialUnitAlias, std::string> SerialUnitAliasMap = {
    {UNKNOWN_SERIAL_UNIT, "UNKNOWN_SERIAL_UNIT"},
    {PARSE_ERROR, "PARSE_ERROR"},
    {DIAG, "DIAG"},
    {CTRL, "CTRL"},
    {GETPID, "GETPID"},
    {GETPIDRSP, "GETPIDRSP"},
    {SETPID, "SETPID"},
    {MOTTOG, "MOTTOG"},
    {TRIG, "TRIG"},
    {DG1, "DG1"},
    {DG2, "DG2"},
    {SETCONF, "SETCONF"},
    {GETCONF, "GETCONF"},
    {GETCONFRSP, "GETCONFRSP"},
    {NOOP, "NOOP"}
  };

const String DiagSerialUnit::LINE_PREFIX = "DIAG>";
const String DG1SerialUnit::LINE_PREFIX = "DG1>";
const String DG2SerialUnit::LINE_PREFIX = "DG2>";
const String ControlSerialUnit::LINE_PREFIX = "CTRL>";
const String TriggerSerialUnit::LINE_PREFIX = "TRIG>";
const String GetPidSerialUnit::LINE_PREFIX = "GETPID>";
const String GetPidResponseSerialUnit::LINE_PREFIX = "GETPIDRSP>";
const String SetPidSerialUnit::LINE_PREFIX = "SETPID>";
const String MotorToggleSerialUnit::LINE_PREFIX = "MOTTOG>";
const String UnknownSerialUnit::LINE_PREFIX = "UNKNOWN>";
const String ParseErrorSerialUnit::LINE_PREFIX = "PARSE_ERROR>";
const String SetConfigSerialUnit::LINE_PREFIX = "SETCONF>";
const String GetConfigSerialUnit::LINE_PREFIX = "GETCONF>";
const String GetConfigResponseSerialUnit::LINE_PREFIX = "GETCONFRSP>";

// Unknown Serial Unit
UnknownSerialUnit::UnknownSerialUnit(String value) : value(value) {}
SerialUnitAlias UnknownSerialUnit::getAlias() { return UNKNOWN_SERIAL_UNIT; }
String UnknownSerialUnit::toString() { return LINE_PREFIX + value; }

// Parse Error Serial Unit
ParseErrorSerialUnit::ParseErrorSerialUnit(String line) : line(line) {}
SerialUnitAlias ParseErrorSerialUnit::getAlias() { return PARSE_ERROR; }
String ParseErrorSerialUnit::toString() { return LINE_PREFIX + line; }

// Diag Serial Unit
DiagSerialUnit::DiagSerialUnit(uint16_t sqn, float cr, float tr, float cs, float ts, float ml, float mr, float si, float sp)
    : sequenceNumber(sqn), currentRoll(cr), targetRoll(tr), currentSpeed(cs), targetSpeed(ts),
      motorLeftOutput(ml), motorRightOutput(mr), steerInput(si), speedInput(sp) {}

SerialUnitAlias DiagSerialUnit::getAlias() { return DIAG; }
String DiagSerialUnit::toString()
{
  return LINE_PREFIX + String(sequenceNumber) + "," + String(currentRoll, 3) + "," + String(targetRoll, 3) + "," +
         String(currentSpeed, 3) + "," + String(targetSpeed, 3) + "," +
         String(motorLeftOutput, 3) + "," + String(motorRightOutput, 3) + "," +
         String(steerInput, 3) + "," + String(speedInput, 3);
}

ISerialUnit *DiagSerialUnit::fromLine(String line)
{
  if (!line.startsWith(LINE_PREFIX))
  {
    return new ParseErrorSerialUnit(line); // Invalid format
  }

  uint16_t sequenceNumber;
  float values[8] = {0}; // Initialize all values to 0

  int parsed = sscanf(line.c_str() + LINE_PREFIX.length(), "%d,%f,%f,%f,%f,%f,%f,%f,%f",
                      &sequenceNumber,
                      &values[0], &values[1], &values[2], &values[3],
                      &values[4], &values[5], &values[6], &values[7]);

  if (parsed >= 1)
  { // Accept at least 1 valid float
    return new DiagSerialUnit(sequenceNumber, values[0], values[1], values[2], values[3],
                              values[4], values[5], values[6], values[7]);
  }

  return new ParseErrorSerialUnit(line); // Parsing failed
}

// DG1 Serial Unit
DG1SerialUnit::DG1SerialUnit(uint16_t sqn, float cr, float tr) : sequenceNumber(sqn), currentRoll(cr), targetRoll(tr) {}
SerialUnitAlias DG1SerialUnit::getAlias() { return DIAG; }
String DG1SerialUnit::toString()
{
  return LINE_PREFIX + String(sequenceNumber) + "," + String(currentRoll, 3) + "," + String(targetRoll, 3);
}
ISerialUnit *DG1SerialUnit::fromLine(String line)
{
  // Not implemented
  return new ParseErrorSerialUnit(line);
}

// DG2 Serial Unit
DG2SerialUnit::DG2SerialUnit(uint16_t sqn, float cs, float ts) : sequenceNumber(sqn), speed(cs), targetSpeed(ts) {}
SerialUnitAlias DG2SerialUnit::getAlias() { return DG2; }
String DG2SerialUnit::toString()
{
  return LINE_PREFIX + String(sequenceNumber) + "," + String(speed, 3) + "," + String(targetSpeed, 3);
}
ISerialUnit *DG2SerialUnit::fromLine(String line)
{
  // Not implemented
  return new ParseErrorSerialUnit(line);
}

// Control Serial Unit
ControlSerialUnit::ControlSerialUnit(float spd, float str) : speed(spd), steer(str) {}
SerialUnitAlias ControlSerialUnit::getAlias() { return CTRL; }
String ControlSerialUnit::toString() { return LINE_PREFIX + String(speed) + "," + String(steer); }

ISerialUnit *ControlSerialUnit::fromLine(String line)
{
  if (!line.startsWith(LINE_PREFIX))
  {
    return new ParseErrorSerialUnit(line); // Invalid format
  }
  try {
    String nonPrefixLine = line.substring(LINE_PREFIX.length());
    int index = nonPrefixLine.indexOf(',');
    if (index == -1) {
      return new ParseErrorSerialUnit(line);
    }
    auto spd = nonPrefixLine.substring(index + 1).toFloat();
    auto str = nonPrefixLine.substring(0, index).toFloat();  
    return new ControlSerialUnit(spd, str);
  } catch (const std::exception &e) {
    Serial.println("Error parsing ControlSerialUnit: " + String(e.what()));
    return new ParseErrorSerialUnit(line);
  }
}

// Trigger Serial Unit
TriggerSerialUnit::TriggerSerialUnit(String t, String ud) : type(t), userData(ud) {}
SerialUnitAlias TriggerSerialUnit::getAlias() { return TRIG; }
String TriggerSerialUnit::toString() { return LINE_PREFIX + type + "," + userData; }

ISerialUnit *TriggerSerialUnit::fromLine(String line)
{
  if (!line.startsWith(LINE_PREFIX))
  {
    return new ParseErrorSerialUnit(line); // Invalid format
  }

  char type[50], userData[50] = {0};
  if (sscanf(line.c_str() + LINE_PREFIX.length(), "%49[^,],%49[^\n]", type, userData) >= 1)
  {
    return new TriggerSerialUnit(String(type), String(userData));
  }
  return new ParseErrorSerialUnit(line);
}

// GetPid Serial Unit
GetPidSerialUnit::GetPidSerialUnit(String t) : type(t) {}
SerialUnitAlias GetPidSerialUnit::getAlias() { return GETPID; }
String GetPidSerialUnit::toString() { return LINE_PREFIX + type; }

ISerialUnit *GetPidSerialUnit::fromLine(String line)
{
  if (!line.startsWith(LINE_PREFIX))
  {
    return new ParseErrorSerialUnit(line); // Invalid format
  }

  char type[50];
  if (sscanf(line.c_str() + LINE_PREFIX.length(), "%49s", type) == 1)
  {
    return new GetPidSerialUnit(String(type));
  }
  return new ParseErrorSerialUnit(line);
}

// Toggle Motor Serial Unit
MotorToggleSerialUnit::MotorToggleSerialUnit(int state) : state(state) {}
SerialUnitAlias MotorToggleSerialUnit::getAlias() { return MOTTOG; }
String MotorToggleSerialUnit::toString() { return LINE_PREFIX + String(state); }

ISerialUnit *MotorToggleSerialUnit::fromLine(String line)
{
  if (!line.startsWith(LINE_PREFIX))
  {
    return new ParseErrorSerialUnit(line); // Invalid format
  }

  int state;
  if (sscanf(line.c_str() + LINE_PREFIX.length(), "%d", &state) == 1)
  {
    return new MotorToggleSerialUnit(state);
  }
  return new ParseErrorSerialUnit(line);
}

// GetPidResponse Serial Unit
GetPidResponseSerialUnit::GetPidResponseSerialUnit(String t, float p, float i, float d) : type(t), p(p), i(i), d(d) {}
SerialUnitAlias GetPidResponseSerialUnit::getAlias() { return GETPIDRSP; }
String GetPidResponseSerialUnit::toString() { return LINE_PREFIX + type + "," + String(p, 10) + "," + String(i, 10) + "," + String(d, 10); }

ISerialUnit *GetPidResponseSerialUnit::fromLine(String line)
{
  if (!line.startsWith(LINE_PREFIX))
  {
    return new ParseErrorSerialUnit(line); // Invalid format
  }

  char type[50];
  float p, i, d;
  if (sscanf(line.c_str() + LINE_PREFIX.length(), "%49[^,],%f,%f,%f", type, &p, &i, &d) == 4)
  {
    return new GetPidResponseSerialUnit(type, p, i, d);
  }
  return new ParseErrorSerialUnit(line);
}

// SetPid Serial Unit
SetPidSerialUnit::SetPidSerialUnit(String t, float p, float i, float d) : type(t), p(p), i(i), d(d) {}
SerialUnitAlias SetPidSerialUnit::getAlias() { return SETPID; }
String SetPidSerialUnit::toString() { return LINE_PREFIX + type + "," + String(p, 10) + "," + String(i, 10) + "," + String(d, 10); }

ISerialUnit *SetPidSerialUnit::fromLine(String line)
{
  if (!line.startsWith(LINE_PREFIX))
  {
    return new ParseErrorSerialUnit(line); // Invalid format
  }

  char type[50];
  float p, i, d;
  if (sscanf(line.c_str() + LINE_PREFIX.length(), "%49[^,],%f,%f,%f", type, &p, &i, &d) == 4)
  {
    return new SetPidSerialUnit(String(type), p, i, d);
  }

  return new ParseErrorSerialUnit(line);
}

SetConfigSerialUnit::SetConfigSerialUnit(String name, String v) : name(name), value(v) {}
SerialUnitAlias SetConfigSerialUnit::getAlias() { return SETCONF; }
String SetConfigSerialUnit::toString() { return LINE_PREFIX + name + "," + value; }

ISerialUnit *SetConfigSerialUnit::fromLine(String line)
{
  if (!line.startsWith(LINE_PREFIX))
  {
    return new ParseErrorSerialUnit(line); // Invalid format
  }

  char name[50], value[50] = {0};
  if (sscanf(line.c_str() + LINE_PREFIX.length(), "%49[^,],%49[^\n]", name, value) >= 1)
  {
    return new SetConfigSerialUnit(String(name), String(value));
  }
  return new ParseErrorSerialUnit(line);
}

GetConfigSerialUnit::GetConfigSerialUnit(String name) : name(name) {}
SerialUnitAlias GetConfigSerialUnit::getAlias() { return GETCONF; }
String GetConfigSerialUnit::toString() { return LINE_PREFIX + name; }

ISerialUnit *GetConfigSerialUnit::fromLine(String line)
{
  if (!line.startsWith(LINE_PREFIX))
  {
    return new ParseErrorSerialUnit(line); // Invalid format
  }

  char name[50];
  if (sscanf(line.c_str() + LINE_PREFIX.length(), "%49s", name) == 1)
  {
    return new GetConfigSerialUnit(String(name));
  }
  return new ParseErrorSerialUnit(line);
}

GetConfigResponseSerialUnit::GetConfigResponseSerialUnit(String name, String value) : name(name), value(value) {}
SerialUnitAlias GetConfigResponseSerialUnit::getAlias() { return GETCONF; }
String GetConfigResponseSerialUnit::toString() { return LINE_PREFIX + name + "," + value; }

ISerialUnit *GetConfigResponseSerialUnit::fromLine(String line)
{
  if (!line.startsWith(LINE_PREFIX))
  {
    return new ParseErrorSerialUnit(line); // Invalid format
  }

  char name[50], value[50] = {0};
  if (sscanf(line.c_str() + LINE_PREFIX.length(), "%49[^,],%49[^\n]", name, value) >= 1)
  {
    return new GetConfigResponseSerialUnit(String(name), String(value));
  }
  return new ParseErrorSerialUnit(line);
}


ISerialUnit *SerialUnitFactory::fromLine(const String &line)
{
  SerialUnitAlias alias = readAlias(line);

  if (alias == DIAG)
  {
    return DiagSerialUnit::fromLine(line);
  }
  else if (alias == CTRL)
  {
    return ControlSerialUnit::fromLine(line);
  }
  else if (alias == TRIG)
  {
    return TriggerSerialUnit::fromLine(line);
  }
  else if (alias == GETPID)
  {
    return GetPidSerialUnit::fromLine(line);
  }
  else if (alias == GETPIDRSP)
  {
    return GetPidResponseSerialUnit::fromLine(line);
  }
  else if (alias == SETPID)
  {
    return SetPidSerialUnit::fromLine(line);
  }
  else if (alias == MOTTOG)
  {
    return MotorToggleSerialUnit::fromLine(line);
  }
  else if (alias == SETCONF)
  {
    return SetConfigSerialUnit::fromLine(line);
  }
  else if (alias == GETCONF)
  {
    return GetConfigSerialUnit::fromLine(line);
  }
  else if (alias == GETCONFRSP)
  {
    return GetConfigResponseSerialUnit::fromLine(line);
  }
  else if (alias == DG1)
  {
    return DG1SerialUnit::fromLine(line);
  }
  else if (alias == DG2)
  {
    return DG2SerialUnit::fromLine(line);
  }

  Serial.println("Unknown Serial Unit: " + line);
  return new NoopSerialUnit();
};

SerialUnitAlias SerialUnitFactory::readAlias(const String &line)
{
  if (line.startsWith(DiagSerialUnit::LINE_PREFIX))
  {
    return DIAG;
  }
  else if (line.startsWith(ControlSerialUnit::LINE_PREFIX))
  {
    return CTRL;
  }
  else if (line.startsWith(TriggerSerialUnit::LINE_PREFIX))
  {
    return TRIG;
  }
  else if (line.startsWith(GetPidSerialUnit::LINE_PREFIX))
  {
    return GETPID;
  }
  else if (line.startsWith(GetPidResponseSerialUnit::LINE_PREFIX))
  {
    return GETPIDRSP;
  }
  else if (line.startsWith(SetPidSerialUnit::LINE_PREFIX))
  {
    return SETPID;
  }
  else if (line.startsWith(MotorToggleSerialUnit::LINE_PREFIX))
  {
    return MOTTOG;
  }
  else if (line.startsWith(SetConfigSerialUnit::LINE_PREFIX))
  {
    return SETCONF;
  }
  else if (line.startsWith(GetConfigSerialUnit::LINE_PREFIX))
  {
    return GETCONF;
  }
  else if (line.startsWith(GetConfigResponseSerialUnit::LINE_PREFIX))
  {
    return GETCONFRSP;
  }
  else if (line.startsWith(DG1SerialUnit::LINE_PREFIX))
  {
    return DG1;
  }
  else if (line.startsWith(DG2SerialUnit::LINE_PREFIX))
  {
    return DG2;
  }

  int bracketPos = indexOfCharInFirstN(line, '>', 12);
  if (bracketPos != -1)
  {
    return UNKNOWN_SERIAL_UNIT;
  }

  return NOOP;
}
