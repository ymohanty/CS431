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



MeUltrasonicSensor ultraSensor( PORT_8 );

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
  Encoder_1.setMotorPwm( moveSpeed );
  Encoder_2.setMotorPwm( moveSpeed );
}

void TurnInplaceLeft(void)
{
  Encoder_1.setMotorPwm( -moveSpeed );
  Encoder_2.setMotorPwm( -moveSpeed );
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

void ArmReset( void ){
  Encoder_3.setMotorPwm( 0 );
  Encoder_3.move( startPos, moveSpeed );
  
}

void ArmZero( void ){
  
  Encoder_3.setMotorPwm( -moveSpeed );
  delay( 1000 );
  
  //Encoder_3.setMotorPwm(0);

  Encoder_3.updateCurPos();
  long pos0 = Encoder_3.getCurPos();
  long pos1;
  int time = 0;
  while( true ){ //watch the arm speed until it stops on the top

    if( time%10000 == 0){
      Encoder_3.updateCurPos();
      pos1 = pos0;
      pos0 = Encoder_3.getCurrentSpeed(); 
    }
     
    if( (pos0 - pos1)<1 ){ //arm stops moving
        Encoder_3.setMotorPwm( 0 );
        break;     
    }

   // Serial.print(" WHILE LOOP \n");
   // Serial.print( (pos1 - pos0 ) ); 
  }


  long travel = 1000;
  Encoder_3.moveTo( pos1-travel, -moveSpeed);
 
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
  //ArmReset2();
   
}

void ArmReset2( void ){ //this is the working reset
  Encoder_3.setMotorPwm( 0 );
  Encoder_3.updateCurPos();
  long curPos = Encoder_3.getCurPos();

  Encoder_3.setMotorPwm( moveSpeed );


  while( curPos < startPos){
    Encoder_3.updateCurPos();
    curPos =  Encoder_3.getCurPos();
  }
  Encoder_3.setMotorPwm( 0 );
  
}

  
    
 
  
  
  
  





void setup() {
  // put your setup code here, to run once:
  Serial.begin( 115200 );
  Serial3.begin(115200);
  delay(100);
  attachInterrupt(Encoder_1.getIntNum(), isr_process_encoder1, RISING);
  attachInterrupt(Encoder_2.getIntNum(), isr_process_encoder2, RISING);
  attachInterrupt(Encoder_3.getIntNum(), isr_process_encoder3, RISING);
  count = 0;

  ArmZero2();
  //Serial.print("\nARM ZEROED\n");
  
  
  ArmReset2();
  
  //Serial.print( startPos );
  //Encoder_3.updateCurPos();
  //startPos = Encoder_3.getCurPos();
  
}


void loop() {
  // put your main code here, to run repeatedly:
  delay(500);
  if (Serial3.available() > 0) {   
     command = Serial3.readString();
     Serial.print( command );
  }

  float dist = ultraSensor.distanceCm();


  //if( steps%10000 == 0){
    // Serial.print("Distance: ");
    // Serial.print( dist );
    // Serial.print(" cm \n" );
    //Serial.print( (startPos - curPos)  );
    //Serial.print( "\n" );
  
  //}
  
  Encoder_3.updateCurPos();
  curPos = Encoder_3.getCurPos();


  if (command == "w" && dist > 20.0) {
      if(dist < 40.0) ForwardSlow();
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
  else if(command == "t" && startPos - curPos <= 0){
        ArmUp();
  }
  else if(command == "g" &&  (startPos - curPos >= 975)){
        ArmDown();
  }
  else if( command = "q"){
      Stop();
    //Serial.print("Stopping");
  }
 

  steps++;
  //else Serial.print(command);

  

  /*
  if (steps%200000 < 50000) Forward();
  else if( steps%200000 == 50000){
     Stop();
     //Serial.print( steps );
  }
  else if(steps%200000 > 50000 && steps%200000 < 100000)
  {
    //Serial.print("TURNING RIGHT IN PLACE");
    TurnInplaceRight(); 
  }
	else if( steps%200000 == 100000) Stop();
  else if(steps%200000 > 100000 && steps%200000 < 150000)
  {
    //Serial.print("TURNING LEFT IN PLACE");
    TurnInplaceLeft(); 
  }
  else if( steps%200000 == 150000) Stop();
  else if(steps%200000 > 150000 && steps%200000 < 199999) TurnLeft();
  else Stop();
 
  */
  
}
