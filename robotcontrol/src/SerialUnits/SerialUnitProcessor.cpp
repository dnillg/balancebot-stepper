#include "SerialUnitProcessor.hpp"

void SerialUnitProcessor::process(const SerialUnitAlias alias, const String &line)
{
  if (serialUnitListeners.find(alias) == serialUnitListeners.end())
  {
    Serial.println("No listeners for alias: " + alias);
    return;
  }
  Serial.println("Processing line: " + line);
  ISerialUnit *unit = SerialUnitFactory::fromLine(line);
  if (xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE) {
    if (backlog.size() >= 10)
    {
      Serial.println("Backlog full, skipping...");
      delete unit;
    }
    else
    {
      backlog.push_back(unit); //TODO: mutex
    }
    xSemaphoreGive(mutex);
  }
}

ISerialUnit* SerialUnitProcessor::popBacklog()
{
  ISerialUnit *unit = nullptr;
  if (xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE) {
    if (backlog.size() > 0)
    {
      unit = backlog.front();
      backlog.pop_front();
    }
    xSemaphoreGive(mutex);
  }
  return unit;
}

void SerialUnitProcessor::pushBacklog(ISerialUnit* unit) {
  if (xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE) {
    backlog.push_back(unit);
    xSemaphoreGive(mutex);
  }
}

void SerialUnitProcessor::run()
{
  ISerialUnit* item = popBacklog();
  while (item != nullptr)
  {
    if (serialUnitListeners.find(item->getAlias()) != serialUnitListeners.end())
    {
      for (SerialUnitListener *listener : serialUnitListeners[item->getAlias()])
      {
        listener->consume(item);
      }
    }
    delete item;
    item = popBacklog();
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