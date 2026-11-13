#include <ACAN_T4.h>
#include <math.h>
#include <string.h>

#include "CAN_Com.h"
#include "Data.h"
#include "Motor_Control.h"

void setupCAN() {

  ACAN_T4_Settings settings(CAN_BAUD_RATE);

  settings.mRxPin = 23;
  settings.mTxPin = 22;

  settings.mLoopBackMode = false;
  settings.mSelfReceptionMode = false;

  const ACANPrimaryFilter primaryFilters[] = {
    ACANPrimaryFilter(kData, kStandard, CAN_ID_MOTOR_COMMAND)
  };

  const uint32_t errorCode = ACAN_T4::can1.begin(settings, primaryFilters, 1);

  if (errorCode == 0) {
    Serial.println("Ready for CAN communication");
  } else {
    Serial.print("CAN initialization error: ");
    Serial.println(errorCode, HEX);
  }
}


void receiveCANCommand() {

  CANMessage message;

  if (ACAN_T4::can1.receive(message)) {

    if (!message.rtr &&
        !message.ext &&
        message.id == CAN_ID_MOTOR_COMMAND &&
        message.len == sizeof(float)) {

      float current_q_cmd;

      // Extract commanded current
      memcpy(&current_q_cmd, message.data, sizeof(float));

      // Validate commanded current
      if (!isfinite(current_q_cmd)) {
        Serial.println("ERROR: Invalid current command!");
        return;
      }

      if (fabs(current_q_cmd) > MAX_CURRENT) {
        Serial.println("ERROR: Current command exceeds limit!");
        return;
      }

      // Update motor current command
      setMotorCurrent(current_q_cmd);

      Serial.print("Received current command: ");
      Serial.print(current_q_cmd, 4);
      Serial.println(" A");
    }
  }
}

