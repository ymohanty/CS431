#include <Arduino.h>
#include <MeMegaPi.h>

#include <SoftwareSerial.h>
#include <Wire.h>


int rx2 = 14;
int tx2 = 15;
SoftwareSerial toPi( rx2, tx2 );

void setup() {
  // put your setup code here, to run once:
  Serial.begin( 9600 );
  Serial.print( "BEGIN TESTING" );

  Serial1.begin( 9600 );
  Serial1.print( "TEST FROM SERIAL1");
  
  Serial2.begin( 9600 );
  Serial2.print( "BEGIN TESTING SERIAL2" );


  Serial3.begin( 9600 );
  Serial3.print( "BEGIN TESTING SERIAL3" );


 
  
  toPi.begin( 9600 );
  toPi.print("HELLO");

}

void loop() {
  // put your main code here, to run repeatedly:
  //Serial.print( "MAIN LOOP" );
  toPi.print("HERE");
  delay( 1000 );


  if(Serial2.available()>0) {
   Serial.print( (Serial2.readString()));
   Serial.print( "\n" );
  }

 /*
  byte  msg = Serial2.read();
  Serial.print(msg);

  msg = Serial1.read();
  Serial1.print(msg);


  msg = Serial2.read();
  Serial.print(msg);

  msg = Serial3.read();
  Serial.print(msg);
*/




}
