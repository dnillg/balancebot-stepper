#ifndef MODELS_CONTROLINPUT_HPP_
#define MODELS_CONTROLINPUT_HPP_

struct ControlInput {
  double targetSpeedProportion = 0; // -1 to 1
  double steerProportion = 0; // -1 to 1
  double step16SpeedSetpoint = 0;
  double steerOffset = 0;
  double rollRadAngle = -PI / 2;
  double speedStepAvg = 0;
};

#endif // MODELS_CONTROLINPUT_HPP_