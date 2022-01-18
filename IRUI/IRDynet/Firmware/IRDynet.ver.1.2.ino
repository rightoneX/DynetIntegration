#include <IRremote.h>

//RS485Bus Connection Setup
#define comPortSpeed 9600
#define pinComStatus 13 // RS485 Communication
#define delayTransmition 10 // Delay on transmition

#include <SoftwareSerial.h>
//// software serial #1: RX = digital pin 3, TX = digital pin 2
SoftwareSerial portOne(3, 2);

//IR Receiver Input
int RECV_PIN = 11;

IRrecv irrecv(RECV_PIN);
decode_results results;

int ircheck = 0;

//ir code - dynalite
#define IRCommandNumber 16

//unsigned long irCode[IRCommandNumber] = {16736925,16769565,16720605,16712445,16761405,16769055,16754775,16748655,16738455,16750695,16756815,16718055,16716015,16734885,16730805,16726215}; //ir code
unsigned long irCode[IRCommandNumber] = {16736925,16769565,16720605,16712445,16761405,16769055,16754775,16748655,16738455,16750695,16756815,16718055,16716015,16734885,16730805,16726215}; //ir code
int DynetCode0[IRCommandNumber] = {0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C}; //message Type
int DynetCode1[IRCommandNumber] = {0x02,0x02,0x03,0x03,0x04,0x04,0x05,0x05,0x06,0x06,0x07,0x07,0x33,0x33,0x34,0x34}; //message Area
int DynetCode2[IRCommandNumber] = {0x64,0x64,0x64,0x64,0x64,0x64,0x64,0x64,0x64,0x64,0x64,0x64,0x64,0x64,0x64,0x64}; //message 3rd byte
int DynetCode3[IRCommandNumber] = {0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03}; //message Opcode
int DynetCode4[IRCommandNumber] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //message 5th byte
int DynetCode5[IRCommandNumber] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //message 6th byte
int DynetCode6[IRCommandNumber] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}; //message 7th byte

void setup()
{
  portOne.begin(comPortSpeed);
  irrecv.enableIRIn(); // Start the receiver
    
  Serial.begin(comPortSpeed);//temp 
  Serial.println("running");//temp
}

void loop() {
    if (irrecv.decode(&results)) {   
        ircheck = 0;
//        Serial.println(irCode[0]);
        Serial.println(results.value); //temp
        Serial.println("--------");//temp

        while (ircheck  <= IRCommandNumber) { 
                  
         if(irCode[ircheck] == results.value){
            dynetTransmit(DynetCode0[ircheck],DynetCode1[ircheck],DynetCode2[ircheck],DynetCode3[ircheck],DynetCode4[ircheck],DynetCode5[ircheck],DynetCode6[ircheck]);
          }

          ircheck ++;
        }

     irrecv.resume(); // Receive the next value      
    }
  delay(500);  
}



//Send Command over RS485 netwrok by Dynet1 protocol-------------------------------------------------------
void dynetTransmit(byte commandType, byte areaCode, byte data1, byte opCode, byte data2, byte data3, byte joinCode){

  unsigned int checkSum = commandType + areaCode + data1 + opCode + data2 + data3 + joinCode;

  digitalWrite(pinComStatus, 1);

//    Serial.flush();
//    Serial.write(commandType);
//    Serial.write(areaCode);
//    Serial.write(data1);
//    Serial.write(opCode);
//    Serial.write(data2);
//    Serial.write(data3);
//    Serial.write(joinCode);
//    Serial.write(0x200-checkSum);

    portOne.flush();
    portOne.write(commandType);
    portOne.write(areaCode);
    portOne.write(data1);
    portOne.write(opCode);
    portOne.write(data2);
    portOne.write(data3);
    portOne.write(joinCode);
    portOne.write(0x200-checkSum);
    
  while (!(UCSR0A & (1 << TXC0)));

  digitalWrite(pinComStatus, 0);  //Complete the transmition
}
//*****************************************************************************************************
