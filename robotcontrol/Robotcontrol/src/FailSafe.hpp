#ifndef FAILSAFE_H_
#define FAILSAFE_H_

#include <Arduino.h>

class FailSafe {
    public:
        FailSafe(uint16_t ms = 1000);
        void heartBeat();
        bool isAlive();
        void setThresholdMs(uint16_t ms);
        uint64_t getCurrentMs();
    private:
        uint16_t thresholdMs;
        suseconds_t lastHeartBeat;
};

#endif // FAILSAFE_H_