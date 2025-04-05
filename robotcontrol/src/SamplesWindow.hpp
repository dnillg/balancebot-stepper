#ifndef SAMPLESWINDOW_HPP_
#define SAMPLESWINDOW_HPP_

#include <Arduino.h>

class SamplesWindow
{
  private:
    uint16_t windowSize;
    double *window;
    uint16_t windowIndex;
    double sum;
    double range;
    double min;
    double max;
  public:
    SamplesWindow(uint16_t windowSize) {
        this->windowSize = windowSize;
        this->window = new double[windowSize];
        this->windowIndex = 0;
        this->sum = 0;
        this->range = 0;
        this->min = 9999999999999.0;
        this->max = -9999999999999.0;
    }
    ~SamplesWindow() {
        delete[] window;
    }
    inline void consume(double value) {
        sum -= window[windowIndex];
        window[windowIndex] = value;
        sum += value;
        windowIndex++;
        if (windowIndex >= windowSize) {
            windowIndex = 0;
        }
    }

    inline double calcRange() {
        double min = 9999999999999.0;
        double max = -999999999999.0;
        for (uint16_t i = 0; i < windowSize; i++) {
            if (window[i] < min) {
                min = window[i];
            }
            if (window[i] > max) {
                max = window[i];
            }
        }
        range = max - min;
        this->min = min;
        this->max = max;
        return range;
    }

    inline double getRange() {
        return range;
    }

    inline double getMin() {
        return min;
    }

    inline double getMax() {
        return max;
    }
};

#endif