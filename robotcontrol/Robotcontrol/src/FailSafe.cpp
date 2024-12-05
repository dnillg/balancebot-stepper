#include "FailSafe.hpp"

FailSafe::FailSafe(uint16_t ms) {
    this->thresholdMs = ms;
    this->lastHeartBeat = getCurrentMs();
}

bool FailSafe::isAlive() {
    return (getCurrentMs() - this->lastHeartBeat) <= this->thresholdMs;
}

void FailSafe::heartBeat() {
    this->lastHeartBeat = getCurrentMs();
}

uint64_t FailSafe::getCurrentMs() {
    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    return static_cast<uint64_t>(tv_now.tv_sec) * 1000 + tv_now.tv_usec / 1000;
}