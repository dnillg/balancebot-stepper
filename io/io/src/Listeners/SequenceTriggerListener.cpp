#include "SequenceTriggerListener.hpp"

SequenceTriggerListener::SequenceTriggerListener(Soul *soul)
    : soul(soul), SerialUnitListener(SerialUnitAlias::TRIG)
{
}

void SequenceTriggerListener::consume(ISerialUnit *unit)
{
    TriggerSerialUnit triggerUnit = *((TriggerSerialUnit *)unit);
    Serial.println("SequenceTriggerListener::consume");
    Serial.println(triggerUnit.getType());

    if (triggerUnit.getType() == "SLAV_CAT")
    {
        soul->setSequence(SequenceType::SLAV_CAT);
    }
    if (triggerUnit.getType() == "CHARLIE")
    {
        soul->setSequence(SequenceType::CHARLIE);
    }
    if (triggerUnit.getType() == "CAT_KISS")
    {
        soul->setSequence(SequenceType::CAT_KISS);
    }
    if (triggerUnit.getType() == "PEDRO_1X")
    {
        soul->setSequence(SequenceType::PEDRO_1X);
    }
    if (triggerUnit.getType() == "PEDRO_2X")
    {
        soul->setSequence(SequenceType::PEDRO_2X);
    }
    if (triggerUnit.getType() == "PEDRO_4X")
    {
        soul->setSequence(SequenceType::PEDRO_4X);
    }
    if (triggerUnit.getType() == "RICK_ROLL")
    {
        soul->setSequence(SequenceType::RICK_ROLL);
    }
    if (triggerUnit.getType() == "ROBOT_FACE_STANDARD")
    {
        soul->setSequence(SequenceType::ROBOT_FACE_STANDARD);
    }
    if (triggerUnit.getType() == "OIIUU_CAT")
    {
        soul->setSequence(SequenceType::OIIUU_CAT);
    }
    if (triggerUnit.getType() == "ZLAD")
    {
        soul->setSequence(SequenceType::ZLAD);
    }
    if (triggerUnit.getType() == "COCONUT")
    {
        soul->setSequence(SequenceType::COCONUT);
    }
    if (triggerUnit.getType() == "NYAN")
    {
        soul->setSequence(SequenceType::NYAN);
    }
    if (triggerUnit.getType() == "TROLOLO")
    {
        soul->setSequence(SequenceType::TROLOLO);
    }
    if (triggerUnit.getType() == "MINECRAFT")
    {
        soul->setSequence(SequenceType::MINECRAFT);
    }
    if (triggerUnit.getType() == "CSIPKES")
    {
        soul->setSequence(SequenceType::CSIPKES);
    }
    if (triggerUnit.getType() == "TARKOV")
    {
        soul->setSequence(SequenceType::TARKOV);
    }

}