//
// Created by adam.howell on 2023-12-28.
//

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


AtomMotion atomMotion;                                                          // An object to manage the ATOM Motion.
xSemaphoreHandle CtlSemaphore;                                            // A semaphore the ATOM Motion uses to control motors and/or servos.
bool direction = true;                                                    // Tracks the direction of each servo.
int speed = 180;                                                          // Stores the speed of the continuous rotation servos.
unsigned int buttonCount = 0;                                             // Tracks the number of times the ATOM button was pressed.
unsigned long lastLoop = 0;                                               // Tracks the last time the main loop executed.
const unsigned int NUM_SENSORS = 4;                                       // The number of sensors.
const unsigned long loopDelay = 10;                                       // The maximum value of 4,294,967,295 allows for a delay of about 49.7 days.
const byte sdaGPIO = 26;                                                  // Use this to set the SDA GPIO if your board uses a non-standard GPIOs for the I2C bus.
const byte sclGPIO = 32;                                                  // Use this to set the SCL GPIO if your board uses a non-standard GPIOs for the I2C bus.
const int PCA_ADDRESS = 0x70;                                             // The I2C address of the Pa.HUB.
std::array<M5_DLight, NUM_SENSORS> sensorArray = {};                      // An array of DLIGHT sensor objects.
std::array<uint16_t, NUM_SENSORS> sensorAddresses = { 0, 1, 4, 5 };       // An array of the Pa.HUB ports with DLIGHT sensors attached.
std::array<uint16_t, NUM_SENSORS> luxValues = { 2112, 2112, 2112, 2112 }; // An array of light readings, one per sensor.

void pcaSelect( uint8_t i );
void GetStatus();
void TaskMotion( void *pvParameters );
void setup();
void loop();

#endif //ATOMMOTIONDEMO_ATOMMOTIONDEMO_H
