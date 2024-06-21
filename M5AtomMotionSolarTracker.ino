/*
M5Stack ATOM Lite:
   ATOM Lite HY2.0-4P:
      G, 5V, G26, G32
      PORT.A (onboard), 32 White/SCL, 26 Yellow/SDA
   Default I2C:
      SCL 21
      SDA 25
      Seems to use this button library: https://docs.m5stack.com/en/api/atom/button

M5Stack ATOM Motion (http://docs.m5stack.com/en/atom/atom_motion):
   https://github.com/m5stack/M5Atom/tree/master/examples/ATOM_BASE/ATOM_Motion
   PORT.B (black connector), 23 white, 33 yellow
   PORT.C (blue connector), 19 yellow, 22 white
   Servo angle range 0 ~ 180
   DC motor speed range -127~127
   APDS-9960 default address: 0x39
   ATOM Motion servo:
      Has four servo ports, numbered from 1 to 4.
      uint8_t SetServoPulse( uint8_t Servo_CH, uint16_t width );
         Seems to return 0 on success, and returns 1 on error.
      uint8_t SetServoAngle( uint8_t Servo_CH, uint8_t angle );
         Seems to return 0 on success, and returns 1 on error.
      Servo(1~4)   angle: 0-180   pulse: 500-2500   R/W

M5Stack PaHUB2 (https://docs.m5stack.com/en/unit/pahub2):
   https://github.com/m5stack/M5Stack/tree/master/examples/Unit/PaHUB_TCA9548A
   This project the PaHUB2 connected to PORT.A (the onboard HY2.0 port of the ATOM Lite).

M5Stack Dlight sensor (https://docs.m5stack.com/en/unit/dlight):
   I2C address 0x23
   https://github.com/m5stack/M5-DLight
   Values from 1-65535, greater values represent brighter light.
   This project uses 4 M5Stack DLight sensors connected to the PaHUB2.
*/


#include "M5AtomMotionSolarTracker.h"


/*
 * The channelSelect function changes the current active channel of the PaHUB I2C multiplexer.
 */
void channelSelect( uint8_t i )
{
   if( i > 7 )
      return;
   Wire.beginTransmission( PCA_ADDRESS );
   Wire.write( 1 << i );
   Wire.endTransmission();
} // End of channelSelect()


/*
 * pulseWidth is a global that is updated in loop().
 * pvParameters is not used.
 */
[[noreturn]] void TaskMotion( __attribute__((unused)) void *pvParameters )
{
   while( true )
   {
      M5.dis.drawpix( 0, ledColor );
      atomMotion.SetServoPulse( AZIMUTH_SERVO, azimuthSpeed );
      atomMotion.SetServoPulse( ALTITUDE_SERVO, altitudeSpeed );
      // Give other threads a chance to take control of the core.
      vTaskDelay( 0 );
   }
} // End of TaskMotion()


void setup()
{
   // Wire.begin() must happen before atomMotion.Init().
   Wire.begin( SDA_GPIO, SCL_GPIO );
   // SerialEnable, I2CEnable, DisplayEnable
   M5.begin( true, false, true );
   Serial.println( "\nBeginning setup()." );
   atomMotion.Init();

   // Establish the two ports as inputs.
   pinMode( PORT_B, INPUT_PULLUP );
   pinMode( PORT_C, INPUT_PULLUP );

   // Pin the TaskMotion() function to core 0.
   xTaskCreatePinnedToCore(
         TaskMotion,  // Pointer to the task entry function.
         "TaskMotion",   // A descriptive name for the task.
         4096,       // The size of the task stack specified as the number of bytes.
         nullptr,    // Pointer that will be used as the parameter for the task being created.
         2,             // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
         nullptr,   // Used to pass back a handle by which the created task can be referenced.
         0 );            // Values 0 or 1 indicate the CPU core which the task will be pinned to.

   // Turn on the LED and set it to white.
   M5.dis.drawpix( 0, WHITE );

   // Configure every M5Stack DLIGHT sensor.
   for( uint8_t i = 0; i < NUM_SENSORS; i++ )
   {
      channelSelect( sensorAddresses[i] );
      sensorArray[i].begin();
      sensorArray[i].setMode( CONTINUOUSLY_H_RESOLUTION_MODE );
   }
   Serial.println( "\nFinished setup().\n" );
} // End of setup()


