#ifndef CAN_COM_H
#define CAN_COM_H

#include <ACAN_T4.h>
#include <Arduino.h>

// Initialize CAN communication
void setupCAN();	

// Receive motor current command from ESP32
void receiveCANCommand();

// Handle motor command 
void handleCANCommand(CANMessage &message);

// Transmit d-axis and q-axis current measurements
void transmitCurrentTelemetry();

// Transmit motor velocity and angle measurements
void transmitDynamicTelemetry();

// add transmit fucntions for time/voltage data, etc.

#endif


