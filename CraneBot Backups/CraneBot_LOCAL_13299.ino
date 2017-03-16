/**
 * MegaPi uses EncoderOnBorader to rotate the motors
 */

#include <Arduino.h>
#include <MeMegaPi.h>

#include <SoftwareSerial.h>
#include <Wire.h>

int16_t moveSpeed = 100;
long steps = 0;

//used to keep track of the arm position
long startPos; //bottom of range
long curPos;   //current pos
long endPos; //top of range


MeEncoderOnBoard Encoder_1(SLOT1);
MeEncoderOnBoard Encoder_2(SLOT2);


MeEncoderOnBoard Encoder_3(SLOT3);
MeEncoderOnBoard Encoder_4(SLOT4); //encoder for claw motor

MeUltrasonicSensor ultraSensor( PORT_8 );

int rx2 = 17;
int tx2 = 16;
SoftwareSerial toPi( rx2, tx2 );



String command;
float dist;
int count;



void isr_process_encoder1(void) {
  if(digitalRead(Encoder_1.getPortB()) == 0) {
    Encoder_1.pulsePosMinus();
  }
  else {
    Encoder_1.pulsePosPlus();
  }
}

void isr_process_encoder2(void) {
  if(digitalRead(Encoder_2.getPortB()) == 0) {
    Encoder_2.pulsePosMinus();
  }
  else {
    Encoder_2.pulsePosPlus();
  }
}

void isr_process_encoder3(void){
  if(digitalRead(Encoder_3.getPortB()) == 0) {
    Encoder_3.pulsePosMinus(); 
  }
  else Encoder_3.pulsePosPlus();
}

void isr_process_encoder4(void){
  if(digitalRead(Encoder_4.getPortB()) == 0) {
    Encoder_4.pulsePosMinus(); 
  }
  else Encoder_4.pulsePosPlus();
}



void Forward(void)
{
  Encoder_1.setMotorPwm(-moveSpeed);
  Encoder_2.setMotorPwm(moveSpeed);
}

void ForwardSlow(void)
{
  Encoder_1.setMotorPwm(-moveSpeed/2);
  Encoder_2.setMotorPwm(moveSpeed/2);
}

void ForwardFast(void){
 Encoder_1.setMotorPwm( -255 );
 Encoder_2.setMotorPwm( 255 ); 
}

void Backward(void)
{
  Encoder_1.setMotorPwm(moveSpeed);
  Encoder_2.setMotorPwm(-moveSpeed);
}

void BackwardAndTurnLeft(void)
{
  Encoder_1.setMotorPwm(moveSpeed/4);
  Encoder_2.setMotorPwm(-moveSpeed);
}

void BackwardAndTurnRight(void)
{
  Encoder_1.setMotorPwm(moveSpeed);
  Encoder_2.setMotorPwm(-moveSpeed/4);
}

void TurnLeft(void)
{
  Encoder_1.setMotorPwm(-moveSpeed);
  Encoder_2.setMotorPwm(moveSpeed/2);
}

void TurnRight(void)
{
  Encoder_1.setMotorPwm(-moveSpeed/2);
  Encoder_2.setMotorPwm(moveSpeed);
}

void TurnInplaceRight(void)
{
  Encoder_1.setMotorPwm( -moveSpeed*2 );
  Encoder_2.setMotorPwm( -moveSpeed*2 );
}

void TurnInplaceLeft(void)
{
  Encoder_1.setMotorPwm( moveSpeed*2 );
  Encoder_2.setMotorPwm( moveSpeed*2 );
}



void Stop(void)
{
  Encoder_1.setMotorPwm(0);
  Encoder_2.setMotorPwm(0);
}



void ArmUp( void ){
  Encoder_3.setMotorPwm( -moveSpeed );
  
  
}

void ArmDown( void ){
  Encoder_3.setMotorPwm( moveSpeed );

}


void ArmStop( void ){
  Encoder_3.setMotorPwm( 0 ); 
}



void ArmZero2( void ){ //this is the working zero method

  Encoder_3.setMotorPwm( -moveSpeed );
  delay( 1000 );

  Encoder_3.updateSpeed();
  while( true ){
    if( Encoder_3.getCurrentSpeed() == 0.0){
     Encoder_3.setMotorPwm(0);
     break;
    }else Encoder_3.updateSpeed(); 
  }

  Encoder_3.updateCurPos();
  long top = Encoder_3.getCurPos();

  //Serial.print("\nTOP:");
  //Serial.print( top );

  endPos = top;
  startPos = top + 1000; //top of crane-1000 degrees is botton of crane arc

}

