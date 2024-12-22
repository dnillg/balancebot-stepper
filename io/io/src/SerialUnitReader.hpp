#ifndef SERIALUNITREADER_HPP_
#define SERIALUNITREADER_HPP_

#include <Arduino.h>
#include <vector>

#include "SerialUnits.hpp"

class SerialUnitReader {
    public: 
        SerialUnitReader();
        SerialUnitAlias readAlias(const String& value);
        ISerialUnit readUnit(SerialUnitAlias alias, const String &str);
    private: 
        void split(const String &line, char delimiter, uint32_t startFrom, std::vector<String> parts);
};

#endif // SERIALUNITREADER_HPP_