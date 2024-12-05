#include <Arduino.h>

#ifndef MODELS_REMOTECONTROL_HPP_
#define MODELS_REMOTECONTROL_HPP_

struct RemoteControl {
  public:
    int16_t speed;
    int16_t direction;
};

#endif // MODELS_REMOTECONTROL_HPP_