#ifndef M5ATOMMOTIONSOLARTRACKER_M5ATOMMOTIONSOLARTRACKER_H
#define M5ATOMMOTIONSOLARTRACKER_M5ATOMMOTIONSOLARTRACKER_H

#ifndef ESP32
#define ESP32
#endif

#include <M5_DLight.h>
#include "AtomMotion.h"
#include <M5Atom.h>
#include <Arduino.h>
#include <Wire.h>


// #define PaHUB_I2C_ADDRESS 0x70
constexpr int PaHUB_I2C_ADDRESS = 0x70;

// Constants
const int                          SDA_GPIO         = 26;                         // Use this to set the SDA GPIO if your board uses a non-standard GPIOs for the I2C bus.
const int                          SCL_GPIO         = 32;                         // Use this to set the SCL GPIO if your board uses a non-standard GPIOs for the I2C bus.
const int                          PCA_ADDRESS      = 0x70;                       // The I2C address of the Pa.HUB.
const long                         RED              = 0xFF0000;                   // The RGB code for the color red.
const long                         ORANGE           = 0xFF8000;                   // The RGB code for the color orange.
const long                         YELLOW           = 0xFFFF00;                   // The RGB code for the color yellow.
const long                         GREEN            = 0x00FF00;                   // The RGB code for the color green.
const long                         BLUE             = 0x0000FF;                   // The RGB code for the color blue.
const long                         INDIGO           = 0x4B0082;                   // The RGB code for the color indigo.
const long                         VIOLET           = 0xEE82EE;                   // The RGB code for the color violet.
const long                         BLACK            = 0x000000;                   // The RGB code for the color black.
const long                         MAGENTA          = 0xFF00FF;                   // The RGB code for the color magenta.
const long                         CYAN             = 0x00FFFF;                   // The RGB code for the color cyan.
const long                         WHITE            = 0xFFFFFF;                   // The RGB code for the color white.
const unsigned int                 PORT_B           = 23;                         // The address of port B.
const unsigned int                 PORT_C           = 22;                         // The address of port C.
const unsigned int                 AZIMUTH_SERVO    = 3;                          // The ATOM Motion port that the azimuth servo is plugged into.
const unsigned int                 ALTITUDE_SERVO   = 4;                          // The ATOM Motion port that the altitude servo is plugged into.
const unsigned int                 NUM_SENSORS      = 4;                          // The number of sensors.
const unsigned int                 SERVO_MIN        = 500;                        // The minimum pulse width for the servos.
const unsigned int                 SERVO_MAX        = 2500;                       // The maximum pulse width for the servos.
const unsigned int                 DEAD_BAND        = 100;                        // The minimum delta before a servo will move.
const unsigned long                LOOP_DELAY       = 10;                         // The maximum value of 4,294,967,295 allows for a delay of about 49.7 days.
const unsigned long                PRINT_LOOP_DELAY = 1000;                       // The minimum time between serial printing of the lux values.
// Variables
uint16_t                           azimuthSpeed     = 1500;                       // The pulse width of the azimuth servo signal in microseconds.
uint16_t                           altitudeSpeed    = 1500;                       // The pulse width of the altitude servo signal in microseconds.
unsigned int                       buttonCount      = 0;                          // Tracks the number of times the ATOM button was pressed.
unsigned long                      lastLoop         = 0;                          // Tracks the last time the main loop executed.
unsigned long                      lastPrintLoop    = 0;                          // Tracks the last time the print loop executed.
unsigned long                      ledColor         = 0xFFFFFF;                   // Tracks the color of the onboard LED.
uint16_t                     luxValues[NUM_SENSORS] = { 2112, 2112, 2112, 2112 }; // An array of light readings: top left, top right, bottom left, bottom right.
uint8_t                sensorAddresses[NUM_SENSORS] = { 0, 1, 4, 5 };             // An array of the Pa.HUB ports with DLIGHT sensors attached.
// Class instances
AtomMotion                         atomMotion;                                    // An object to manage the ATOM Motion.
M5_DLight                  sensorArray[NUM_SENSORS] = {};                         // An array of DLIGHT sensor objects.


void channelSelect( uint8_t i );

void TaskMotion( void *pvParameters );

#endif //M5ATOMMOTIONSOLARTRACKER_M5ATOMMOTIONSOLARTRACKER_H
