#ifndef SERIALUNITPROCESSOR_HPP_
#define SERIALUNITPROCESSOR_HPP_

#include <Arduino.h>
#include <map>
#include <vector>
#include <list>

#include "SerialUnits/SerialUnits.hpp"
#include <freertos/semphr.h>

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
  std::list<ISerialUnit*> backlog;
  QueueHandle_t mutex = xSemaphoreCreateMutex();
  ISerialUnit* popBacklog();
  void pushBacklog(ISerialUnit* unit);
public:
  ~SerialUnitProcessor();
  void addListener(SerialUnitListener* listener)
  {
    serialUnitListeners[listener->getAlias()].push_back(listener);
  }
  void process(const SerialUnitAlias alias, const String& line);
  void run();

};

#endif // SERIALUNITPROCESSOR_HPP_