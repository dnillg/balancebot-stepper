#ifndef DIAGSENDER_HPP_
#define DIAGSENDER_HPP_

#include <Arduino.h>

#include "DiagDTO.hpp"
#include "SerialUnits/SerialUnits.hpp"

enum DiagMode {
  DIAG_MODE_OFF,
  DIAG_MODE_ROLL,
  DIAG_MODE_SPEED
};

class DiagSender
{
  private:
    DiagMode mode = DIAG_MODE_OFF;
    HardwareSerial *serial;
  public:
    DiagSender(HardwareSerial *serial) : serial(serial) {}
    void send(DiagDTO dto);
    void setMode(DiagMode mode) {
      this->mode = mode;
    }  
};

#endif