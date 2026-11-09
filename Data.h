#ifndef DATA_H
#define DATA_H

#include <stdint.h> // Assigned integer lengths

// Define data structures and shared varibales

// CAN ids: Lower values get higher priority
// Motor commands: Received from ESP32 over CAN
// current command ids: 0x10x where x is motor #
// Motor telemetry: Teensys to ESP32 
// current data ids: 0x11x where x is motor #, q and d current measurements from motors
// dynamic data ids: 0x12x where x is motor #, velocity and angle measurements from motors
#define CAN_ID_MOTOR_COMMAND             (0x100 + MOTOR_NUMBER)
#define CAN_ID_MOTOR_CURRENT_TELEMETRY   (0x110 + MOTOR_NUMBER)
#define CAN_ID_MOTOR_DYNAMIC_TELEMETRY   (0x120 + MOTOR_NUMBER)

// Shared variable which motor number
extern const uint8_t MOTOR_NUMBER; 

// CAN baud rate for teensy and esp32
const uint32_t CAN_BAUD_RATE = 500000; // 5 kbps , same as ESP32

// Maximum magnitude of allowed current to be commanded 
const float MAX_CURRENT = 1.1f; // [A]

// Timing configuration [microsec]
const uint32_t TELEMETRY_INTERVAL_US = 10000; // Allowable time Teenys have to send telemetry
const uint32_t COMMAND_TIMEOUT_US = 100000; // Allowable time ESP32 has to send command

// Shared commanded current received from ESP32 buffer
extern char receivedCommandBuffer[128];


struct MotorCommands {

    float current_q_cmd; // q-axis current [A]

};

// Shared motor command structure
extern MotorCommands motorCommands;

// Motor telemetry 16 bytes so need two 8 byte CAN data frames per motor
struct MotorTelemetry {

    float current_q; // measured d-current [mA]
    float current_d; // measured q-current [mA]
    float velocity;  // measured ang vel [rad/s]
    float angle;     // measured motor position angle [rad]
    float timestamp;  // time of measurements [microsec]
    // voltage monitoring
  //  float current_set[4]; // set q-current from teensy [A]

};

// Shared motor telemetry structure
extern MotorTelemetry motorTelemetry;



#endif
