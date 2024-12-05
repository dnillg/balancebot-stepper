#include <Arduino.h>

#include <BluetoothSerial.h>
#include <TMC5160.h>
#include <TMCStepper.h>
#include <AccelStepper.h>
#include <SparkFun_BNO080_Arduino_Library.h>

#include "config.h"
#include "FailSafe.hpp"
#include "IMU.h"
#include "Motor.hpp"
#include "SpeedAggregator.hpp"
#include "models/MotorOutput.hpp"
#include "Control.hpp"
//#include "models/RemoteControl.hpp"

// ----------------------------------------------------------------------------
// Types
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Function Declarations
// ----------------------------------------------------------------------------

//Tasks
void controlTask(void *pvParameters);
void failsafeTask(void *pvParameters);

void halt();

// Timer functions
void IRAM_ATTR motorLeftTimerFunction();
void IRAM_ATTR motorRightTimerFunction();

// Configuration Structures
MotorPins createMotorLeftPins();
MotorPins createMotorRightPins();
MotorConfig createMotorConfig();

// ----------------------------------------------------------------------------
// Global Variables
// ----------------------------------------------------------------------------


struct GlobalState
{
  boolean initialized = false;
  FailSafe failSafe;
  BluetoothSerial SerialBT;
  Control control;
  IMU imu;
  Motor leftMotor = Motor(createMotorConfig(), createMotorLeftPins(), motorLeftTimerFunction, 0);
  Motor rightMotor = Motor(createMotorConfig(), createMotorRightPins(), motorRightTimerFunction, 1);
  uint64_t counter = 0;
  MotorOutput motorOutput;
  //RemoteControl remoteControl;
  SpeedAggregator speedAgg100 = SpeedAggregator(10, 2); // 100ms
  SpeedAggregator speedAgg500 = SpeedAggregator(10, 2); // 500ms
};

GlobalState gstate;

// ----------------------------------------------------------------------------
// Mutex
// ----------------------------------------------------------------------------

portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

// ----------------------------------------------------------------------------
// Types
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Timer Functions
// ----------------------------------------------------------------------------

void IRAM_ATTR motorLeftTimerFunction()
{
  portENTER_CRITICAL_ISR(&timerMux);
  gstate.leftMotor.run();
  portEXIT_CRITICAL_ISR(&timerMux);
}
void IRAM_ATTR motorRightTimerFunction()
{
  portENTER_CRITICAL_ISR(&timerMux);
  gstate.rightMotor.run();
  portEXIT_CRITICAL_ISR(&timerMux);
}

// ----------------------------------------------------------------------------
// Setup
// ----------------------------------------------------------------------------

void setup()
{
  setCpuFrequencyMhz(160);
  delay(100);
  Serial.begin(921600);
  delay(333);

  pinMode(PIN_TMC5160_LEFT_EN, OUTPUT);
  pinMode(PIN_TMC5160_LEFT_STP, OUTPUT);
  pinMode(PIN_TMC5160_LEFT_DIR, OUTPUT);
  digitalWrite(PIN_TMC5160_LEFT_EN, HIGH);

  pinMode(PIN_TMC5160_RIGHT_EN, OUTPUT);
  pinMode(PIN_TMC5160_RIGHT_STP, OUTPUT);
  pinMode(PIN_TMC5160_RIGHT_DIR, OUTPUT);
  digitalWrite(PIN_TMC5160_RIGHT_EN, HIGH);

  Serial.println("Initializing I2C");
  Wire.begin(PIN_BNO085_SDA, PIN_BNO085_SCL);
  Wire.setClock(400000);
  Serial.println("I2C initialized.");
  if (!gstate.imu.init())
  {
    Serial.println("Could not initialize IMU.");
    halt();
  }
  Serial.println("IMU initialized.");

  Serial.println("Initializing motors");
  gstate.leftMotor.init();
  gstate.rightMotor.init();
  Serial.println("Motors initialized.");

  Serial.println("Starting tasks");
  #if CONTROL_FAILSAFE_ENABLED == true
  xTaskCreatePinnedToCore(
      failsafeTask,
      "failsafe",
      2048,
      NULL,
      1,
      NULL,
      1);
  delay(500);
  Serial.println("Failsafe started.");
  #endif

  #if CONTROL_TASK_ENABLED == true
  delay(500);
  xTaskCreatePinnedToCore(
      controlTask,
      "ctrl",
      2048,
      NULL,
      2,
      NULL,
      1);
  Serial.println("ControlTask started.");
  #endif

  gstate.leftMotor.enable();
  gstate.rightMotor.enable();
  gstate.initialized = true;
  Serial.println("Initialization complete.");
}

int nextSpeed = 0;
uint64_t setSpeedAt=0;

