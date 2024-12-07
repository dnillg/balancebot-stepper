#ifndef COMMANDPARSER_HPP_
#define COMMANDPARSER_HPP_

#include <Arduino.h>

#include "Motor.hpp"
#include "Control.hpp"

#define MAX_COMMAND_PARAMS 10

class CommandParser
{
  public:
    CommandParser(Motor* motorLeft, Motor* motorRight, Control* control);
    bool handleLine(String& line);
  private:
    Motor* motorLeft;
    Motor* motorRight;
    Control* control;

    void split(String& line, char delimiter, String* parts, uint8_t maxParts);
    void truncateCommand(String& line);
};

#endif // COMMANDPARSER_HPP_