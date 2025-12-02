#include "CH9120.h"


char DataChar[508]; //used to send data
String DataString; // string to receive and send data
unsigned int dataLength = 508; // max len of a data packet

void setup()
{
   CH9120_init(); //starts ethernet connection. set connection details at the top CH9120.cpp
   // the connection mode needs to be set in 2 places in the
   // 3rd line of CH9120.cpp and line 40 of CH9120.h (the choices are listed on line 39)
   Serial.println( "READY" ); // let me know init is finished. connection still takes a few sec to start.
   // you can ping the IP of the device to know when it is ready
}


void loop()
{
   while( Serial.available() ) // only executes if data is available at serial port
   {
      DataString = Serial.readString(); // get data from serial port
      DataString.toCharArray( DataChar, dataLength ); // convert to an array of Chars
      Serial.println( "Sending" ); // print to know that while loop executed
      UART_ID1.write( DataChar ); // send data
   }

   while( UART_ID1.available() ) // only executes if data is available at ethernet port
   {
      DataString = UART_ID1.readString(); // read data from ethernet port
      if( DataString == "Ping" )
      {
         // send an Ack if the string Ping is sent
         UART_ID1.write( "Ack" );
      }
      Serial.println( DataString ); // print out data received
   }
}
