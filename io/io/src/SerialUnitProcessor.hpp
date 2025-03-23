#ifndef SERIALUNITPROCESSOR_HPP_
#define SERIALUNITPROCESSOR_HPP_

#include <Arduino.h>
#include <map>
#include <vector>

#include "SerialUnits/SerialUnits.hpp"

class SerialUnitListener
{
private:
  SerialUnitAlias alias;

public:
  SerialUnitListener(SerialUnitAlias alias) : alias(alias) {}
  const SerialUnitAlias getAlias() { return alias; }
  virtual void consume(ISerialUnit* unit) = 0;
};

class SerialUnitProcessor
{
private:
  std::map<SerialUnitAlias, std::vector<SerialUnitListener*>> serialUnitListeners = {};

public:
  ~SerialUnitProcessor()
  {
    for (auto const &pair : serialUnitListeners)
    {
      for (SerialUnitListener* listener : pair.second)
      {
        delete listener;
      }
    }
  }

  void addListener(SerialUnitListener* listener)
  {
    serialUnitListeners[listener->getAlias()].push_back(listener);
  }

  void process(const SerialUnitAlias alias, const String& line)
  {
    if (serialUnitListeners.find(alias) == serialUnitListeners.end())
    {
      return;
    }
    ISerialUnit* unit = 0;
    for (SerialUnitListener* listener : serialUnitListeners[alias])
    {
      if (unit == 0)
      {
        unit = SerialUnitFactory::fromLine(line);
      }
      listener->consume(unit);
    }
    if (unit != 0) {
      delete unit;
    }
  }
};

#endif // SERIALUNITPROCESSOR_HPP_