void loop()
{
   // M5.update() seems to only call M5.Btn.read(), which reads the state of the in-built button.
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

   // Calculate the difference between the rows and sides.
   long rowDelta = topRow - bottomRow;
   long sideDelta = leftSide - rightSide;

   // Constrain the rows and sides, because delta greater than 3k is enough that we should move full speed.
   long rowValue = constrain( rowDelta, -3000, 3000 );
   long sideValue = constrain( sideDelta, -3000, 3000 );
   // map( value, fromLow, fromHigh, toLow, toHigh );
   long tempAlt = map( rowValue, -3000, 3000, SERVO_MIN, SERVO_MAX );
   long tempAz = map( sideValue, -3000, 3000, SERVO_MIN, SERVO_MAX );
   altitudeSpeed = tempAlt;
   azimuthSpeed = tempAz;

   // If the up stop is tripped, prevent the servo from moving upward.
   if( !digitalRead( PORT_B ))
   {
      // ToDo: Determine if this should be 500-1500 instead.
      altitudeSpeed = constrain( altitudeSpeed, 1500, 2500 );
      ledColor = GREEN;
      Serial.printf( "-- Hit limit B up stop!\n" );
   }

   // If the down stop is tripped, prevent the servo from moving downward.
   if( !digitalRead( PORT_C ))
   {
      // ToDo: Determine if this should be 1500-2500 instead.
      altitudeSpeed = constrain( altitudeSpeed, 500, 1500 );
      ledColor = BLUE;
      Serial.printf( "-- Hit limit C down stop!\n" );
   }

   // Don't move up or down if the up/down delta is less than the dead-band setting.
   if( abs( rowDelta ) <= DEAD_BAND )
      altitudeSpeed = 1500;

   // Don't move left or right if the L/R delta is less than the dead-band setting.
   if( abs( sideDelta ) <= DEAD_BAND )
      azimuthSpeed = 1500;

   if(( lastLoop == 0 ) || ( millis() - lastLoop ) > LOOP_DELAY )
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
      lastLoop = millis();
   }

   // Print values.
   if(( lastPrintLoop == 0 ) || ( millis() - lastPrintLoop ) > PRINT_LOOP_DELAY )
   {
      Serial.println( "Readings:" );
      Serial.printf( "%5hu + %5hu = %ld\n", luxValues[0], luxValues[1], topRow );
      Serial.printf( "%5hu + %5hu = %ld\n", luxValues[2], luxValues[3], bottomRow );
      Serial.printf( "-----   -----\n" );
      Serial.printf( "%5ld   %5ld\n", leftSide, rightSide );
      Serial.println( "" );
      Serial.printf( "azimuthSpeed:  %5hu\n", azimuthSpeed );
      Serial.printf( "altitudeSpeed: %5hu\n", altitudeSpeed );
      Serial.println( "" );
      Serial.printf( "top - bottom: %5ld\n", rowDelta );
      Serial.printf( "left - right: %5ld\n", sideDelta );
      if( abs( rowDelta ) > DEAD_BAND )
      {
         if( topRow > bottomRow )
            Serial.printf( "Moving altitude servo up.\n" );
         else
            Serial.printf( "Moving altitude servo down.\n" );
      }
      if( abs( sideDelta ) > DEAD_BAND )
      {
         if( leftSide > rightSide )
            Serial.printf( "Moving azimuth servo left.\n" );
         else
            Serial.printf( "Moving azimuth servo right.\n" );
      }
      Serial.println( "" );

      // ToDo: Delete this when done testing.
      // map( value, fromLow, fromHigh, toLow, toHigh );
      long angle = map( sideValue, -3000, 3000, 0, 180 );
      Serial.println( "" );
      Serial.printf( "sideValue: %ld\n", sideValue );
      Serial.printf( "angle: %ld\n", angle );
      Serial.println( "" );
      Serial.println( "---------------------" );
      Serial.println( "" );

      lastPrintLoop = millis();
   }
} // End of loop()
