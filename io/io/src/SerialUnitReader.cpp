#include "SerialUnitReader.hpp"
#include "math.h"
#include <vector>

std::map<SerialUnitAlias, String> serialUnitAliases = {
  {SerialUnitAlias::DIAG, "DIAG"},
  {SerialUnitAlias::CTRL, "CTRL"},
  {SerialUnitAlias::GETPID, "GETPID"},
  {SerialUnitAlias::GETPIDRSP, "GETPIDRSP"},
  {SerialUnitAlias::SETPID, "SETPID"},
  {SerialUnitAlias::MOTTOG, "MOTTOG"},
  {SerialUnitAlias::TRIG, "TRIG"}
};

SerialUnitReader::SerialUnitReader() {
}

SerialUnitAlias SerialUnitReader::readAlias(const String& str) {
    auto until = min(str.length(), 20u);
    for (int i = 0; i <= until; i++) {
        if (str[i] == '>') {
            if (str.startsWith("DIAG")) {
                return SerialUnitAlias::DIAG;
            } else if (str.startsWith("CTRL")) {
                return SerialUnitAlias::CTRL;
            } else if (str.startsWith("GETPID")) {
                return SerialUnitAlias::GETPID;
            } else if (str.startsWith("SETPID")) {
                return SerialUnitAlias::SETPID;
            } else if (str.startsWith("MOTTOG")) {
                return SerialUnitAlias::MOTTOG;
            } else if (str.startsWith("TRIG")) {
                return SerialUnitAlias::TRIG;
            } else if (str.startsWith("GETPIDRSP")) {
                return SerialUnitAlias::GETPIDRSP;
            }
            return SerialUnitAlias::UNKNOWN_SERIAL_UNIT;
        }
    }
    return SerialUnitAlias::UNKNOWN_SERIAL_UNIT;
}

ISerialUnit SerialUnitReader::readUnit(SerialUnitAlias alias, const String& str) {
    if (alias == SerialUnitAlias::UNKNOWN_SERIAL_UNIT) {
        return NoopSerialUnit();
    }
    auto entry = serialUnitAliases.find(alias);
    if (entry == serialUnitAliases.end()) {
        return NoopSerialUnit();
    }


    std::vector<String> parts(10);
    split(str, ',', entry->second.length(), parts);
    
    if (alias == SerialUnitAlias::TRIG) {
        if (parts.size() < 1) {
            return NoopSerialUnit();
        }
        TriggerSerialUnit result;
        result.type = parts[0];
        return result;
    }
    
    return NoopSerialUnit();
}

void SerialUnitReader::split(const String &line, char delimiter, uint32_t startFrom, std::vector<String> parts)
{
  uint32_t lineIndex = startFrom;
  uint32_t partStart = startFrom;
  while (lineIndex < line.length())
  {
    if (line[lineIndex] == delimiter)
    {
      parts.push_back(line.substring(partStart, lineIndex));
      partStart = lineIndex + 1;
    }
    lineIndex++;
  }
  parts.push_back(line.substring(partStart, lineIndex));
}