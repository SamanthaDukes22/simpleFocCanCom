#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <Arduino.h>

//#define  SIMPLEFOC_TEENSY_DEBUG

// Initialize motor hardware and SimpleFOC
bool initMotor();

// Set commanded q-axis current [A]
void setMotorCurrent(float current_q_cmd);

// Execute motor FOC with current command from ESP32
void updateMotor();

// Update measured motor telemetry
void updateMotorTelemetry();


#endif