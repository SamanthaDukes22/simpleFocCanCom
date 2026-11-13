#include <SimpleFOC.h>

#include "Data.h"
#include "Motor_Control.h"

// Implementation of functions for FOC torque control
// SimpleFOC guide: https://docs.simplefoc.com/torque_control

// Motor 1 implementation

// InlineCurrentSensor constructor
//  - mVpA  - mV per Amp ratio
//  - phA   - A phase adc pin
//  - phB   - B phase adc pin
//  - phC   - C phase adc pin (optional)
InlineCurrentSense current_sense  = InlineCurrentSense( 132.0f,  A0, A1, A2 );

// MagneticSensorSPI(int cs, float_cpr, int_angle_register, long_clock_speed)
// cs             - SPI chip select pin. Test : 0
// bit_resolution - magnetic sensor resolution - AS508A : 14
// angle_register - (optional) angle read register - default 0x3FFF
// clock_speed    - (optional) SPI clock speed - default 1MHz -
MagneticSensorSPI motorSensor01 = MagneticSensorSPI( 0, 14, 0x3FFF );

// BLDCMotor(pole pair number, phase resistance [Ohms], KV rating [rpm/V], q-axis inductance [H], d-axis inductance [H])
// Inductance only needed for high speed applications. Has the least impact on performance of the motor.
BLDCMotor motor = BLDCMotor( 11, 6.34, 117, 0.00108, 0.00108 );

//  BLDCDriver6PWM( int phA_h, int phA_l, int phB_h, int phB_l, int phC_h, int phC_l, int en)
//  - phA_h, phA_l - A phase pwm pin high/low pair
//  - phB_h, phB_l - B phase pwm pin high/low pair
//  - phB_h, phC_l - C phase pwm pin high/low pair
//  - enable pin    - (optional input)
BLDCDriver6PWM driver = BLDCDriver6PWM(6,9, 8,7, 2,3, 40);


// Initialize Motor
bool initMotor() {
 
  pinMode(23, OUTPUT);

  SimpleFOCDebug::enable(&Serial);
  pinMode(23, OUTPUT);
 // SimpleFOCDebug::enable(&Serial);
  // Sensor Initialization Encoder uses SPI_MODE1
  motorSensor01.spi_mode = SPI_MODE1;
  // I fried SPI0 on the first Teensy.
  motorSensor01.init(&SPI1);
  motor.linkSensor(&motorSensor01);
  


  // Calibration parameters, To find parameters: "find_sensor_offset_and_direction.ino"
  //motor.sensor_direction = Direction::CCW;
  //motor.zero_electric_angle = 1.58;
 
  // Limit voltage and current
  motor.current_limit = MAX_CURRENT; // set current limit to 1 A
  motor.voltage_limit = 6.0; // set voltage limit to 6 V

  // Set velocity low-pass filter value
  motor.LPF_velocity.Tf = 0.1f;

  // pwm frequency to be used [Hz] for atmega328 either 4k or 32kHz, esp32/stm32/teensy configurable
  driver.pwm_frequency = 100000;

  // dead_zone [0,1] - default 0.02 - 2%
  driver.dead_zone = 0.02;

  // power supply voltage [V]: Max DC voltage allowed - default 0.5 power supply.
  driver.voltage_power_supply = 11.00;

  // init driver
  Serial.print("Driver init ");
  if (driver.init()) Serial.println("success!");
  else{
    Serial.println("failed!");
    return false;
  }
 
  // link current sense and driver
  current_sense.linkDriver(&driver);

  // init current sense
  if (current_sense.init())  Serial.println("Current sense init success!");
  else{
    Serial.println("Current sense init failed!");
    return false;
  }


  // link motor and current sense
  motor.linkDriver(&driver);
  motor.linkCurrentSense(&current_sense);

  // Downsample Motion
  //motorA.motion_downsample = 5; // downsample 5 times

  // Set the torque control type
  motor.torque_controller = TorqueControlType::foc_current;

  // Configure current control set motion control loop to be used
  motor.controller = MotionControlType::torque;

  // use monitoring with serial comment out if not needed
  motor.useMonitoring(Serial);

  // Initialize motor
  motor.init();
  
  float bandwidth = _2PI*3142;
  
  // PID tunning
  motor.PID_current_q.P = 0.034796103;
  motor.PID_current_q.I = 14408.74411;
  motor.PID_current_d.P = 0.034796103;
  motor.PID_current_d.I = 14408.74411;

  // LPF tunning
  motor.LPF_current_d.Tf = 1.0f / (bandwidth * 5.0f);
  motor.LPF_current_q.Tf = 1.0f / (bandwidth * 5.0f);
  
  // Align sensor and start FOC
  motor.initFOC();

  if (motor.initFOC() == 0) {
    Serial.println("FOC initialization failed!");
    return false;
  }

  // Start with zero q-axis current command
  motorCommands.current_q_cmd = 0.0f;

  Serial.println(F("Motor ready."));

  return true;

}


<<<<<<< HEAD
// Execute motor FOC based on current command received
void updateMotor() {

  // main FOC algorithm function
  motor.loopFOC();
  // Motion control function
  motor.move(motorCommands.current_q_cmd);
  //Serial.println(motorSensor01.getSensorAngle());
  // user communication
 // command.run();

}


=======
>>>>>>> 844840d8f64ff8e82e0bea67050e14404b34a4a0
// Set commanded q-axis current [A]
void setMotorCurrent(float current_q_cmd) {

  motorCommands.current_q_cmd = current_q_cmd;

}


// Execute motor FOC based on current command received
void updateMotor() {

 // Serial.println(motorSensor01.getAngle(), 6);
  // main FOC algorithm function
  motor.loopFOC();
  // Motion control function
  motor.move(motorCommands.current_q_cmd);

}


// Update measured motor telemetry
void updateMotorTelemetry() {

  // Read motor electrical angle [rad]
  float electrical_angle = motor.electricalAngle();

  // Read measured d-axis and q-axis currents
  DQCurrent_s currents = current_sense.getFOCCurrents(electrical_angle);

  motorTelemetry.current_q = currents.q;
  motorTelemetry.current_d = currents.d;

  // Read motor shaft velocity and angle
  motorTelemetry.velocity = motor.shaftVelocity();
  motorTelemetry.angle = motor.shaftAngle();

  // Record measurement timestamp
  motorTelemetry.timestamp = micros();

}