/*
#include <ACAN_T4.h>
#include <math.h>
#include <string.h>

#include "CAN_Com.h"
#include "Data.h"
#include "Motor_Control.h"

// Implementation of functions for CAN communication between Teensy and ESP32
// ACAN_T4 docuementation: https://github.com/pierremolinaro/acan-t4

uint32_t id = 0 ; // Frame identifier
bool ext = false ; // false ‐> standard frame, true ‐> extended frame

void setupCAN() {

  // returns settings object fully initialized with CAN bit settings for the wished bitrate
  ACAN_T4_Settings settings (CAN_BAUD_RATE) ; 

  settings.mRxPin = 23 ; // 13 or 23 allowed
  settings.mTxPin = 22 ; // 11 or 22 allowed

  // mLoopBackMode and mSelfReceptionMode
  settings.mLoopBackMode = false ;
  settings.mSelfReceptionMode = false ;

  // Initiate filters for cmd/telemetry handling
  const ACANPrimaryFilter primaryFilters [] = {
  ACANPrimaryFilter (kData, kStandard, CAN_ID_MOTOR_COMMAND) // Filter for motor cmd id
 // ACANPrimaryFilter (kData, kStandard, CAN_ID_MOTOR_CURRENT_TELEMETRY), // Filter for motor current data
 // ACANPrimaryFilter (kData, kStandard, CAN_ID_MOTOR_DYNAMIC_TELEMETRY) // Filter for motor dynamic data
  };

  // settings, filter array, Filter array size
  const uint32_t errorCode = ACAN_T4::can1.begin (settings, primaryFilters , 1) ; 

  // Configuration of the ACAN_T4::can1 driver returns an error code. value 0 = okay
  if (0 == errorCode) {
    Serial.print("Motor ") ;
    Serial.print(MOTOR_NUMBER);
    Serial.println(" ready for CAN communication");
  }
  else {
    Serial.print("ERROR: Motor ") ;
    Serial.print(MOTOR_NUMBER);
    Serial.print(" not initiated successfully") ;
    Serial.println (errorCode, HEX) ;
  }
  
}



void receiveCANCommand() {

  CANMessage message;

  if (ACAN_T4::can1.receive(message)) {

    // Verify standard data frame and correct motor command ID
    if (!message.rtr && !message.ext &&
        message.id == CAN_ID_MOTOR_COMMAND &&
        message.len == sizeof(float)) {

        if (handleCANCommand(message)) {
          Serial.print("Motor ");
          Serial.print(MOTOR_NUMBER);
          Serial.println(" accepted the current command!");
        }
    }
  }
}
/*
// Receive motor current command from ESP32
// Receive motor current command from teensy
void receiveCANCommand() {

  CANMessage message;

  if (ACAN_T4::can1.receive(message)) {

    // Verify standard data frame and correct motor command ID
    if (!message.rtr && !message.ext && 
        message.id == static_cast<uint32_t>(CAN_ID_MOTOR1_COMMAND) && 
        message.len == static_cast<uint32_t>(sizeof(float))) {
        handleCANCommand(message);
        Serial.print("Motor ");
        Serial.print(MOTOR_NUMBER);
        Serial.println(" successfully received command!");
    }
  }

}


bool handleCANCommand(CANMessage &message) {

  float current_q_cmd;

  // Extract commanded current from CAN message
  memcpy(&current_q_cmd, message.data, sizeof(float));

  // Validate commanded current
  if (!isfinite(current_q_cmd)) {
      Serial.println("ERROR: Invalid current command!");
      return false;
  }

  if (fabs(current_q_cmd) > MAX_CURRENT) {
      Serial.println("ERROR: Current command exceeds limit!");
      return false;
  }

  // Update motor current command
  setMotorCurrent(current_q_cmd);

  Serial.print("Received q-axis current command: ");
  Serial.print(current_q_cmd, 4);
  Serial.println(" A");

  return true;
}

// Handle motor command and store in motor command structure
bool handleCANCommand(CANMessage &message) {

  float current_q_cmd;

  // Extract commanded current from CAN message
  memcpy(&current_q_cmd, message.data, sizeof(float));

  // Validate commanded current
  if (!isfinite(current_q_cmd)) {
      Serial.println("ERROR: Invalid current command!");
      return;
  }

  if (fabs(current_q_cmd) > MAX_CURRENT) {
      Serial.println("ERROR: Current command exceeds limit!");
      return;
  }

  // Update motor current command
  setMotorCurrent(current_q_cmd);

  Serial.print("Motor ");
  Serial.print(MOTOR_NUMBER);
  Serial.println(" command has been validated");

}



// Transmit q and d current measurements
void transmitCurrentTelemetry() {

  /*
  // message object fully initialized by the default constructor, standard data frame w/ id = 0 and no data
  CANMessage message ;
  message.id = CAN_ID_MOTOR_CURRENT_TELEMETRY;


  // Set the message identifier. In a real code, we set here message data, and for an extended frame the ext boolean property
  const bool ok = ACAN_T4::can1.tryToSend (message) ;
  // We try to send the data message. Actually, we try to transfer it into the Driver transmit buffer. The
  // transfer succeeds if the buffer is not full. The tryToSend method returns false if the buffer is full, and true
  // otherwise. Note the returned value only tells if the transfer into the Driver transmit buffer is successful or not:
  // we have no way to know if the frame is actually sent on the the CAN network.


  // Create CAN message
  CANMessage message;

  // Set CAN message ID
  message.id = CAN_ID_MOTOR_CURRENT_TELEMETRY;

  // Standard CAN data frame
  message.ext = false;
  message.rtr = false;

  // Message is 8 bytes
  message.len = 8;

  // Package q and d currents [mA]
  memcpy(&message.data[0], &motorTelemetry.current_q, sizeof(float));
  memcpy(&message.data[4], &motorTelemetry.current_d, sizeof(float));

  // Transmit CAN message
  const bool ok = ACAN_T4::can1.tryToSend(message);

  if (!ok) {
      Serial.println("ERROR: Current telemetry CAN buffer full!");
  }

  Serial.print("Motor ");
  Serial.print(MOTOR_NUMBER);
  Serial.println(" current CAN frame accepted into Driver transmit buffer!");

}

// Transmit motor velocity and angle measurements
void transmitDynamicTelemetry() {

  // Create CAN message
  CANMessage message;

  // Set CAN message ID
  message.id = CAN_ID_MOTOR_DYNAMIC_TELEMETRY;

  // Standard CAN data frame
  message.ext = false;
  message.rtr = false;

  // Message is 8 bytes
  message.len = 8;

  // Package velocity and angle [V , rad]
  memcpy(&message.data[0], &motorTelemetry.velocity, sizeof(float));
  memcpy(&message.data[4], &motorTelemetry.angle, sizeof(float));

  // Transmit CAN message
  const bool ok = ACAN_T4::can1.tryToSend(message);

  if (!ok) {
      Serial.println("ERROR: Dynamic telemetry CAN buffer full!");
  }

  Serial.print("Motor ");
  Serial.print(MOTOR_NUMBER);
  Serial.println(" dynamic CAN frame accepted into Driver transmit buffer");

}
*/












