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
SHT40 default address: 0x44
APDS-9960 default address: 0x39
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
   // Wire.begin() must happen before atomMotion.Init().
   Wire.begin( sdaGPIO, sclGPIO );
   // Wire.begin() must happen before atomMotion.Init().
   atomMotion.Init();

   delay( 500 );
   Serial.println( "\nBeginning setup()." );

   pinMode( PORT_B, INPUT_PULLUP );
   pinMode( PORT_C, INPUT_PULLUP );
   M5.dis.drawpix( 0, WHITE );

   delay( 500 );
   for( uint8_t i = 0; i < NUM_SENSORS; i++ )
   {
      channelSelect( sensorAddresses[i] );
      sensorArray[i].begin();
      sensorArray[i].setMode( CONTINUOUSLY_H_RESOLUTION_MODE );
   }
   delay( 500 );
   Serial.println( "\nFinished setup().\n" );
} // End of setup()


void loop()
{
   M5.update();

   if(( lastLoop == 0 ) || ( millis() - lastLoop ) > loopDelay )
   {
      long ifLoop = millis();
      if( M5.Btn.wasPressed())
      {
         if( speed == 100 )
         {
            speed = 0;
         }
         else
         {
            speed = 100;
         }
         Serial.printf( "New speed: %d\n", speed );
         switch( buttonCount )
         {
            case 0:
               M5.dis.drawpix( 0, RED );
               break;
            case 1:
               M5.dis.drawpix( 0, ORANGE );
               break;
            case 2:
               M5.dis.drawpix( 0, YELLOW );
               break;
            case 3:
               M5.dis.drawpix( 0, GREEN );
               break;
            case 4:
               M5.dis.drawpix( 0, BLUE );
               break;
            case 5:
               M5.dis.drawpix( 0, INDIGO );
               break;
            case 6:
               M5.dis.drawpix( 0, VIOLET );
               break;
            default:
               break;
         }
         buttonCount++;
         if( buttonCount >= 7 )
         {
            buttonCount = 0;
         }
      }
      if( !digitalRead( PORT_B ))
      {
         atomMotion.SetServoAngle( 4, 90 );
         Serial.printf( "Hit limit B!\n" );
      }
      else
      {
         atomMotion.SetServoAngle( 4, speed );
      }
      if( !digitalRead( PORT_C ))
      {
         atomMotion.SetServoAngle( 2, 90 );
         Serial.printf( "Hit limit C!\n" );
      }
      else
      {
         atomMotion.SetServoAngle( 2, speed );
      }

      // Read all sensors before acting on the values.
      for( uint8_t i = 0; i < NUM_SENSORS; i++ )
      {
         channelSelect( sensorAddresses[i] );
         luxValues[i] = sensorArray[i].getLUX();
      }
      // Print values in a format the Arduino Serial Plotter can use.
      if(( lastPrintLoop == 0 ) || ( millis() - lastPrintLoop ) > printLoopDelay )
      {
         Serial.printf( "L0:%d L1:%d L4:%d L5:%d\n", luxValues[0], luxValues[1], luxValues[2], luxValues[3] );
         lastPrintLoop = millis();
      }
      lastLoop = millis();
   }
} // End of loop()