void loop()
{
  #if CONTROL_TASK_ENABLED == true
  return;
  #endif
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    long speed = command.toInt();

    if (abs(speed) > 8000) {
      speed = 0;
    }
    if (speed == 1) {
      Serial.println(gstate.leftMotor.getStatus(), HEX);
      gstate.leftMotor.logStatus();
      Serial.println(gstate.rightMotor.getStatus(), HEX);
      gstate.rightMotor.logStatus();
      return;
    }
    if (speed == 2) {
      Serial.println(gstate.leftMotor.stepper.speed());
      return;
    }
    if (speed == 3) {
      Serial.println("Disabling motors");
      gstate.leftMotor.disable();
      gstate.rightMotor.disable();
      return;
    }
    if (speed == 4) {
      Serial.println("Left");
      Serial.println(gstate.leftMotor.driver.diag0_error());
      Serial.println(gstate.leftMotor.driver.drv_err());
      Serial.println("Right");
      Serial.println(gstate.rightMotor.driver.diag0_error());
      Serial.println(gstate.rightMotor.driver.drv_err());
      return;
    }
    if (speed == 5) {
      Serial.println(gstate.leftMotor.driver.reset());
      return;
    }
    if (speed == 6) {
      Serial.println("Enabling motors");
      gstate.leftMotor.enable();
      gstate.rightMotor.enable();
      return;
    }
    // Serial.print("Speed: ");
    // Serial.println(speed);
    // gstate.leftMotor.setSpeed(speed);
    // setSpeedAt = gstate.failSafe.getCurrentMs() + 20;
    nextSpeed = -speed;
    if (speed >= 10) {
      gstate.motorOutput.speedLeft = speed;
      gstate.motorOutput.speedRight = speed;
      Serial.print("Speed: ");
      Serial.println(speed);
      gstate.leftMotor.setSpeed(speed);
      gstate.rightMotor.setSpeed(speed);
      gstate.leftMotor.enable();
      gstate.rightMotor.enable();
    }
  }
}

void failsafeTask(void *pvParameters)
{
  while (!gstate.initialized)
  {
    delay(1);
  }
  while (true)
  {
    if (!gstate.failSafe.isAlive())
    {
      gstate.leftMotor.disable();
      gstate.rightMotor.disable();
      Serial.println("Failsafe activated!");
    }
    vTaskDelay(100 / portTICK_PERIOD_MS); // Allow other tasks to run
  }
}

void controlTask(void *pvParameters)
{
  const TickType_t xFrequency = pdMS_TO_TICKS(1000 / CONTROL_FREQUENCY); // 5 ms = 200 Hz
  while (true)
  {
    TickType_t xLastWakeTime = xTaskGetTickCount();

    if (gstate.initialized)
    {
      gstate.imu.getData();
      float currentRoll = gstate.imu.getRoll();
      gstate.control.setInputAngleRad(currentRoll);
      gstate.control.setInputSpeedAvg(gstate.speedAgg500.getSpeed());
      gstate.control.setInputTargetSpeedProportion(0.0); //TODO: Remote Control
      gstate.control.setInputSteerProportion(0.0);  //TODO: Remote Control

      gstate.control.compute();
      if (gstate.control.getCycleNo() % 50) {
        Serial.print("Roll: ");
        Serial.print(currentRoll);
        Serial.print(", Speed: ");
        Serial.print(gstate.speedAgg500.getSpeed());
        Serial.print(", Setpoint: ");
        Serial.print(gstate.control.getRollSetpoint());
        Serial.print(", Left: ");
        Serial.print(gstate.control.getSteps16Left());
        Serial.print(", Right: ");
        Serial.println(gstate.control.getSteps16Right());
      }
      gstate.leftMotor.setSpeed(gstate.control.getSteps16Left());
      gstate.rightMotor.setSpeed(gstate.control.getSteps16Right());

      uint32_t currentSpeed = gstate.motorOutput.speed();
      gstate.speedAgg100.consume(currentSpeed);
      gstate.speedAgg500.consume(currentSpeed);
      
      gstate.failSafe.heartBeat();
    }

    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
}

void halt()
{
  while (true)
  {
    delay(1000);
  }
}

MotorConfig createMotorConfig()
{
  MotorConfig motorConfig;
  motorConfig.rSense = MOTOR_R_SENSE;
  motorConfig.toff = MOTOR_TOFF;
  motorConfig.rmsCurrent = MOTOR_RMS_CURRENT;
  motorConfig.pwmAutoscale = MOTOR_PWM_AUTOSCALE;
  motorConfig.pwmMode = MOTOR_PWM_MODE;
  motorConfig.maxSpeed = MOTOR_MAX_SPEED;
  motorConfig.maxAcceleration = MOTOR_MAX_ACCELERATION;
  return motorConfig;
}

MotorPins createMotorLeftPins()
{
  MotorPins motorPins;
  motorPins.dir = PIN_TMC5160_LEFT_DIR;
  motorPins.stp = PIN_TMC5160_LEFT_STP;
  motorPins.clk = PIN_TMC5160_LEFT_CLK;
  motorPins.miso = PIN_TMC5160_LEFT_MISO;
  motorPins.cs = PIN_TMC5160_LEFT_CS;
  motorPins.sck = PIN_TMC5160_LEFT_SCK;
  motorPins.mosi = PIN_TMC5160_LEFT_MOSI;
  motorPins.en = PIN_TMC5160_LEFT_EN;
  return motorPins;
}

MotorPins createMotorRightPins()
{
  MotorPins motorPins;
  motorPins.dir = PIN_TMC5160_RIGHT_DIR;
  motorPins.stp = PIN_TMC5160_RIGHT_STP;
  motorPins.clk = PIN_TMC5160_RIGHT_CLK;
  motorPins.miso = PIN_TMC5160_RIGHT_MISO;
  motorPins.cs = PIN_TMC5160_RIGHT_CS;
  motorPins.sck = PIN_TMC5160_RIGHT_SCK;
  motorPins.mosi = PIN_TMC5160_RIGHT_MOSI;
  motorPins.en = PIN_TMC5160_RIGHT_EN;
  return motorPins;
}
