#ifndef FAILSAFE_H_
#define FAILSAFE_H_

#include <Arduino.h>
#include "MotorPosition.hpp"
#include "MotorOutputFilter.hpp"

class FailSafeMotorOutputFilter : public MotorOutputFilter {
    public:
        FailSafeMotorOutputFilter(uint16_t ms = 1000);
        void heartBeat();
        bool isAlive();
        void setThresholdMs(uint16_t ms);
        double filter(double input, MotorPosition motorPos) override;
    private:
        uint16_t thresholdMs;
        suseconds_t lastHeartBeat;
};

#endif // FAILSAFE_H_