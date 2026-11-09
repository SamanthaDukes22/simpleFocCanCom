#ifndef WATCHDOG_H
#define WATCHDOG_H

#include <Arduino.h>

// Initialize motor safe handing and communication checks
void initWatchdog();

// Validate commanded q-axis current
bool validateCommand(float current_q);

// Update command watchdog timestamp
void updateCommandWatchdog();

// Check command timeout and motor safety
void checkMotorSafety();

// Enter safe state by commanding zero current
void enterSafeState();


#endif