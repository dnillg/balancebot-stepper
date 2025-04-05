#ifndef SPEEDAGGREGATOR_HPP_
#define SPEEDAGGREGATOR_HPP_

#include <Arduino.h>

class SpeedAggregator {
    private:
        uint8_t samplesCount;
        uint8_t head;
        int16_t* samples;
        int32_t samplesSum;

        uint8_t batchSize;
        uint8_t batchCounter;
        int32_t batchSum;
    public:
        SpeedAggregator(uint8_t samplesCount, uint8_t batchSize);
        ~SpeedAggregator();
        void consume(int16_t sample);
        void addSample(int16_t sample);
        int16_t getSpeed();
};

#endif // SPEEDAGGREGATOR_HPP_