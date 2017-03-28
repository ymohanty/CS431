#include <Arduino.h>
#include <MeMegaPi.h>

#include <SoftwareSerial.h>
#include <Wire.h>
#include <MeGyro.h> 

int16_t moveSpeed = 100;


//used to keep track of the arm position
long startPos; //bottom of range
long curPos;   //current pos
long endPos; //top of range

MeEncoderOnBoard Encoder_1(SLOT2);
MeEncoderOnBoard Encoder_2(SLOT1);

MeEncoderOnBoard Encoder_3(SLOT3);
MeEncoderOnBoard Encoder_4(SLOT4); //encoder for claw motor


MeGyro gyro( PORT_7 );
MeUltrasonicSensor ultraSensor( PORT_8 );

String command; //command from user
float dist; //distance from ultrasonic measurements  

float scale; //used to compare motor rotation to track distance traveled


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

//motion methods
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
void TurnInplaceRight(void)
{
  Encoder_1.setMotorPwm( -moveSpeed );
  Encoder_2.setMotorPwm( -moveSpeed );
}
void TurnInplaceLeft(void)
{
  Encoder_1.setMotorPwm( moveSpeed );
  Encoder_2.setMotorPwm( moveSpeed );
}
void Stop(void)
{
  Encoder_1.setMotorPwm(0);
  Encoder_2.setMotorPwm(0);
}
//arm motion methods
void ArmUp( void ){
  Encoder_3.setMotorPwm( -moveSpeed ); 
}
void ArmDown( void ){
  Encoder_3.setMotorPwm( moveSpeed );

}
void ArmStop( void ){
  Encoder_3.setMotorPwm( 0 ); 
}

//making sure the arm is in the right place
void ArmZero( void ){ //this is the working zero method

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
void ArmReset( void ){ //this is the working reset
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



   scale = ( abs(motorTravel1) + abs(motorTravel2))/2/abs(deltaDist);
  //assign scale, use average of both motors
 

  Serial.print( "TRACK ZERO\n" );
  Serial.print( motorTravel1 );
  Serial.print( "\n" );
  Serial.print( motorTravel2 );
  Serial.print( "\n" );
  Serial.print( deltaDist );
  Serial.print( "\n SCALE: " );
  Serial.print( scale );
  Serial.print( "\n" );

  
  
}

//newest axact turn methods, use gyro on board
void turn90r( void ){
    Stop();
    gyro.begin();

    double t0 = gyro.getAngle(3);
    TurnInplaceRight();
  
    double t1 = gyro.getAngle(3);
    while( abs( t1 - t0) <= 90 ){
        t1 = gyro.getAngle(3);
       
    }
    Stop();
    Serial3.write( "RIGHT 90" );    
}

void turn90l( void ){
    Stop();
    gyro.begin();

    double t0 = gyro.getAngle(3);
    TurnInplaceLeft();
  
    double t1 = gyro.getAngle(3);
    while( abs( t1 - t0) <= 90 ){
        t1 = gyro.getAngle(3);
    }
    Stop();
    Serial3.write( "LEFT 90" );

    
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

  gyro.begin();

  
  ArmZero();
  ArmReset();
  
  setTrackZero();

  Serial.print("SETUP DONE");
  
  

}

//to be used to track distance traveled
long startPos1;
long startPos2;
long endPos1;
long endPos2;


void loop() {


   //if(( Serial3.available() > 0);
  
   command = Serial3.readString()[0];
   Serial.print( "\n");
   Serial.print( command );

   float dist = ultraSensor.distanceCm();

   Encoder_3.updateCurPos();
   curPos = Encoder_3.getCurPos();

   if (command == "w") {
      Serial3.print("backward");
      Encoder_1.updateCurPos();
      Encoder_2.updateCurPos();
      startPos1 = Encoder_1.getCurPos();
      startPos2 = Encoder_2.getCurPos();
      
      if(dist < 5.0) Stop();
      else if(dist < 15.0) ForwardSlow();
      else Forward();
      Serial.write( "DIST" );
  }
  else if (command == "s"){
      Serial3.print("backward");
      Encoder_1.updateCurPos();
      Encoder_2.updateCurPos();
      startPos1 = Encoder_1.getCurPos();
      startPos2 = Encoder_2.getCurPos();

      Backward();
      Serial.write( "DIST" );
  }
  else if (command == "a"){
    Serial3.print("left");
    turn90l();
    command = "q";
  }
  else if (command == "d"){
    Serial3.print("right");
    turn90r();
    command = "q";
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

      Encoder_1.updateCurPos();
      Encoder_2.updateCurPos();
      endPos1 = Encoder_1.getCurPos();
      endPos2 = Encoder_2.getCurPos();
      
      float DIST = (abs(endPos1 - startPos1)+ abs(endPos2 - startPos2))/(2*scale);
      
      Serial.print( DIST );
      Serial3.print( DIST );
     
     }



     
}









