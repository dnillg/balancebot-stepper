#include "Soul.hpp"

Soul::Soul(
    EspSoftwareSerial::UART *mp3Serial,
    RobotDisplay *display,
    SDFS* sd)
{
    this->mp3Serial = mp3Serial;
    this->display = display;
    this->sequence = SEQUENCE_TYPE_IDLE;
    this->sd = sd;
}

void Soul::run()
{
    // TODO
}

void Soul::setSequence(SequenceType sequence)
{
    this->sequence = sequence;
}

uint32_t Soul::elapsedNanos()
{
    return micros() * 1000;
}

void Soul::getFrame(uint16_t frameNumber, uint8_t *frameBuffer)
{
    // TODO
}

