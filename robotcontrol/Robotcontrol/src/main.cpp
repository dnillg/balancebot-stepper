#include <Arduino.h>

#include <BluetoothSerial.h>
#include <HardwareSerial.h>
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
#include "CommandParser.hpp"
#include "StationaryCutoff.hpp"

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

// ----------------------------------------------------------------------------
// Global Variables
// ----------------------------------------------------------------------------


struct GlobalState
{
  boolean initialized = false;
  HardwareSerial ioSerial = HardwareSerial(2);
  FailSafe failSafe;
  BluetoothSerial SerialBT;
  Control control;
  IMU imu;
  Motor leftMotor = Motor(createMotorLeftPins(), motorLeftTimerFunction, 0, FORWARD);
  Motor rightMotor = Motor(createMotorRightPins(), motorRightTimerFunction, 1, BACKWARD);
  uint64_t counter = 0;
  //RemoteControl remoteControl;
  SpeedAggregator speedAgg500 = SpeedAggregator(10, 10); // 500ms
  SpeedAggregator speedAgg250 = SpeedAggregator(10, 5); // 250ms
  CommandParser commandParser = CommandParser(&leftMotor, &rightMotor, &control);
  StationaryCutoff stationaryCutoff = StationaryCutoff(20, 20, STATIONARY_CUTOFF_ROLL_RANGE, STATIONARY_CUTOFF_SPEED_RANGE);
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

  #if IO_SERIAL_ENABLED == true
  gstate.ioSerial.begin(IO_SERIAL_BAUD);
  #endif

  gstate.leftMotor.enable();
  gstate.rightMotor.enable();
  gstate.initialized = true;
  Serial.println("Initialization complete.");
}

void loop()
{
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    Serial.print("Received: ");
    Serial.println(input);
    gstate.commandParser.handleLine(input);
  }
}

void failsafeTask(void *pvParameters)
{
  while (!gstate.initialized)
  {
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
  while (true)
  {
    if (!gstate.failSafe.isAlive())
    {
      gstate.leftMotor.disable();
      gstate.rightMotor.disable();
      Serial.println("Failsafe activated!");
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
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
      gstate.control.setInputSpeedAvg250(gstate.speedAgg250.getSpeed());
      gstate.control.setInputSpeedAvg500(gstate.speedAgg500.getSpeed());

      gstate.control.compute();

      int16_t step16SpeedLeft = gstate.control.getSteps16Left();
      int16_t step16SpeedRight = gstate.control.getSteps16Right();
      String output = "Roll: " + String(currentRoll, 4) + 
                ", Speed: " + String(gstate.speedAgg500.getSpeed()) + 
                ", Setpoint: " + String(gstate.control.getRollSetpoint(), 4) + 
                ", Left: " + String(step16SpeedLeft) + 
                ", Right: " + String(step16SpeedRight);
      #if PRINT_CONTROL_STATE == true 
      if (gstate.control.getCycleNo() % 40 == 0) {
        Serial.println(output);
      }
      #endif
      #if IO_SERIAL_ENABLED == true
      if (gstate.control.getCycleNo()% 10 == 0) {
        gstate.ioSerial.println("ALIVE>");
      }
      #endif
      gstate.leftMotor.setSpeed(gstate.stationaryCutoff.filter(step16SpeedLeft)); 
      gstate.rightMotor.setSpeed(gstate.stationaryCutoff.filter(step16SpeedRight));

      int16_t controlSpeed = (((int32_t)step16SpeedLeft) + step16SpeedRight) / 2;
      #if STATIONARY_CUTOFF_ENABLED == true
      gstate.stationaryCutoff.consumeRollAndMotorSpeed(currentRoll, controlSpeed);
      int16_t currentSpeed = gstate.stationaryCutoff.filter(controlSpeed);
      #else
      int16_t currentSpeed = controlSpeed;
      #endif
      gstate.speedAgg500.consume(currentSpeed);
      gstate.speedAgg250.consume(currentSpeed);

      
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
