#include <Arduino.h>
#include <unity.h>

#include "SerialUnits/SerialUnits.hpp"
#include "SerialUnits/SerialUnits.cpp"

void test_DiagSerialUnit()
{
    DiagSerialUnit diag(4, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0);
    String line = diag.toString();
    TEST_ASSERT_EQUAL_STRING("DIAG>4,1.000,2.000,3.000,4.000,5.000,6.000,7.000,8.000", line.c_str());
    TEST_ASSERT_EQUAL(DIAG, diag.getAlias());
    DiagSerialUnit* diag2 = (DiagSerialUnit*)(DiagSerialUnit::fromLine(line));
    TEST_ASSERT_EQUAL(DIAG, diag2->getAlias());
    TEST_ASSERT_EQUAL(1.0f, diag2->getCurrentRoll());
    TEST_ASSERT_EQUAL(2.0f, diag2->getTargetRoll());
    TEST_ASSERT_EQUAL(3.0f, diag2->getCurrentSpeed());
    TEST_ASSERT_EQUAL(4.0f, diag2->getTargetSpeed());
    TEST_ASSERT_EQUAL(5.0f, diag2->getMotorLeftOutput());
    TEST_ASSERT_EQUAL(6.0f, diag2->getMotorRightOutput());
    TEST_ASSERT_EQUAL(7.0f, diag2->getSteerInput());
    TEST_ASSERT_EQUAL(8.0f, diag2->getSpeedInput());
    delete diag2;
}

void test_ControlSerialUnit()
{
    ControlSerialUnit control(1.0, 2.0);
    String line = control.toString();
    TEST_ASSERT_EQUAL_STRING(line.c_str(), "CTRL>1.00,2.00");
    TEST_ASSERT_EQUAL(CTRL, control.getAlias());
    ControlSerialUnit* control2 = (ControlSerialUnit*)(ControlSerialUnit::fromLine(line));
    TEST_ASSERT_EQUAL(CTRL, control2->getAlias());
    TEST_ASSERT_EQUAL(1.0f, control2->getSpeed());
    TEST_ASSERT_EQUAL(2.0f, control2->getSteer());
    delete control2;
}

void test_TriggerSerialUnit()
{
    TriggerSerialUnit trigger("type", "userData");
    String line = trigger.toString();
    TEST_ASSERT_EQUAL_STRING("TRIG>type,userData", line.c_str());
    TEST_ASSERT_EQUAL(TRIG, trigger.getAlias());
    TriggerSerialUnit* trigger2 = (TriggerSerialUnit*)(TriggerSerialUnit::fromLine(line));
    TEST_ASSERT_EQUAL_STRING(line.c_str(), "TRIG>type,userData");
    TEST_ASSERT_EQUAL(TRIG, trigger2->getAlias());
    TEST_ASSERT_EQUAL_STRING("userData", trigger2->getUserData().c_str());
    TEST_ASSERT_EQUAL_STRING("type", trigger2->getType().c_str());
    delete trigger2;
}

void test_TriggerSerialUnit2()
{
    TriggerSerialUnit trigger("type", "");
    String line = trigger.toString();
    TEST_ASSERT_EQUAL_STRING("TRIG>type,", line.c_str());
    TEST_ASSERT_EQUAL(TRIG, trigger.getAlias());
    TriggerSerialUnit* trigger2 = (TriggerSerialUnit*)(TriggerSerialUnit::fromLine(line));
    TEST_ASSERT_EQUAL_STRING(line.c_str(), "TRIG>type,");
    TEST_ASSERT_EQUAL(TRIG, trigger2->getAlias());
    TEST_ASSERT_EQUAL_STRING("", trigger2->getUserData().c_str());
    TEST_ASSERT_EQUAL_STRING("type", trigger2->getType().c_str());
    delete trigger2;
}

void test_GetPidSerialUnit()
{
    GetPidSerialUnit getPid("type");
    String line = getPid.toString();
    TEST_ASSERT_EQUAL_STRING("GETPID>type", line.c_str());
    TEST_ASSERT_EQUAL(GETPID, getPid.getAlias());
    GetPidSerialUnit* getPid2 = (GetPidSerialUnit*)(GetPidSerialUnit::fromLine(line));
    TEST_ASSERT_EQUAL(GETPID, getPid2->getAlias());
    TEST_ASSERT_EQUAL_STRING("type", getPid2->getType().c_str());
    delete getPid2;
}

void test_SetPidSerialUnit()
{
    SetPidSerialUnit setPid("type", 1.0, 2.0, 3.0);
    String line = setPid.toString();
    TEST_ASSERT_EQUAL_STRING("SETPID>type,1.00,2.00,3.00", line.c_str());
    TEST_ASSERT_EQUAL(SETPID, setPid.getAlias());
    SetPidSerialUnit* setPid2 = (SetPidSerialUnit*)(SetPidSerialUnit::fromLine(line));
    TEST_ASSERT_EQUAL(SETPID, setPid2->getAlias());
    TEST_ASSERT_EQUAL_STRING("type", setPid2->getType().c_str());
    TEST_ASSERT_EQUAL(1.0f, setPid2->getP());
    TEST_ASSERT_EQUAL(2.0f, setPid2->getI());
    TEST_ASSERT_EQUAL(3.0f, setPid2->getD());
    delete setPid2;
}

void test_GetPidResponseSerialUnit()
{
    GetPidResponseSerialUnit getPidResponse("type", 1.0, 2.0, 3.0);
    String line = getPidResponse.toString();
    TEST_ASSERT_EQUAL_STRING("GETPIDRSP>type,1.00,2.00,3.00", line.c_str());
    TEST_ASSERT_EQUAL(GETPIDRSP, getPidResponse.getAlias());
    GetPidResponseSerialUnit* getPidResponse2 = (GetPidResponseSerialUnit*)(GetPidResponseSerialUnit::fromLine(line));
    TEST_ASSERT_EQUAL(GETPIDRSP, getPidResponse2->getAlias());
    TEST_ASSERT_EQUAL_STRING("type", getPidResponse2->getType().c_str());
    TEST_ASSERT_EQUAL(1.0f, getPidResponse2->getP());
    TEST_ASSERT_EQUAL(2.0f, getPidResponse2->getI());
    TEST_ASSERT_EQUAL(3.0f, getPidResponse2->getD());
    delete getPidResponse2;
}

void test_MotorToggleSerialUnit()
{
    MotorToggleSerialUnit motorToggle(1);
    String line = motorToggle.toString();
    TEST_ASSERT_EQUAL_STRING("MOTTOG>1", line.c_str());
    TEST_ASSERT_EQUAL(MOTTOG, motorToggle.getAlias());
    MotorToggleSerialUnit* motorToggle2 = (MotorToggleSerialUnit*)(MotorToggleSerialUnit::fromLine(line));
    TEST_ASSERT_EQUAL(MOTTOG, motorToggle2->getAlias());
    TEST_ASSERT_EQUAL(1, motorToggle2->getState());
    delete motorToggle2;
}

// Entry point for tests
void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_DiagSerialUnit);
    RUN_TEST(test_ControlSerialUnit);
    RUN_TEST(test_TriggerSerialUnit);
    RUN_TEST(test_TriggerSerialUnit2);
    RUN_TEST(test_GetPidSerialUnit);
    RUN_TEST(test_SetPidSerialUnit);
    RUN_TEST(test_GetPidResponseSerialUnit);
    RUN_TEST(test_MotorToggleSerialUnit);

    UNITY_END();
}

void loop() {
    // Leave empty, not needed
}