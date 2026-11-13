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
    //Serial.println(motorSensor01.getAngle(), 6);
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

<<<<<<< HEAD
 // static uint32_t lastTelemetryTime = 0;

  // Sample motor data @ 100Hz and send to ESP32 
 /* if (micros() - lastTelemetryTime >= 10000) { // 100 Hz
=======
  //static uint32_t lastTelemetryTime = 0;

  // Sample motor data @ 100Hz and send to ESP32 
  //if (micros() - lastTelemetryTime >= 10000) { // 100 Hz
>>>>>>> 844840d8f64ff8e82e0bea67050e14404b34a4a0

   //lastTelemetryTime = micros();

  //    updateMotorTelemetry();

   // transmitCurrentTelemetry();
   // transmitDynamicTelemetry();

<<<<<<< HEAD
  }
*/
=======
  

>>>>>>> 844840d8f64ff8e82e0bea67050e14404b34a4a0
}
 
