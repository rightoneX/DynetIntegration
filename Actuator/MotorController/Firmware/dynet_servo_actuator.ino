String readString;
#include <Servo.h>
Servo myservo;  // create servo object to control a servo
  #define pinComStatus 13 // RS485 Communication

  
void setup() {
  Serial.begin(9600);

  myservo.writeMicroseconds(1500); //set initial servo position if desired
  myservo.attach(2);  //the pin for the servo control
  Serial.println("servo-test-22-dual-input"); // so I can keep track of what is loaded
}

void loop() {
//  while (Serial.available()) {
   // char input = Serial.read();  //gets one byte from serial buffer
  //  readString += input; //makes the string readString
  //  delay(2);  //slow looping to allow buffer to fill with next character
 // }
/*
  if (readString.length() >0) {
      Serial.println(readString);  //so you can see the captured string
      int degree = readString.toInt();  //convert readString into a number
      
      myservo.write(degree);
      
      Serial.print("writing Angle: ");
      Serial.println(degree);
      readString=""; //empty for next input
  }
*/

     netMessageReceive();
    //  digitalWrite(pinComStatus,1);//set com port to listening mode

} 



//---Serial port receive function-------------------------------------------------------
void netMessageReceive(){
 //digitalWrite(pinComStatus,0);//set com port to listening mode

 if(Serial.available() > 0) {
  byte serialData[7];
  Serial.readBytes(serialData, 8);
//1C  serialdata[0] message type, [1] logical area, [2] fade time, [3] scene number, [4] message type, [5] message type, [6] join
//5C  serialdata[0] message type, [1] Device Type, [2] Box Number, [3] Opc, [4] message type, [5] message type, [6] join
 
//Checking the sum number Serial.print(serialData[i],HEX); Serial.print(":");
unsigned int checkSum; 
for(int i =0; i++; i==6){checkSum =+ (serialData[i], HEX);}
checkSum = 0x200 - (checkSum,HEX);

if((checkSum,HEX) == (serialData[7],HEX)){ //Check sum is correct
        //first byte - message type
        switch (serialData[0]) {
              case 0x1C://logical message type
              
                 if( serialData[1] == 0x02){//Area
                  
                                //preset
                                 if(serialData[3] == 0x00){
                                     myservo.write(90);
                                  }
                                  if(serialData[3] == 0x03){
                                     myservo.write(180);
                                  }
              
                                  if(serialData[3] == 0x02){
                                     myservo.write(0);
                                  }
                 }
                  //   logicalMessageChecking(serialData[1],serialData[2],serialData[3],serialData[4],serialData[5],serialData[6]);
                 
              break;
              

/*

                    
              case 0x5C://physical message type
 
                  if(serialData[1] == deviceType && serialData[2] == deviceNumber){  //checking the box number & device type
                       switch(serialData[3]){ //Checking Opc
                              case 0x01: // set device to the programming mode 
                                progModeEnabled = true;
                              break;
                              
                              case 0x02: // reset the device
                                  resetDevice(); //call reset  
                              break; 
                                
                              case 0x40: //request device  serial Number 
                                  if(serialData[6] == 0x0D){//status LoWord
                                    delay(delayTransmition);
                                    netMessageSend(0x5C,deviceType,deviceNumber,0x41,serialNumLW1,serialNumLW0,0x0D);
                                  }
                                  if(serialData[6] == 0x0E){//status HiWord
                                    delay(delayTransmition);
                                    netMessageSend(0x5C,deviceType,deviceNumber,0x41,serialNumHW1,serialNumHW0,0x0E);                              
                                  }                            
                              break; 
                               
                              case 0x80: //request device  firmware version
                                    delay(delayTransmition);
                                    netMessageSend(0x5C,deviceType,deviceNumber,0x00,deviceFWvar,deviceFWgeneration,0x00);
                              break; 
                               
                              case 0x03: //read EEPROM      //adress serialData[4] serialData[5]           //  data serialData[6]                        
                                    delay(delayTransmition);
                                    netMessageSend(0x5C,deviceType,deviceNumber,0x05,EEPROM.read(serialData[4]),EEPROM.read(serialData[5]),EEPROM.read(serialData[6]));
                              break; 
                      }                    
                    }                 
                break;
                */
             }
    }       
  }
}
//*********************************************************************************
