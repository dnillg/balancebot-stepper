#ifndef SOUL_HPP_
#define SOUL_HPP_

#include <Arduino.h>
#include <SD.h>
#include <SoftwareSerial.h>
#include <DFMiniMp3.h>
#include <map>

#include "DfConfig.h"
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

class SequenceConfiguration {
  private:
    SequenceType sequence;
    uint16_t volume;
    int16_t length;
    SequenceType nextSequence;
  public:
    SequenceConfiguration(SequenceType sequence, int16_t length, uint16_t volume, SequenceType nextSequence)
      : sequence(sequence), volume(volume), length(length), nextSequence(nextSequence) {}
    SequenceType getSequence() { return sequence; }
    uint16_t getVolume() { return volume; }
    int16_t getLength() { return length; }
    SequenceType getNextSequence() { return nextSequence; }
};

extern const std::map<SequenceType, SequenceConfiguration*> SequenceConfigurationMap;

class Soul
{
public:
  Soul(
      DfMp3 *df,
      RobotDisplay *display
    );
  void run();
  void setSequence(SequenceType sequence);

private:
  DfMp3* df;
  RobotDisplay* display;
  SequenceType currentSequence;
  unsigned long lastRunAtMillis = 0;
  unsigned long leftOverSequenceTime = 999999;
};

#endif // SOUL_HPP_