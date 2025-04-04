#include "DiagSender.hpp"

void DiagSender::send(DiagDTO dto) {
  if (mode == DIAG_MODE_OFF) {
    return;
  }
  if (mode == DIAG_MODE_ROLL) {
    DG1SerialUnit serialUnit = DG1SerialUnit(dto.cycleNo, dto.roll, dto.rollSetpoint);
    this->serial->println(serialUnit.toString());
  } else if (mode == DIAG_MODE_SPEED) {
    DG1SerialUnit serialUnit = DG1SerialUnit(dto.cycleNo, dto.roll, dto.rollSetpoint);
    this->serial->println(serialUnit.toString());
  }
}