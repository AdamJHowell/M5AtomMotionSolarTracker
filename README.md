# M5AtomMotionSolarTracker

A program which uses the ATOM Motion to track the sun (or other light sources).

The solar tracker will be constructed using Lego bricks where possible.

Requires:

* [M5Stack board definition file](https://m5stack.oss-cn-shenzhen.aliyuncs.com/resource/arduino/package_m5stack_index.json)
* [ATOM Library](https://github.com/m5stack/M5Atom/)
* [M5Stack Library](https://github.com/m5stack/M5Stack/)
* [M5-DLight](https://github.com/m5stack/M5-DLight/)
* [FastLED library](https://github.com/FastLED/FastLED/)

M5Stack ATOM Lite notes:
* ATOM Lite HY2.0-4P:
* G, 5V, G26, G32
* PORT.A (onboard), 32 White/SCL, 26 Yellow/SDA
* Default I2C:
* SCL 21
* SDA 25
* Seems to use this button library: https://docs.m5stack.com/en/api/atom/button

It may be required to call M5.begin() before atomMotion.Init()

[M5Stack ATOM Motion](http://docs.m5stack.com/en/atom/atom_motion) notes:
* https://github.com/m5stack/M5Atom/tree/master/examples/ATOM_BASE/ATOM_Motion
* PORT.B (black connector), 23 white, 33 yellow
* PORT.C (blue connector), 19 yellow, 22 white
* Servo angle range 0 ~ 180
* DC motor speed range -127~127
* APDS-9960 default address: 0x39
* ATOM Motion servo:
* Has four servo ports, numbered from 1 to 4.
* uint8_t SetServoPulse( uint8_t Servo_CH, uint16_t width );
* Seems to return 0 on success, and returns 1 on error.
* uint8_t SetServoAngle( uint8_t Servo_CH, uint8_t angle );
* Seems to return 0 on success, and returns 1 on error.
* Servo(1~4)   angle: 0-180   pulse: 500-2500   R/W

[M5Stack PaHUB2](https://docs.m5stack.com/en/unit/pahub2) notes:
* https://github.com/m5stack/M5Stack/tree/master/examples/Unit/PaHUB_TCA9548A
* This project the PaHUB2 connected to PORT.A (the onboard HY2.0 port of the ATOM Lite).

[M5Stack Dlight sensor](https://docs.m5stack.com/en/unit/dlight) notes:
* I2C address 0x23
* https://github.com/m5stack/M5-DLight
* Values from 1-65535, greater values represent brighter light.
* This project uses 4 M5Stack DLight sensors connected to the PaHUB2.

[M5Stack Unit Limit sensor](https://docs.m5stack.com/en/unit/Unit%20Limit)
