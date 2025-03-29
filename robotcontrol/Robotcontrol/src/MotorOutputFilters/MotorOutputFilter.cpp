#include "MotorOutputFilter.hpp"

double MotorOutputFilterChain::filter(double input, MotorPosition motorPos) {
  if (current != nullptr) {
    input = current->filter(input, motorPos);
  }
  if (next != nullptr) {
    return next->filter(input, motorPos);
  }
  return input;
}