#ifndef ATOMMOTIONDEMO_ATOMMOTIONDEMO_H
#define ATOMMOTIONDEMO_ATOMMOTIONDEMO_H


#include <Wire.h>
#include <M5Atom.h>
#include <M5_DLight.h>
#include "AtomMotion.h"
#include <Arduino.h>
#include <array>


#define PORT_B  23
#define PORT_C  22
#define RED     0xFF0000
#define ORANGE  0xFF8000
#define YELLOW  0xFFFF00
#define GREEN   0x00FF00
#define BLUE    0x0000FF
#define INDIGO  0x4B0082
#define VIOLET  0xEE82EE
#define BLACK   0x000000
#define MAGENTA 0xFF00FF
#define CYAN    0x00FFFF
#define WHITE   0xFFFFFF


AtomMotion                         atomMotion;                                   // An object to manage the ATOM Motion.
unsigned int                       buttonCount     = 0;                          // Tracks the number of times the ATOM button was pressed.
unsigned int                       servo2speed     = 90;                         // 0 = full reverse, 90 = stationary, 180 = full forward.
unsigned int                       servo4speed     = 90;                         // 0 = full reverse, 90 = stationary, 180 = full forward.
unsigned long                      lastLoop        = 0;                          // Tracks the last time the main loop executed.
unsigned long                      lastPrintLoop   = 0;                          // Tracks the last time the print loop executed.
unsigned long                      ledColor        = 0xFFFFFF;                   // Tracks the color of the onboard LED.
const unsigned int                 NUM_SENSORS     = 4;                          // The number of sensors.
const unsigned int                 SERVO_MIN       = 500;                        // The minimum pulse width for the servos.
const unsigned int                 SERVO_MAX       = 2500;                       // The maximum pulse width for the servos.
const unsigned int                 DEAD_BAND       = 200;                        // The minimum delta before a servo will move.
const unsigned long                debounce        = 80;                         // How many milliseconds to wait before considering a button to have been pressed twice.
const unsigned long                loopDelay       = 10;                         // The maximum value of 4,294,967,295 allows for a delay of about 49.7 days.
const unsigned long                printLoopDelay  = 1000;                       // The minimum time between serial printing of the lux values.
const byte                         sdaGPIO         = 26;                         // Use this to set the SDA GPIO if your board uses a non-standard GPIOs for the I2C bus.
const byte                         sclGPIO         = 32;                         // Use this to set the SCL GPIO if your board uses a non-standard GPIOs for the I2C bus.
const int                          PCA_ADDRESS     = 0x70;                       // The I2C address of the Pa.HUB.
std::array<M5_DLight, NUM_SENSORS> sensorArray     = {};                         // An array of DLIGHT sensor objects.
std::array<uint16_t, NUM_SENSORS>  sensorAddresses = { 0, 1, 4, 5 };             // An array of the Pa.HUB ports with DLIGHT sensors attached.
std::array<uint16_t, NUM_SENSORS>  luxValues       = { 2112, 2112, 2112, 2112 }; // An array of light readings: top left, top right, bottom left, bottom right.

void channelSelect( uint8_t i );

void setup();

void loop();

#endif //ATOMMOTIONDEMO_ATOMMOTIONDEMO_H
