#include "SpeedAggregator.hpp"

SpeedAggregator::SpeedAggregator(uint8_t samplesCount, uint8_t batchSize) {
    this->samplesCount = samplesCount;
    this->batchSize = batchSize;
    this->samples = new uint16_t[samplesCount];
    for (uint8_t i = 0; i < samplesCount; i++) {
        samples[i] = 0;
    }
}

SpeedAggregator::~SpeedAggregator() {
    delete[] samples;
}

void SpeedAggregator::consume(uint16_t sample) {
    if (batchSize == 1) {
        addSample(sample);
    } else {
        batchSum += sample;
        batchCounter++;
        if (batchCounter == batchSize) {
            batchCounter = 0;
            int32_t sample = batchSum / batchSize;
            batchSum = 0;
            addSample(sample);
        }
    }
}

void SpeedAggregator::addSample(uint16_t sample) {
    samplesSum -= samples[head];
    samples[head] = sample;
    samplesSum += samples[head];
    head++;
    if (head == samplesCount) {
        head = 0;
    }
}

uint16_t SpeedAggregator::getSpeed() {
    return samplesSum / samplesCount;
}

