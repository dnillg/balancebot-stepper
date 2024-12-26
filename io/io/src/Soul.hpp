#ifndef SOUL_HPP_
#define SOUL_HPP_

#include <Arduino.h>
#include <SD.h>
#include <SoftwareSerial.h>
#include "RobotDisplay.hpp"

enum SequenceType
{
  SEQUENCE_TYPE_IDLE,
  SEQUENCE_TYPE_HAPPY,
  SEQUENCE_TYPE_SAD,
  SEQUENCE_TYPE_ANGRY,
  SEQUENCE_TYPE_SCARED,
  SEQUENCE_TYPE_BEER,
};

class Soul
{
public:
  Soul(
      EspSoftwareSerial::UART *mp3Serial,
      RobotDisplay *display,
      SDFS* sd);
  void run();
  void setSequence(SequenceType sequence);

private:
  EspSoftwareSerial::UART *mp3Serial;
  RobotDisplay *display;
  SDFS* sd;
  SequenceType sequence;
  uint32_t elapsedNanos();
  void getFrame(uint16_t frameNumber, uint8_t *frameBuffer);
};

#endif // SOUL_HPP_