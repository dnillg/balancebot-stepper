#include <Arduino.h>
#include <BluetoothSerial.h>
#include <math.h>

// Create a BluetoothSerial object
BluetoothSerial SerialBT;

// Define the name of the Bluetooth device
const char *bluetoothDeviceName = "balance-robot";

unsigned long lastPrintTime = 0;
const unsigned long printInterval = 50; // Interval to print metrics (50 ms)

// Function to generate a sine wave with a given frequency and phase
float generateSineWave(float time, float frequency, float phase) {
    return sin(2 * PI * frequency * time + phase);
}

void setup() {
    // Initialize the serial connection for debugging (optional)
    Serial.begin(921600);
    // Initialize the Bluetooth serial connection
    if (!SerialBT.begin(bluetoothDeviceName)) {
        Serial.println("Failed to initialize Bluetooth!" );
        while (true) {} // Stop execution if Bluetooth doesn't start
    }
    Serial.println("Bluetooth device started, ready to pair with name: " + String(bluetoothDeviceName));
}

void loop() {
    unsigned long currentTime = millis();
    
    // Check if it's time to print the metrics
    if (currentTime - lastPrintTime >= printInterval) {
        lastPrintTime = currentTime;
        float timeInSeconds = currentTime / 1000.0;

        float currentRoll     = generateSineWave(timeInSeconds, 0.5, 0) * 30 + 90;
        float targetRoll      = generateSineWave(timeInSeconds, 0.5, PI / 4) * 20 + 90;
        float currentSpeed    = generateSineWave(timeInSeconds, 0.2, PI / 2) * 10000;
        float targetSpeed     = generateSineWave(timeInSeconds, 0.2, PI / 3) * 10000;
        float steerInput      = generateSineWave(timeInSeconds, 0.3, PI / 6) * 100;
        float motorLeftOutput = generateSineWave(timeInSeconds, 0.4, PI / 5) * 10000;
        float motorRightOutput= generateSineWave(timeInSeconds, 0.4, PI / 7) * 10000;
        
        String metrics = "DIAG>" + String(currentRoll, 3) + "," +
                         String(targetRoll, 3) + "," +
                         String(currentSpeed, 3) + "," +
                         String(targetSpeed, 3) + "," +
                         String(steerInput, 3) + "," +
                         String(motorLeftOutput, 3) + "," +
                         String(motorRightOutput, 3);

        //Serial.println(metrics);
        SerialBT.println(metrics);
    }

    if (SerialBT.available()) {
        String command = SerialBT.readStringUntil('\n');
        Serial.println("Received command: " + command);
    }
    if (Serial.available()) {
        String command = Serial.readStringUntil('\n');
        Serial.println("Received command: " + command);
    }
}