void ArmReset2( void ){ //this is the working reset
  Encoder_3.setMotorPwm( 0 );
  Encoder_3.updateCurPos();
  long curPos = Encoder_3.getCurPos();

  Encoder_3.setMotorPwm( moveSpeed );


  while( curPos < (startPos-500)){
    Encoder_3.updateCurPos();
    curPos =  Encoder_3.getCurPos();
  }
  Encoder_3.setMotorPwm( 0 );
  
}



void clawOpen( void ){
  Encoder_4.setMotorPwm( moveSpeed );
}

void clawClose( void ){
  Encoder_4.setMotorPwm( -moveSpeed );
}

void clawStop( void ){
    Encoder_4.setMotorPwm( 0 );
}



float scale; //used to compare motor rotation to track distance traveled

void setTrackZero( void ){
  Stop();

  Encoder_1.updateCurPos();
  Encoder_2.updateCurPos();
  long startPos1 = Encoder_1.getCurPos();
  long startPos2 = Encoder_2.getCurPos();
  
  float dist_i = ultraSensor.distanceCm();

  ForwardSlow();
  delay( 5000 );
  Stop();
  
  float dist_f = ultraSensor.distanceCm();

  Encoder_1.updateCurPos();
  Encoder_2.updateCurPos();
  long endPos1 = Encoder_1.getCurPos();
  long endPos2 = Encoder_2.getCurPos();
  
  float motorTravel1 = endPos1 - startPos1;
  float motorTravel2 = endPos2 - startPos2;
  float deltaDist = dist_f - dist_i ;


  Serial.print( "TRACK ZERO\n" );
  Serial.print( motorTravel1 );
  Serial.print( "\n" );
  Serial.print( motorTravel2 );
  Serial.print( "\n" );
  Serial.print( deltaDist );
  Serial.print( "\n" );

  scale = ( abs(motorTravel1) + abs(motorTravel2))/2*deltaDist;
  //assign scale, use average of both motors
  
  
 
  
}
  
    
 
  
  
  
  





void setup() {
  // put your setup code here, to run once:
  Serial.begin( 9600 );
  Serial3.begin(115200);
  delay(100);
  attachInterrupt(Encoder_1.getIntNum(), isr_process_encoder1, RISING);
  attachInterrupt(Encoder_2.getIntNum(), isr_process_encoder2, RISING);
  attachInterrupt(Encoder_3.getIntNum(), isr_process_encoder3, RISING);
  attachInterrupt(Encoder_4.getIntNum(), isr_process_encoder4, RISING);
  count = 0;

  ArmZero2();
  //Serial.print("\nARM ZEROED\n");
  
  
  ArmReset2();
  
  
  Serial.print( startPos );
  Encoder_3.updateCurPos();
  curPos = Encoder_3.getCurPos();
  
  //Serial.print( "startPos " );
  //Serial.print( startPos );
  
  
  //Serial.print( "\ncurPos ");
  //Serial.print( curPos );

  //Serial.print( "\nendPos ");
  //Serial.print( endPos );

  setTrackZero();
  
  
  //clawOpen();
  
}


void loop() {
  // put your main code here, to run repeatedly:
  //delay(100);
  //Serial.print( Serial3.available() );
  //Serial.print("Hello");
  //if (Serial3.available() > 0) {   
     
     command = Serial3.readString()[2];
     Serial.print( "\n");
     Serial.print( command );
  //}

  float dist = ultraSensor.distanceCm();

    //Serial.print( "\ndist:");
   //Serial.print( dist );


  
    Encoder_3.updateCurPos();
    curPos = Encoder_3.getCurPos();


  if (command == "w") {
      if(dist < 40.0) Stop();
      else if(dist < 60.0) ForwardSlow();
      else Forward();
    //Serial.print("Going forward");
  }
  else if (command == "s"){
    Backward();
    //Serial.print("Going backwards");
  }
  else if (command == "a"){
    TurnInplaceLeft();
    //Serial.print("Turning left");
  }
  else if (command == "d"){
    TurnInplaceRight();
    //Serial.print("Turning right");
  }
  else if(command == "t"){
    if(curPos  > endPos){
      ArmUp();
  }else ArmStop();
    
  
}
  else if(command == "g"){
      if(curPos  < startPos){
        ArmDown();
      }else {
        ArmStop();  
      }
  }
  else if( command == "q"){
      Stop();
      ArmStop();
    //Serial.print("Stopping");
  }



  
  
}
