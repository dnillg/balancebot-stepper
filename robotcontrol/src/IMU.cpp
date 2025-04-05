#include "IMU.h"
#include "config.h"

boolean IMU::init() {
    bno080 = new BNO080();
    if (!bno080->begin(BNO085_I2C_ADDR)) {
        return false;
    }
    delay(50);
    bno080->enableRotationVector(BNO085_ROTATION_VECTOR_PERIOD);
    return true;
}

void IMU::getData() {
    if (bno080->dataAvailable()) {
        yaw = bno080->getYaw();
        pitch = bno080->getPitch();
        roll = bno080->getRoll();
    }
}

float IMU::getRoll() {
    return roll;
}

float IMU::getPitch() {
    return pitch;
}

float IMU::getYaw() {
    return yaw;
}