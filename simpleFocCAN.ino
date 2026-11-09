#include <Arduino.h>

#include "CAN_Com.h"
#include "Dancing_Functions.h"
#include "Data.h"
#include "Motor_Control.h"
#include "Watchdog.h"

// Motor 1 implementation

void setup() {

  Serial.begin(115200); // output the debug information to the serial

  if (!initMotor()) {
      Serial.println("Motor initialization failed!");
      while (true) {}
  }

  setupCAN();

 // initMotorDancing();

 // initWatchdog();

}


void loop() {

  receiveCANCommand();

  updateMotor();

  //float foc_frequency = 1000000.0f / motor.loopfoc_time_us;
  //float motion_frequency = 1000000.0f / motor.move_time_us;

  static uint32_t lastTelemetryTime = 0;

  // Sample motor data @ 100Hz and send to ESP32 
  if (micros() - lastTelemetryTime >= 10000) { // 100 Hz

    lastTelemetryTime = micros();

    updateMotorTelemetry();

    transmitCurrentTelemetry();
    transmitDynamicTelemetry();

  }

}
 
