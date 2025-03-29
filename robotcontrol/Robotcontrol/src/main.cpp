#include <Arduino.h>

#include <HardwareSerial.h>
#include <TMC5160.h>
#include <TMCStepper.h>
#include <AccelStepper.h>
#include <SparkFun_BNO080_Arduino_Library.h>

#include "config.h"
#include "IMU.h"
#include "Motor.hpp"
#include "SpeedAggregator.hpp"
#include "models/MotorOutput.hpp"
#include "Control.hpp"
#include "CommandParser.hpp"
#include "MotorOutputFilters/MotorOutputFilter.hpp"
#include "MotorOutputFilters/FailSafe.hpp"
#include "MotorOutputFilters/StationaryCutoff.hpp"
#include "MotorOutputFilters/SteeringMotorOutputFilter.hpp"
#include "SerialUnits/SerialUnits.hpp"
#include "MotorPosition.hpp"
#include "Motors.hpp"
#include "SerialUnits/SerialUnitProcessor.hpp"
#include "Listeners/ControlToggleListener.hpp"
#include "Listeners/RemoteControlListener.hpp"

// ----------------------------------------------------------------------------
// Function Declarations
// ----------------------------------------------------------------------------

//Tasks
void controlTask(void *pvParameters);
void failsafeTask(void *pvParameters);
void ioSerialReadTask(void *pvParameters);

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
  Control control;
  IMU imu;
  Motor leftMotor = Motor(createMotorLeftPins(), motorLeftTimerFunction, 0, FORWARD, LEFT);
  Motor rightMotor = Motor(createMotorRightPins(), motorRightTimerFunction, 1, BACKWARD, RIGHT);
  Motors motors = Motors(&leftMotor, &rightMotor);
  SpeedAggregator speedAgg500 = SpeedAggregator(10, 10); // 500ms
  SpeedAggregator speedAgg250 = SpeedAggregator(10, 5); // 250ms
  CommandParser commandParser = CommandParser(&leftMotor, &rightMotor, &control);
  FailSafeMotorOutputFilter failSafe = FailSafeMotorOutputFilter(FAILSAFE_THRESHOLD_MS);
  StationaryCutoffMotorOutputFilter stationaryCutoff = StationaryCutoffMotorOutputFilter(20, 20, STATIONARY_CUTOFF_ROLL_RANGE, STATIONARY_CUTOFF_SPEED_RANGE);
  SteeringMotorOutputFilter steeringMotorOutputFilter = SteeringMotorOutputFilter(REMOTE_MAX_STEERING_OFFSET);
  MotorOutputFilterChain motorOutputFilterChain = MotorOutputFilterChain();
  RemoteControlListener remoteControlListener = RemoteControlListener(&control, &steeringMotorOutputFilter);
  ControlToggleListener controlToggleListener = ControlToggleListener(&control, &motors);
  SerialUnitProcessor serialUnitProcessor = SerialUnitProcessor();
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
  Serial.begin(115200);
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
  gstate.motors.init();
  Serial.println("Motors initialized.");

  Serial.println("Initializing IO Serial");
  #if IO_SERIAL_ENABLED == true
  gstate.ioSerial.begin(IO_SERIAL_BAUD);
  #endif
  Serial.println("IO Serial initialized.");

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
  Serial.println("Initializing IO Serial Task");  
  xTaskCreatePinnedToCore(
      ioSerialReadTask,
      "ioSerial",
      2048,
      NULL,
      4,
      NULL,
      1);
  Serial.println("IO Serial Task started.");
  #endif

  Serial.println("Initializing Motor Output Filter Chain");
  if(FAILSAFE_ENABLED) {
    gstate.motorOutputFilterChain.add(&gstate.failSafe);
  }
  gstate.motorOutputFilterChain.add(&gstate.stationaryCutoff);
  Serial.println("Motor Output Filter Chain initialized.");

  if (ENABLE_CONTROL_ON_STARTUP) {
    gstate.motors.enable();
    Serial.println("Motors enabled.");
    gstate.control.enable();
    Serial.println("Control enabled.");
  } else {
    gstate.motors.disable();
    Serial.println("Motors disabled.");
    gstate.control.disable();
    Serial.println("Control disabled.");  
  }

  gstate.serialUnitProcessor.addListener(&gstate.remoteControlListener);
  gstate.serialUnitProcessor.addListener(&gstate.controlToggleListener);

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

void ioSerialReadTask(void *pvParameters)
{
  while (true)
  {
    if (gstate.ioSerial.available()) {
      String input = gstate.ioSerial.readStringUntil('\n');
      Serial.print("REC-IO: ");
      Serial.println(input);
      //gstate.serialUnitProcessor.process(SerialUnitFactory::readAlias(input), input);
    }
    vTaskDelay(portTICK_PERIOD_MS);
  }
}

void controlTask(void *pvParameters)
{
  const TickType_t xFrequency = pdMS_TO_TICKS(1000 / CONTROL_FREQUENCY); // 5 ms = 200 Hz
  gstate.serialUnitProcessor.run();
  while (true)
  {
    TickType_t xLastWakeTime = xTaskGetTickCount();

    if (gstate.initialized)
    {
      gstate.imu.getData();
      float currentRoll = gstate.imu.getRoll();
      //Serial.print("Roll: "); Serial.println(currentRoll, 6);
      gstate.control.setInputAngleRad(currentRoll);
      gstate.control.setInputSpeedAvg250(gstate.speedAgg250.getSpeed());
      gstate.control.setInputSpeedAvg500(gstate.speedAgg500.getSpeed());

      gstate.control.compute();

      int16_t step16SpeedLeft = gstate.control.getSteps16Left();
      int16_t step16SpeedRight = gstate.control.getSteps16Right();
      uint16_t cycleNo = gstate.control.getCycleNo();
      #if IO_SERIAL_ENABLED == true
      if (cycleNo % 10 == 0) {
        // 5ms * 5 = 25ms
        auto line = DiagSerialUnit(gstate.control.getMillis(), currentRoll, gstate.control.getRollSetpoint(), gstate.speedAgg500.getSpeed(), 0.0, step16SpeedLeft, step16SpeedRight, 0.0, 0.0).toString();
        gstate.ioSerial.println(line);
        gstate.ioSerial.flush();
      }
      #endif
      gstate.leftMotor.setSpeed(gstate.motorOutputFilterChain.filter(step16SpeedLeft, LEFT)); 
      gstate.rightMotor.setSpeed(gstate.motorOutputFilterChain.filter(step16SpeedRight, RIGHT));

      int16_t controllSpeed = (((int32_t)step16SpeedLeft) + step16SpeedRight) / 2;
      gstate.stationaryCutoff.consumeRollAndMotorSpeed(currentRoll, controllSpeed);
      gstate.speedAgg500.consume(controllSpeed);
      gstate.speedAgg250.consume(controllSpeed);

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
