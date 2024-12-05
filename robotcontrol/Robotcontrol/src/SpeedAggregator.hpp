#ifndef SPEEDAGGREGATOR_HPP_
#define SPEEDAGGREGATOR_HPP_

#include <Arduino.h>

class SpeedAggregator {
    private:
        uint8_t samplesCount = 0;
        uint8_t head = 0;
        uint16_t* samples;
        uint32_t samplesSum = 0;

        uint8_t batchSize = 1;
        uint8_t batchCounter = 0;
        int32_t batchSum = 0;
    public:
        SpeedAggregator(uint8_t samplesCount, uint8_t batchSize);
        ~SpeedAggregator();
        void consume(uint16_t sample);
        void addSample(uint16_t sample);
        uint16_t getSpeed();
};

#endif // SPEEDAGGREGATOR_HPP_