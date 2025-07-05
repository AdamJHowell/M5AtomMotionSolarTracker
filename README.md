# M5AtomMotionSolarTracker

A program which uses the ATOM Motion to track the sun (or other light sources).

The solar tracker will be constructed using Lego bricks where possible.

### Hardware:
* A plethora of Lego Technic pieces
* 1 x [M5Stack ATOM Motion](https://docs.m5stack.com/en/atom/atom_motion)
* 1 x [M5Stack PaHUB2](https://docs.m5stack.com/en/unit/pahub2)
* 4 x [M5Stack DLight](https://docs.m5stack.com/en/unit/dlight)
* 2 x [M5Stack Unit Limit switch](https://docs.m5stack.com/en/unit/Unit%20Limit)
* 2 x [Kittenbot Geekservo continuous rotation servos](https://www.aliexpress.us/item/3256802685917966.html)

### Software:
* [M5Stack board definition file](https://m5stack.oss-cn-shenzhen.aliyuncs.com/resource/arduino/package_m5stack_index.json)
* [ATOM library](https://github.com/m5stack/M5Atom/)
* [M5Stack library](https://github.com/m5stack/M5Stack/)
* [M5Stack DLight library](https://github.com/m5stack/M5-DLight/)
* [FastLED library](https://github.com/FastLED/FastLED/)

### Description:
#### Controller:
The M5Stack ATOM is the MCU/processor for this project.

The ATOM Motion bas gives it the ability to control the two servos.
It has an internal PWM controller that uses the default I2C.

#### Sensor:
The PaHUB2 is an I2C multiplexer that I have connected to PORT.A on the MCU.
It connects to the DLight sensors and polls their readings.
Because of the ATOM Motion PWM controller, a second Wire instance is needed for PORT.A.
This is managed using the TwoWire class in the code.

#### Motion:
The two limit switches connect to PORT.B and PORT.C on the ATOM Motion.
These tell the MCU when the altitude servo should stop moving.
One limit switch tells it when to stop moving up, and the other tells when to stop moving down.

---

## Random notes about the components

#### M5Stack ATOM Lite notes:
* ATOM Lite HY2.0-4P:
* * G, 5V, G26, G32
* PORT.A (onboard), 32 White/SCL, 26 Yellow/SDA
* Default I2C:
* * SCL 21
* * SDA 25
* Seems to use this button library: https://github.com/m5stack/M5Atom/blob/master/src/utility/Button.cpp

It may be required to call M5.begin() before atomMotion.Init()

#### [M5Stack ATOM Motion](http://docs.m5stack.com/en/atom/atom_motion) notes:
* [Library](https://github.com/m5stack/M5Atom/tree/master/examples/ATOM_BASE/ATOM_Motion)
* PORT.B (black connector), 23 white, 33 yellow
* PORT.C (blue connector), 19 yellow, 22 white
* Servo angle range 0 ~ 180 (unknown how 360° servos behave, but continuous rotation servos work just fine)
* Servo pulse width range: 500-2500
* DC motor speed range -127~127
* APDS-9960 default address: 0x39
* ATOM Motion servo:
* Has four servo ports, numbered from 1 to 4.
* uint8_t SetServoPulse( uint8_t Servo_CH, uint16_t width );
* Seems to return 0 on success, and returns 1 on error.
* uint8_t SetServoAngle( uint8_t Servo_CH, uint8_t angle );
* Seems to return 0 on success, and returns 1 on error.
* Servo(1~4)   angle: 0-180   pulse: 500-2500   R/W

#### [M5Stack PaHUB2](https://docs.m5stack.com/en/unit/pahub2) notes:
* [Library](https://github.com/m5stack/M5Stack/tree/master/examples/Unit/PaHUB_TCA9548A)
* This project the PaHUB2 connected to PORT.A (the onboard HY2.0 port of the ATOM Lite).

#### [M5Stack Dlight sensor](https://docs.m5stack.com/en/unit/dlight) notes:
* I2C address 0x23
* [Library](https://github.com/m5stack/M5-DLight)
* Values from 1-65535, greater values represent brighter light.
* This project uses 4 M5Stack DLight sensors connected to the PaHUB2.

[M5Stack Unit Limit sensor](https://docs.m5stack.com/en/unit/Unit%20Limit)

[![CodeFactor](https://www.codefactor.io/repository/github/adamjhowell/m5atommotionsolartracker/badge)](https://www.codefactor.io/repository/github/adamjhowell/m5atommotionsolartracker)
