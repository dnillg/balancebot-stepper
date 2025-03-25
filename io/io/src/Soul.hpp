#ifndef SOUL_HPP_
#define SOUL_HPP_

#include <Arduino.h>
#include <SD.h>
#include <SoftwareSerial.h>
#include "RobotDisplay.hpp"

enum SequenceType
{
  SLAV_CAT,
  CHARLIE,
  CAT_KISS,
  PEDRO_1X,
  PEDRO_2X,
  PEDRO_4X,
  RICK_ROLL,
  ROBOT_FACE_STANDARD,
  OIIUU_CAT,
  ZLAD,
  COCONUT
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