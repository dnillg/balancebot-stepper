#include "Soul.hpp"

const std::map<SequenceType, SequenceConfiguration*> SequenceConfigurationMap = {
    {SLAV_CAT, new SequenceConfiguration(SLAV_CAT, 230, 0, ROBOT_FACE_STANDARD)},
    {CHARLIE, new SequenceConfiguration(CHARLIE, 225, 0, ROBOT_FACE_STANDARD)},
    {CAT_KISS, new SequenceConfiguration(CAT_KISS, 8, 0, ROBOT_FACE_STANDARD)},
    {PEDRO_1X, new SequenceConfiguration(PEDRO_1X, 26, 0, ROBOT_FACE_STANDARD)},
    {PEDRO_2X, new SequenceConfiguration(PEDRO_2X, 26, 0, ROBOT_FACE_STANDARD)},
    {PEDRO_4X, new SequenceConfiguration(PEDRO_4X, 26, 0, ROBOT_FACE_STANDARD)},
    {RICK_ROLL, new SequenceConfiguration(RICK_ROLL, 212, 0, ROBOT_FACE_STANDARD)},
    {ROBOT_FACE_STANDARD, new SequenceConfiguration(ROBOT_FACE_STANDARD, 20, 0, ROBOT_FACE_STANDARD)},
    {OIIUU_CAT, new SequenceConfiguration(OIIUU_CAT, 105, 0, ROBOT_FACE_STANDARD)},
    {ZLAD, new SequenceConfiguration(ZLAD, 180, 0, ROBOT_FACE_STANDARD)},
    {COCONUT, new SequenceConfiguration(COCONUT, 20, 212, ROBOT_FACE_STANDARD)}
};

Soul::Soul(DfMp3 *df, RobotDisplay *display)
    : df(df), display(display), currentSequence(SequenceType::ROBOT_FACE_STANDARD)
{
}

void Soul::run()
{
    unsigned long currentTime = millis();
    if (lastRunAtMillis == 0) {
        lastRunAtMillis = currentTime;
        return;
    }
    unsigned long elapsedTime = currentTime - lastRunAtMillis;
    if (elapsedTime < 0)
    {
        // Handle overflow
        elapsedTime = currentTime;
    }
    lastRunAtMillis = currentTime;

    if (leftOverSequenceTime > 0)
    {
        if (elapsedTime >= leftOverSequenceTime)
        {
            leftOverSequenceTime = 0;
        }
        else
        {
            leftOverSequenceTime -= elapsedTime;
        }
    }

    if (leftOverSequenceTime == 0)
    {
        leftOverSequenceTime = 10000;
        Serial.println("Leftover sequence time is 0, setting next sequence.");
        auto result = SequenceConfigurationMap.find(currentSequence);
        if (result == SequenceConfigurationMap.end())
        {
            Serial.println("ERROR: Sequence not found in map.");
            return;
        }
        SequenceConfiguration *config = result->second;
        Serial.println("Setting next sequence: " + String(config->getNextSequence()));
        this->setSequence(config->getNextSequence());
    }
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

    auto result = SequenceConfigurationMap.find(currentSequence);
    if (result == SequenceConfigurationMap.end())
    {
        Serial.println("ERROR: Sequence not found in map.");
        return;
    }
    leftOverSequenceTime = result->second->getLength() * 1000ul; 
}