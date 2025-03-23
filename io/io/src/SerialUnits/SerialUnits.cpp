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
    {NOOP, "NOOP"}};

// Unknown Serial Unit
UnknownSerialUnit::UnknownSerialUnit(String value) : value(value) {}
SerialUnitAlias UnknownSerialUnit::getAlias() { return UNKNOWN_SERIAL_UNIT; }
String UnknownSerialUnit::toString() { return "UNKNOWN>" + value; }

// Parse Error Serial Unit
ParseErrorSerialUnit::ParseErrorSerialUnit(String line) : line(line) {}
SerialUnitAlias ParseErrorSerialUnit::getAlias() { return PARSE_ERROR; }
String ParseErrorSerialUnit::toString() { return "PARSE_ERROR>" + line; }

// Diag Serial Unit
DiagSerialUnit::DiagSerialUnit(float cr, float tr, float cs, float ts, float ml, float mr, float si, float sp)
    : currentRoll(cr), targetRoll(tr), currentSpeed(cs), targetSpeed(ts),
      motorLeftOutput(ml), motorRightOutput(mr), steerInput(si), speedInput(sp) {}

SerialUnitAlias DiagSerialUnit::getAlias() { return DIAG; }
String DiagSerialUnit::toString()
{
    return "DIAG>" + String(currentRoll, 3) + "," + String(targetRoll, 3) + "," +
           String(currentSpeed, 3) + "," + String(targetSpeed, 3) + "," +
           String(motorLeftOutput, 3) + "," + String(motorRightOutput, 3) + "," +
           String(steerInput, 3) + "," + String(speedInput, 3);
}

ISerialUnit* DiagSerialUnit::fromLine(String line) 
{
    if (!line.startsWith("DIAG>")) { 
        return new ParseErrorSerialUnit(line);  // Invalid format
    }

    float values[8] = {0};
    int parsed = sscanf(line.c_str() + 5, "%f,%f,%f,%f,%f,%f,%f,%f", 
                        &values[0], &values[1], &values[2], &values[3], 
                        &values[4], &values[5], &values[6], &values[7]);

    if (parsed == 8) {
        return new DiagSerialUnit(values[0], values[1], values[2], values[3], 
                                  values[4], values[5], values[6], values[7]);
    }

    return new ParseErrorSerialUnit(line);  // Parsing failed
}

// Control Serial Unit
ControlSerialUnit::ControlSerialUnit(float spd, float str) : speed(spd), steer(str) {}
SerialUnitAlias ControlSerialUnit::getAlias() { return CTRL; }
String ControlSerialUnit::toString() { return "CTRL>" + String(speed) + "," + String(steer); }

ISerialUnit* ControlSerialUnit::fromLine(String line)
{
    float spd, str;
    if (sscanf(line.c_str(), "CTRL>%f,%f", &spd, &str) == 2)
    {
        return new ControlSerialUnit(spd, str);
    }
    return new ParseErrorSerialUnit(line);
}

// Trigger Serial Unit
TriggerSerialUnit::TriggerSerialUnit(String t, String ud) : type(t), userData(ud) {}
SerialUnitAlias TriggerSerialUnit::getAlias() { return TRIG; }
String TriggerSerialUnit::toString() { return "TRIG>" + type + "," + userData; }
const String& TriggerSerialUnit::getType() { return type; }

ISerialUnit* TriggerSerialUnit::fromLine(String line)
{
    if (line.length() < 6 || !line.startsWith("TRIG>")) { 
        return new ParseErrorSerialUnit(line);  // Invalid format
    }

    int commaIndex = line.indexOf(',', 5);  // Look for ',' after "TRIG>"
    
    if (commaIndex == -1) {
        return new TriggerSerialUnit(line.substring(5), "");  // No userData
    }

    return new TriggerSerialUnit(line.substring(5, commaIndex), line.substring(commaIndex + 1));
}

// GetPid Serial Unit
GetPidSerialUnit::GetPidSerialUnit(String t) : type(t) {}
SerialUnitAlias GetPidSerialUnit::getAlias() { return GETPID; }
String GetPidSerialUnit::toString() { return "GETPID>" + type; }

ISerialUnit* GetPidSerialUnit::fromLine(String line)
{
    char t[16];
    if (sscanf(line.c_str(), "GETPID>%15s", t) == 1)
    {
        return new GetPidSerialUnit(String(t));
    }
    return new ParseErrorSerialUnit(line);
}

// Toggle Motor Serial Unit
MotorToggleSerialUnit::MotorToggleSerialUnit(int state) : state(state) {}
SerialUnitAlias MotorToggleSerialUnit::getAlias() { return MOTTOG; }
String MotorToggleSerialUnit::toString() { return "MOTTOG>" + String(state); }

ISerialUnit* MotorToggleSerialUnit::fromLine(String line)
{
    int state;
    if (sscanf(line.c_str(), "MOTTOG>%d", &state) == 1)
    {
        return new MotorToggleSerialUnit(state);
    }
    return new ParseErrorSerialUnit(line);
}

// GetPidResponse Serial Unit
GetPidResponseSerialUnit::GetPidResponseSerialUnit(String t, float p, float i, float d) : type(t), p(p), i(i), d(d) {}
SerialUnitAlias GetPidResponseSerialUnit::getAlias() { return GETPIDRSP; }
String GetPidResponseSerialUnit::toString() { return "GETPIDRSP>" + type + "," + String(p) + "," + String(i) + "," + String(d); }

ISerialUnit* GetPidResponseSerialUnit::fromLine(String line)
{
    char t[16];
    float p, i, d;
    if (sscanf(line.c_str(), "GETPIDRSP>%15[^,],%f,%f,%f", t, &p, &i, &d) == 4)
    {
        return new GetPidResponseSerialUnit(String(t), p, i, d);
    }
    return new ParseErrorSerialUnit(line);
}

// SetPid Serial Unit
SetPidSerialUnit::SetPidSerialUnit(String t, float p, float i, float d) : type(t), p(p), i(i), d(d) {}
SerialUnitAlias SetPidSerialUnit::getAlias() { return SETPID; }
String SetPidSerialUnit::toString() { return "SETPID>" + type + "," + String(p) + "," + String(i) + "," + String(d); }

ISerialUnit* SetPidSerialUnit::fromLine(String line)
{
    char t[16];
    float p, i, d;
    if (sscanf(line.c_str(), "SETPID>%15[^,],%f,%f,%f", t, &p, &i, &d) == 4)
    {
        return new SetPidSerialUnit(String(t), p, i, d);
    }
    return new ParseErrorSerialUnit(line);
}

ISerialUnit* SerialUnitFactory::fromLine(const String& line)
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
    
    return new NoopSerialUnit();
};

SerialUnitAlias SerialUnitFactory::readAlias(const String& line)
{
    if (line.startsWith("DIAG>"))
    {
        return DIAG;
    }
    else if (line.startsWith("CTRL>"))
    {
        return CTRL;
    }
    else if (line.startsWith("TRIG>"))
    {
        return TRIG;
    }
    else if (line.startsWith("GETPID>"))
    {
        return GETPID;
    }
    else if (line.startsWith("GETPIDRSP>"))
    {
        return GETPIDRSP;
    }
    else if (line.startsWith("SETPID>"))
    {
        return SETPID;
    }
    else if (line.startsWith("MOTTOG>"))
    {
        return MOTTOG;
    }

    int bracketPos = indexOfCharInFirstN(line, '>', 12);
    if (bracketPos != -1) 
    {
        return UNKNOWN_SERIAL_UNIT;
    }

    return NOOP;
}
