#include "CommandParser.hpp"

CommandParser::CommandParser(Motor *motorLeft, Motor *motorRight, Control *control)
{
  this->motorLeft = motorLeft;
  this->motorRight = motorRight;
  this->control = control;
}

bool CommandParser::handleLine(String &line)
{
  String parts[MAX_COMMAND_PARAMS];
  if (line.startsWith("c>")) {
    split(line, '>', parts, MAX_COMMAND_PARAMS);
    this->control->setRemoteControlInput(parts[0].toInt() / 128.0, parts[1].toInt() / 128.0);
    return true;
  }
  if (line.equals("moff"))
  {
    motorLeft->disable();
    motorRight->disable();
    return true;
  }
  if (line.equals("moon"))
  {
    motorLeft->enable();
    motorRight->enable();
    return true;
  }
  if (line.startsWith("rpid>"))
  {
    truncateCommand(line);
    split(line, '>', parts, MAX_COMMAND_PARAMS);
    this->control->setRollParams(parts[0].toDouble(), parts[1].toDouble(), parts[2].toDouble());
    return true;
  }
  if (line.startsWith("spid>"))
  {
    truncateCommand(line);
    split(line, '>', parts, MAX_COMMAND_PARAMS);
    this->control->setSpeedParams(parts[0].toDouble(), parts[1].toDouble(), parts[2].toDouble());
    return true;
  }
  if (line.startsWith("rollsp>")) {
    truncateCommand(line);
    split(line, '>', parts, MAX_COMMAND_PARAMS);
    this->control->setRollSetpoint(parts[0].toDouble());
    return true;
  }
  if (line.startsWith("stltmax>")) {
    truncateCommand(line);
    split(line, '>', parts, MAX_COMMAND_PARAMS);
    this->control->setMaxSpeedPidOutputRad(parts[0].toDouble());
    return true;
  }
  
  
  return false;
}

void CommandParser::split(String &line, char delimiter, String *parts, uint8_t maxParts)
{
  uint8_t partIndex = 0;
  uint8_t lineIndex = 0;
  uint8_t partStart = 0;
  while (lineIndex < line.length() && partIndex < maxParts)
  {
    if (line[lineIndex] == delimiter)
    {
      parts[partIndex] = line.substring(partStart, lineIndex);
      partStart = lineIndex + 1;
      partIndex++;
    }
    lineIndex++;
  }
  if (partIndex < maxParts)
  {
    parts[partIndex] = line.substring(partStart, lineIndex);
  }
}

void CommandParser::truncateCommand(String &line)
{
  line.remove(0, line.indexOf('>') + 1);
}