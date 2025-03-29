#include "SteeringMotorOutputFilter.hpp"

double SteeringMotorOutputFilter::filter(double input, MotorPosition motorPos) {
    if (motorPos == LEFT) {
        return input + steeringOffset * (1.0 - steeringIntensity);
    } else if (motorPos == RIGHT) {
        return input - steeringOffset * (1.0 - steeringIntensity);
    } else {
        return input; // No change for other positions
    }
}

void SteeringMotorOutputFilter::setSteeringIntensity(double intensity) {
    steeringIntensity = intensity;
}