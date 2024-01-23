/*
ATOM Motion:
    SCL 21
    SDA 25
    PORT.A (onboard), 32 White/SCL, 26 Yellow/SDA
    PORT.B (black connector), 23 white, 33 yellow
    PORT.C (blue connector), 19 yellow, 22 white
Servo angle range 0 ~ 180
DC motor speed range -127~127
ATOM Lite HY2.0-4P:
    G, 5V, G26, G32
APDS-9960 default address: 0x39
Uses 4 M5Stack DLight sensors on I2C address 0x23: https://github.com/m5stack/M5-DLight, https://docs.m5stack.com/en/unit/dlight
Values from 1-65535.
M5Stack Dlight sensor returns greater values for brighter light.
ATOM Motion:
   https://docs.m5stack.com/en/atom/atom_motion
   https://github.com/m5stack/M5Atom/tree/master/examples/ATOM_BASE/ATOM_Motion
ATOM Motion servo:
   https://github.com/m5stack/M5Atom/blob/master/examples/ATOM_BASE/ATOM_Motion/AtomMotion.h
   uint8_t SetServoPulse( uint8_t Servo_CH, uint16_t width );
   Servo(1~4)   angle: 0-180   pulse: 500-2500   R/W
*/

/*
The M5 ATOM seems to use this button library: https://docs.m5stack.com/en/api/atom/button
*/

#include "M5AtomMotionSolarTracker.h"


void channelSelect( uint8_t i )
{
   if( i > 7 )
      return;
   Wire.beginTransmission( PCA_ADDRESS );
   Wire.write( 1 << i );
   Wire.endTransmission();
} // End of channelSelect()


void setup()
{
   // SerialEnable, I2CEnable, DisplayEnable
   M5.begin( true, false, true );
   Serial.println( "\nBeginning setup()." );
   M5.dis.drawpix( 0, VIOLET );
   // Wire.begin() must happen before atomMotion.Init().
   Wire.begin( sdaGPIO, sclGPIO );
   // Wire.begin() must happen before atomMotion.Init().
   atomMotion.Init();

   pinMode( PORT_B, INPUT_PULLUP );
   pinMode( PORT_C, INPUT_PULLUP );

   delay( 500 );
   M5.dis.drawpix( 0, WHITE );
   for( uint8_t i = 0; i < NUM_SENSORS; i++ )
   {
      channelSelect( sensorAddresses[i] );
      sensorArray[i].begin();
      sensorArray[i].setMode( CONTINUOUSLY_H_RESOLUTION_MODE );
   }
   delay( 500 );
   M5.dis.drawpix( 0, GREEN );
   Serial.println( "\nFinished setup().\n" );
} // End of setup()


void loop()
{
   long altitudeSpeed = 1500;				  // Holds the current speed of the altitude servo.  Ranges from 500 to 2500.  The default of 1500 is motionless.
   long azimuthSpeed = 1500;				  // Holds the current speed of the azimuth servo.  Ranges from 500 to 2500.  The default of 1500 is motionless.

   // M5.update() seems to only call M5.Btn.read();
   M5.update();

   // Read the light sensors.
   for( uint8_t i = 0; i < NUM_SENSORS; i++ )
   {
      channelSelect( sensorAddresses[i] );
      luxValues[i] = sensorArray[i].getLUX();
   }
   // Sum the top sensors.
   long topRow = luxValues[0] + luxValues[1];
   // Sum the bottom sensors.
   long bottomRow = luxValues[2] + luxValues[3];
   // Sum the left sensors.
   long leftSide = luxValues[0] + luxValues[2];
   // Sum the right sensors.
   long rightSide = luxValues[1] + luxValues[3];

   long rowDelta = topRow - bottomRow;
   long sideDelta = leftSide - rightSide;
   long rowValue = constrain( rowDelta, -3000, 3000 );
   long sideValue = constrain( sideDelta, -3000, 3000 );
   // map( value, fromLow, fromHigh, toLow, toHigh );
   altitudeSpeed = map( rowValue, -3000, 3000, SERVO_MIN, SERVO_MAX );
   azimuthSpeed = map( sideValue, -3000, 3000, SERVO_MIN, SERVO_MAX );

   // If the up stop is tripped, stop the servo.
   //   If the top row is brighter than the bottom row, move up.
   if( !digitalRead( PORT_B ) )
   {
      altitudeSpeed = 1500;
      ledColor = GREEN;
      Serial.printf( "Hit limit B!\n" );
   }
   // If the down stop is tripped:
   //   If the bottom row is brighter than the top row, move down.
   if( !digitalRead( PORT_C ) )
   {
      altitudeSpeed = 1500;
      ledColor = BLUE;
      Serial.printf( "Hit limit C!\n" );
   }

   // Only move if the up/down delta is greater than the dead-band setting.
   if( abs( topRow - bottomRow ) > DEAD_BAND )
   {
      if( topRow > bottomRow )
         Serial.printf( "Moving altitude servo up.\n" );
      else
         Serial.printf( "Moving altitude servo down.\n" );
      // uint8_t SetServoPulse( uint8_t Servo_CH, uint16_t width );
      atomMotion.SetServoPulse( altitudeServo, altitudeSpeed );
   }

   // Only move if the L/R delta is greater than the dead-band setting.
   if( abs( leftSide - rightSide ) > DEAD_BAND )
   {
      if( leftSide > rightSide )
         Serial.printf( "Moving azimuth servo left.\n" );
      else
         Serial.printf( "Moving azimuth servo right.\n" );
      // uint8_t SetServoPulse( uint8_t Servo_CH, uint16_t width );
      atomMotion.SetServoPulse( azimuthServo, azimuthSpeed );
   }

   if( ( lastLoop == 0 ) || ( millis() - lastLoop ) > loopDelay )
   {
      if( M5.Btn.lastChange() > lastLoop )
      {
         buttonCount++;
         Serial.printf( "  Button press!\n" );
         if( buttonCount > 9 )
            buttonCount = 0;
         switch( buttonCount )
         {
            case 0:
               ledColor = RED;
               break;
            case 1:
               ledColor = ORANGE;
               break;
            case 2:
               ledColor = YELLOW;
               break;
            case 3:
               ledColor = GREEN;
               break;
            case 4:
               ledColor = BLUE;
               break;
            case 5:
               ledColor = INDIGO;
               break;
            case 6:
               ledColor = VIOLET;
               break;
            case 7:
               ledColor = MAGENTA;
               break;
            case 8:
               ledColor = CYAN;
               break;
            case 9:
               ledColor = BLACK;
               break;
            default:
               break;
         }
      }

      // Print values in a format the Arduino Serial Plotter can use.
      if(( lastPrintLoop == 0 ) || ( millis() - lastPrintLoop ) > printLoopDelay )
      {
         Serial.printf( "%d - %d = %d\n", luxValues[0], luxValues[1], topRow );
         Serial.printf( "%d - %d = %d\n", luxValues[2], luxValues[3], bottomRow );
         Serial.printf( "| - |\n" );
         Serial.printf( "%d - %d\n", leftSide, rightSide );
         Serial.printf( "azimuthSpeed:%u altitudeSpeed:%u\n", azimuthSpeed, altitudeSpeed );
         Serial.printf( "top - bottom: %d\n", topRow - bottomRow );
         Serial.printf( "left - right: %d\n\n", leftSide - rightSide );
         lastPrintLoop = millis();
      }
      lastLoop = millis();
   }
   M5.dis.drawpix( 0, ledColor );
} // End of loop()
