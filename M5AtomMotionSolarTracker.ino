/*
ATOM Motion:
    SCL 21
    SDA 25
    PORT.B (black connector), 23 white, 33 yellow
    PORT.C (blue connector), 19 yellow, 22 white
Servo angle range 0 ~ 180
DC motor speed range -127~127
ATOM Lite HY2.0-4P:
    G, 5V, G26, G32
SHT40 default address: 0x44
APDS-9960 default address: 0x39
*/


/**
I2CBusScanner will scan the I2C bus for device addresses in the range of 0x08 to 0x77.
Using the default SDA and SCL GPIOs.
Addresses with a device will be represented by '#'.
Addresses without a device will be represented by '-'.
Addresses which return an error will be represented by 'E'.
    0123456789ABCDEF
0x0         --------
0x1 ----------------
0x2 ----------------
0x3 ---------#------
0x4 ----#---#-------
0x5 ----------------
0x6 ----------------
0x7 --------

3 devices found.
Address: 0x39
Address: 0x44
Address: 0x48

Scan # 12 complete.
Pausing for 5 seconds.
*/


#include "M5AtomMotionSolarTracker.h"


void pcaSelect( uint8_t i )
{
	if( i > 7 )
		return;
	Wire.beginTransmission( PCA_ADDRESS );
	Wire.write( 1 << i );
	Wire.endTransmission();
} // End of pcaSelect()


void GetStatus()
{
	for( int ch = 1; ch < 5; ch++ )
		Serial.printf( "Servo Channel %d: %d \n", ch, atomMotion.ReadServoAngle( ch ) );
	Serial.printf( "Motor Channel %d: %d \n", 1, atomMotion.ReadMotorSpeed( 1 ) );
	Serial.printf( "Motor Channel %d: %d \n", 2, atomMotion.ReadMotorSpeed( 2 ) );
} // End of GetStatus()


void TaskMotion( void *pvParameters )
{
	while( 1 )
	{
		for( int ch = 1; ch < 5; ch++ )
			atomMotion.SetServoAngle( ch, 180 );
		GetStatus();
		vTaskDelay( 1000 / portTICK_RATE_MS );
		for( int ch = 1; ch < 5; ch++ )
			atomMotion.SetServoAngle( ch, 0 );
		GetStatus();
		vTaskDelay( 1000 / portTICK_RATE_MS );
		if( direction )
		{
			atomMotion.SetMotorSpeed( 1, 100 );
			atomMotion.SetMotorSpeed( 2, 100 );
			M5.dis.drawpix( 0, RED );
		}
		else
		{
			atomMotion.SetMotorSpeed( 1, -100 );
			atomMotion.SetMotorSpeed( 2, -100 );
			M5.dis.drawpix( 0, BLUE );
		}
	}
} // End of TaskMotion()


void setup()
{
	M5.begin( true, false, true );
	atomMotion.Init();
	vSemaphoreCreateBinary( CtlSemaphore );
	xTaskCreatePinnedToCore(
		 TaskMotion,	// Pointer to the task entry function.
		 "TaskMotion", // A descriptive name for the task.
		 4096,			// The size of the task stack specified as the number of bytes.
		 NULL,			// Pointer that will be used as the parameter for the task being created.
		 2,				// Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
		 NULL,			// Used to pass back a handle by which the created task can be referenced.
		 0 );				// Values 0 or 1 indicate the index number of the CPU which the task should be pinned to.
	pinMode( PORT_B, INPUT_PULLUP );
	pinMode( PORT_C, INPUT_PULLUP );
	M5.dis.drawpix( 0, WHITE );

	Wire.begin( sdaGPIO, sclGPIO );
	for( uint8_t i = 0; i < NUM_SENSORS; i++ )
	{
		pcaSelect( sensorAddresses[i] );
		sensorArray[i].begin();
		sensorArray[i].setMode( CONTINUOUSLY_H_RESOLUTION_MODE );
	}
	Serial.println( "\nI2C scanner and lux sensor are ready!" );
} // End of setup()


void loop()
{
   M5.update();

	if( millis() - lastLoop >= loopDelay )
	{
		if( M5.Btn.wasPressed() )
		{
			direction = !direction;
			if( speed == 180 )
				speed = 0;
			else
				speed = 180;
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
				buttonCount = 0;
		}
		if( !digitalRead( PORT_B ) )
		{
			atomMotion.SetServoAngle( 4, 90 );
			Serial.printf( "Hit limit B!\n" );
		}
		else
			atomMotion.SetServoAngle( 4, speed );
		if( !digitalRead( PORT_C ) )
		{
			atomMotion.SetServoAngle( 2, 90 );
			Serial.printf( "Hit limit C!\n" );
		}
		else
			atomMotion.SetServoAngle( 2, speed );
		lastLoop = millis();

		// Read all sensors before acting on the values.
		for( uint8_t i = 0; i < NUM_SENSORS; i++ )
		{
			pcaSelect( sensorAddresses[i] );
			luxValues[i] = sensorArray[i].getLUX();
		}
		// Print values in a format the Arduino Serial Plotter can use.
		Serial.printf( "L0:%d L1:%d L4:%d L5:%d\n", luxValues[0], luxValues[1], luxValues[2], luxValues[3] );
	}
} // End of loop()
