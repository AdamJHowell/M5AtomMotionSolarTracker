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
ATOM Motion servo:
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
   int altitudeSpeed = 1500;				  // Holds the current speed of the altitude servo.  Ranges from 500 to 2500.  The default of 1500 is motionless.
   int azimuthSpeed = 1500;				  // Holds the current speed of the azimuth servo.  Ranges from 500 to 2500.  The default of 1500 is motionless.

   // M5.update() seems to only call M5.Btn.read();
   M5.update();

   // Read the light sensors.
   for( uint8_t i = 0; i < NUM_SENSORS; i++ )
   {
      channelSelect( sensorAddresses[i] );
      luxValues[i] = sensorArray[i].getLUX();
   }
   // Sum the top sensors.
   uint16_t topRow = luxValues[0] + luxValues[1];
   // Sum the bottom sensors.
   uint16_t bottomRow = luxValues[2] + luxValues[3];
   // Sum the left sensors.
   uint16_t leftSide = luxValues[0] + luxValues[2];
   // Sum the right sensors.
   uint16_t rightSide = luxValues[1] + luxValues[3];

   altitudeSpeed = map( topRow - bottomRow, -65535, 65535, SERVO_MIN, SERVO_MAX );
   azimuthSpeed = map( leftSide - rightSide, -65535, 65535, SERVO_MIN, SERVO_MAX );
   // ToDo: If the up stop is not tripped:
   //   If the top row is brighter than the bottom row, move up.
   if( digitalRead( PORT_B ) )
   {
      servo4speed = buttonCount * 10;
      altitudeSpeed = 1500;
   }
   else
   {
      servo4speed = 90;
      ledColor = RED;
      Serial.printf( "Hit limit B!\n" );
   }
   // ToDo: If the down stop is not tripped:
   //   If the bottom row is brighter than the top row, move down.
   if( digitalRead( PORT_C ) )
   {
      servo2speed = 90;
      ledColor = BLUE;
      Serial.printf( "Hit limit C!\n" );
   }
   else
   {
      servo2speed = buttonCount * 10;
   }

   // Only move if the L/R delta is greater than the dead-band setting.
   if( abs( leftSide - rightSide ) > DEAD_BAND )
   {
      // uint8_t SetServoPulse( uint8_t Servo_CH, uint16_t width );
      atomMotion.SetServoAngle( 1, azimuthSpeed );
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
         Serial.printf( "L0:%d L1:%d L4:%d L5:%d servo2speed:%u servo4speed:%u\n", luxValues[0], luxValues[1], luxValues[2], luxValues[3], servo2speed, servo4speed );
         lastPrintLoop = millis();
      }
      lastLoop = millis();
   }
//   atomMotion.SetServoAngle( 2, servo2speed );
//   atomMotion.SetServoAngle( 4, servo4speed );
   M5.dis.drawpix( 0, ledColor );
} // End of loop()
