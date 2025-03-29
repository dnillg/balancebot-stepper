#ifndef MOTOR_OUTPUT_FILTER_HPP
#define MOTOR_OUTPUT_FILTER_HPP

#include <Arduino.h>
#include "MotorPosition.hpp"

class MotorOutputFilter
{
public:
  virtual double filter(double input, MotorPosition motorPos) = 0;
};

class MotorOutputFilterChain {
  private:
    MotorOutputFilter* current = nullptr;
    MotorOutputFilterChain* next = nullptr;
  public:
    ~MotorOutputFilterChain() {
      if (next != nullptr) {
        delete next;
      }
    }
    MotorOutputFilterChain(MotorOutputFilter* filter) : current(filter), next(nullptr) {}
    MotorOutputFilterChain() : current(nullptr), next(nullptr) {}
    
    double filter(double input, MotorPosition motorPos);

    void add(MotorOutputFilter* filter) {
      if (this->current == nullptr) {
        this->current = filter;
      } else if (this->next == nullptr) {
        this -> next = new MotorOutputFilterChain(filter);
      } else {
        this->next->add(filter);
      }
    }
 
};

#endif