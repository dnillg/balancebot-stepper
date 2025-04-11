// my_class.h
#ifndef CONFIG_H // include guard
#define CONFIG_H

// ----------------------------------------------------------------------------
// Logging
// ----------------------------------------------------------------------------

#define PRINT_CONTROL_STATE false

// ----------------------------------------------------------------------------
// Motor Config
// ----------------------------------------------------------------------------

#define MOTOR_R_SENSE 0.075f
#define MOTOR_TOFF 5
#define MOTOR_RMS_CURRENT 1500
#define MOTOR_IRUN 26
#define MOTOR_IHOLD 18
#define MOTOR_IHOLDDELAY 5
#define MOTOR_MAX_MICROSTEPPING 16
#define MOTOR_PWM_AUTOSCALE true
#define MOTOR_PWM_MODE true

#define MOTOR_MAX_SPEED 10000
#define MOTOR_MAX_ACCELERATION 12000

// ----------------------------------------------------------------------------
// IO Serial Config
// ----------------------------------------------------------------------------

#define IO_SERIAL_BAUD 115200
#define IO_SERIAL_PORT 2
#define IO_SERIAL_ENABLED true

// ----------------------------------------------------------------------------
// IMU Config
// ----------------------------------------------------------------------------

#define BNO085_I2C_ADDR 0x4B
#define BNO085_ROTATION_VECTOR_PERIOD 5 // 5ms = 200Hz

// ----------------------------------------------------------------------------
// BALANANCE CONTROL
// ----------------------------------------------------------------------------
#define FAILSAFE_ENABLED true
#define FAILSAFE_THRESHOLD_MS 100
#define FAILSAFE_MAX_ROLL_OFFSET 0.9

#define CONTROL_DRY_RUN false
#define CONTROL_TASK_ENABLED true
#define ENABLE_CONTROL_ON_STARTUP false
#define CONTROL_FREQUENCY 200

#define CONTROL_TARGET_ROLL -1.495
#define CONTROL_MAX_TARGET_STEP16_SPEED 2800
#define CONTROL_MAX_SPEED_ROLL_RAD_OFFSET 0.75 // 40-45 degrees in radians
#define CONTROL_MAX_STEER_STEP16_OFFSET 3200

#define CONTROL_ROLL_PID_KP 44300
#define CONTROL_ROLL_PID_KI 5000
#define CONTROL_ROLL_PID_KD 94

#define CONTROL_SPEED_PID_KP 0.000045
#define CONTROL_SPEED_PID_KI 0.0
#define CONTROL_SPEED_PID_KD 0.0015

#define CONTROL_MAX_STEP16_SPEED 10000
#define CONTROL_MAX_ACCELERATION 40000

#define CONTROL_MAX_SPEED_PID_OUTPUT_ACCELERATION 2 // Might be not needed

#define STATIONARY_CUTOFF_ENABLED true
#define STATIONARY_CUTOFF_SPEED_RANGE 200.0
#define STATIONARY_CUTOFF_ROLL_RANGE 0.0065

// ----------------------------------------------------------------------------
// Pins
// ----------------------------------------------------------------------------

#define PIN_TMC5160_LEFT_DIR 22
#define PIN_TMC5160_LEFT_STP 25
#define PIN_TMC5160_LEFT_CLK -1
#define PIN_TMC5160_LEFT_MISO 19
#define PIN_TMC5160_LEFT_CS 21
#define PIN_TMC5160_LEFT_SCK 18
#define PIN_TMC5160_LEFT_MOSI 23
#define PIN_TMC5160_LEFT_EN 32

#define PIN_TMC5160_RIGHT_DIR 4
#define PIN_TMC5160_RIGHT_STP 26
#define PIN_TMC5160_RIGHT_CLK -1
#define PIN_TMC5160_RIGHT_MISO 19
#define PIN_TMC5160_RIGHT_CS 5
#define PIN_TMC5160_RIGHT_SCK 18
#define PIN_TMC5160_RIGHT_MOSI 23
#define PIN_TMC5160_RIGHT_EN 33

#define PIN_BNO085_SCL 14
#define PIN_BNO085_SDA 13
#define PIN_BNO085_ADO -1
#define PIN_BNO085_CS 15
#define PIN_BNO085_INT 34
#define PIN_BNO085_RST 27
#define PIN_BNO085_PSO 2
#define PIN_BNO085_PSI -1

#define PIN_SERIAL_RX 17
#define PIN_SERIAL_TX 16

#endif /* CONFIG_H */