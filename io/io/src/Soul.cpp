#include "Soul.hpp"

Soul::Soul(DfMp3 *df, RobotDisplay *display)
    : df(df), display(display), currentSequence(SequenceType::ROBOT_FACE_STANDARD)
{
}

void Soul::run()
{
    // Nothing
}

void Soul::setSequence(SequenceType sequence)
{
    //df->stop();
    this->currentSequence = sequence;
    char fileName[16];  // 4 digits + 1 null terminator
    sprintf(fileName, "/%04d.gif", sequence);
    this->display->playGif(fileName);
    this->df->playMp3FolderTrack(sequence);
    this->df->start();
}