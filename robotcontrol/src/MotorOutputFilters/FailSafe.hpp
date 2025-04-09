#ifndef FAILSAFE_H_
#define FAILSAFE_H_

#include <Arduino.h>
#include "Motor/MotorPosition.hpp"
#include "MotorOutputFilter.hpp"
#include "config.h"

class FailSafeMotorOutputFilter : public MotorOutputFilter {
    public:
        FailSafeMotorOutputFilter(uint16_t ms = 1000);
        void heartBeat();
        void setThresholdMs(uint16_t ms);
        void setRoll(double roll) { this->currentRoll = roll; }
        double filter(double input, MotorPosition motorPos) override;
    private:
        uint16_t thresholdMs;
        suseconds_t lastHeartBeat;
        bool lastStatus = 1;
        double currentRoll;
        bool isRollValid(); 
        bool hadRecentHeartBeat();
};

#endif // FAILSAFE_H_