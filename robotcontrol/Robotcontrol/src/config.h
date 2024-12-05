// my_class.h
#ifndef CONFIG_H // include guard
#define CONFIG_H

// ----------------------------------------------------------------------------
// Motor Config
// ----------------------------------------------------------------------------

#define MOTOR_R_SENSE 0.075f
#define MOTOR_TOFF 5
#define MOTOR_RMS_CURRENT 1100
#define MOTOR_IRUN 20
#define MOTOR_IHOLD 4
#define MOTOR_IHOLDDELAY 700
#define MOTOR_MAX_MICROSTEPPING 16
#define MOTOR_PWM_AUTOSCALE true
#define MOTOR_PWM_MODE true

#define MOTOR_MAX_SPEED 8000
#define MOTOR_MAX_ACCELERATION 20000

// ----------------------------------------------------------------------------
// IMU Config
// ----------------------------------------------------------------------------

#define BNO085_I2C_ADDR 0x4B
#define BNO085_ROTATION_VECTOR_PERIOD 5 // 5ms = 200Hz

// ----------------------------------------------------------------------------
// CONTROL
// ----------------------------------------------------------------------------
#define FAILSAFE_ENABLED false
#define FAILSAFE_THRESHOLD_MS 1000

#define CONTROL_TASK_ENABLED true
#define CONTROL_FREQUENCY 200 // Hz

#define CONTROL_TARGET_ROLL -1.53
#define CONTROL_MAX_ROLL_SPEED_STEPS 1000
#define CONTROL_MAX_TILT_RAD_OFFSET 0.1745 // 10 degrees in radians
#define CONTROL_MAX_STEER_STEP16_OFFSET 200

#define CONTROL_ROLL_PID_KP 14000
#define CONTROL_ROLL_PID_KI 1000
#define CONTROL_ROLL_PID_KD 600

#define CONTROL_SPEED_PID_KP 0.0
#define CONTROL_SPEED_PID_KI 0.0
#define CONTROL_SPEED_PID_KD 0.0

#define CONTROL_MAX_STEP16_SPEED 8000
#define CONTROL_MAX_ACCERATION 20000

#define AVG_SPEED_SAMPLES 20

// ----------------------------------------------------------------------------
// Pins
// ----------------------------------------------------------------------------

#define PIN_TMC5160_LEFT_DIR 22
#define PIN_TMC5160_LEFT_STP 25 // 25
#define PIN_TMC5160_LEFT_CLK -1
#define PIN_TMC5160_LEFT_MISO 19
#define PIN_TMC5160_LEFT_CS 21
#define PIN_TMC5160_LEFT_SCK 18
#define PIN_TMC5160_LEFT_MOSI 23
#define PIN_TMC5160_LEFT_EN 32

#define PIN_TMC5160_RIGHT_DIR 4
#define PIN_TMC5160_RIGHT_STP 16 // 26
#define PIN_TMC5160_RIGHT_CLK -1
#define PIN_TMC5160_RIGHT_MISO 19
#define PIN_TMC5160_RIGHT_CS 5
#define PIN_TMC5160_RIGHT_SCK 18
#define PIN_TMC5160_RIGHT_MOSI 23
#define PIN_TMC5160_RIGHT_EN 26 // 33

#define PIN_BNO085_SCL 14
#define PIN_BNO085_SDA 13
#define PIN_BNO085_ADO -1
#define PIN_BNO085_CS 15
#define PIN_BNO085_INT 34
#define PIN_BNO085_RST 27
#define PIN_BNO085_PSI 2
#define PIN_BNO085_PSO 12

#define PIN_SERIAL_RX 17
#define PIN_SERIAL_TX -1 //old: 25 //new: 16

#endif /* CONFIG_H */