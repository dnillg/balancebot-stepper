#include "SerialUnitProcessor.hpp"

void SerialUnitProcessor::process(const SerialUnitAlias alias, const String &line)
{
  if (serialUnitListeners.find(alias) == serialUnitListeners.end())
  {
    return;
  }
  if (backlog.size() >= 10)
  {
    Serial.println("Backlog full, skipping...");
  }
  else
  {
  ISerialUnit *unit = SerialUnitFactory::fromLine(line);
  backlog.push_back(unit);
  }
}

void SerialUnitProcessor::run()
{
  while (backlog.size() > 0)
  {
    ISerialUnit *unit = backlog.front();
    backlog.pop_front();
    if (serialUnitListeners.find(unit->getAlias()) != serialUnitListeners.end())
    {
      for (SerialUnitListener *listener : serialUnitListeners[unit->getAlias()])
      {
        listener->consume(unit);
      }
    }
    delete unit;
  }
}

SerialUnitProcessor::~SerialUnitProcessor()
  {
    for (auto const &pair : serialUnitListeners)
    {
      for (SerialUnitListener* listener : pair.second)
      {
        delete listener;
      }
    }
  }