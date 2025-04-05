#ifndef IMU_H_
#define IMU_H_

#include <SparkFun_BNO080_Arduino_Library.h>

class IMU {
    private:
        BNO080* bno080;
        float roll = 0.0f;
        float pitch = 0.0f;
        float yaw = 0.0f;
    public:
        IMU() {}
        bool init();
        void getData();
        float getRoll();
        float getPitch();
        float getYaw();
};

#endif // IMU_H_