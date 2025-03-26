#ifndef SOUL_HPP_
#define SOUL_HPP_

#include <Arduino.h>
#include <SD.h>
#include <SoftwareSerial.h>
#include <DFMiniMp3.h>

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
};

#endif // SOUL_HPP_