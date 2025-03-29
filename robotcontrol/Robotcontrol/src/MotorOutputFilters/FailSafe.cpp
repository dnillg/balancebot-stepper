#include "FailSafe.hpp"

FailSafeMotorOutputFilter::FailSafeMotorOutputFilter(uint16_t ms) {
    this->thresholdMs = ms;
    this->lastHeartBeat = millis();
}

bool FailSafeMotorOutputFilter::isAlive() {
    return (millis() - this->lastHeartBeat) <= this->thresholdMs;
}

void FailSafeMotorOutputFilter::heartBeat() {
    this->lastHeartBeat = millis();
}

double FailSafeMotorOutputFilter::filter(double input, MotorPosition motorPos) {
    if (isAlive()) {
        return input;
    } else {
        return 0.0; // or some other default value
    }
}