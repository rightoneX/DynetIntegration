#include <Servo.h>

Servo myServo;  // create a servo object
int pos = 0;

#define inputSound 13
#define outputServo 2

int inputCount = 0;
int countMeter = 0;

void setup() { 
Serial.begin(9600);

myServo.attach(outputServo); // attaches the servo on pin 9 to the servo object
myServo.write(45); 
pinMode(inputSound ,INPUT);//LED common pin
}



//MINE LOOP------------------------------------------------------------
void loop() {
  
//myServo.write(45); 
   
  countMeter ++; 
  delayMicroseconds(100);
   
  //clear the meter
  if (countMeter > 10000){
    countMeter = 0; 
    inputCount = 0; 
    }
    
 
  
    if(digitalRead(inputSound)){
         countMeter = 0;      
         inputCount ++;
          Serial.println(inputCount); 
         delay(100);
     }    
      
    if(inputCount == 3 && countMeter == 9999){
         Serial.println("LIGHTS OFF"); 
        inputCount = 0; 
        countMeter = 0; 
       pos = 0;
       myServo.write(pos); 
        delayMicroseconds(50);
    }
         
    if(inputCount == 2 && countMeter == 9000){
         Serial.println("LIGHTS ON"); 
        inputCount = 0; 
        countMeter = 0; 
        pos = 180;
        myServo.write(pos); 
        delayMicroseconds(50);
    }
  



  

}
//---------------------------------------------------------------------



