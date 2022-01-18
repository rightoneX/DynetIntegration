//rev.1.4.3 2016.09.21 15:49

//device settings--------------------------------------------------------------------------------
#define serialNumLW0 0x01 //last two numbers is the device network number (25) as default
#define serialNumLW1 0x00 //last two numbers is the device network number (25) as default
#define serialNumHW1 0x00 //last two numbers is the device network number (25) as default
#define serialNumHW0 0x00 //last two numbers is the device network number (25) as default

#define deviceType  0x50       //switch panel, dimmer, relay, bridge and etc.
#define deviceNumber 0x5F      //device box number
#define devicePCB 0x00         //PCB revision
#define deviceFWgeneration 0x02//Firmware revision
#define deviceFWvar 0x01       //Firmware revision

//RS485Bus Connection Setup
#define comPortSpeed 9600
#define pinComStatus 13 // RS485 Communication
#define netMessage_type 0x1c

//Hardware Setup
#define comServiceBut 12

#define channelQuantity 8 //panel type button quantity from 1 to 8

int channelPin[channelQuantity] = {2,3,4,5,6,7,8,9};


//******************************************************************************************************

//channels Declaration----------------------------------------------------------------------------
int channelDefaultStatus[channelQuantity] = {1,1,0,0,0,0,0,0}; // 0 - off, 100 - on
int channelTempStatus[channelQuantity] = {0,0,0,0,0,0,0,0}; // 0 - off, 100 - on


//Channel Function 
int channelStatus1[channelQuantity] = {0,0,0,0,0,0,0,0}; // 0 - off, 100 - on
int channelStatus1Byte1[channelQuantity] = {0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C}; //message Type
int channelStatus1Byte2[channelQuantity] = {0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02}; //message Area
int channelStatus1Byte3[channelQuantity] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //message 3rd byte
int channelStatus1Byte4[channelQuantity] = {0x03,0x00,0x02,0x03,0x04,0x03,0x06,0x00}; //message Opcode
int channelStatus1Byte5[channelQuantity] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //message 5th byte
int channelStatus1Byte6[channelQuantity] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //message 6th byte
int channelStatus1Byte7[channelQuantity] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}; //message 7th byte

int channelStatus2[channelQuantity] = {100,100,100,100,100,100,100,100}; // 0 - off, 100 - on
int channelStatus2Byte1[channelQuantity] = {0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C}; //message Type
int channelStatus2Byte2[channelQuantity] = {0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02}; //message Area
int channelStatus2Byte3[channelQuantity] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //message 3rd byte
int channelStatus2Byte4[channelQuantity] = {0x00,0x00,0x02,0x03,0x04,0x03,0x06,0x00}; //message Opcode
int channelStatus2Byte5[channelQuantity] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //message 5th byte
int channelStatus2Byte6[channelQuantity] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //message 6th byte
int channelStatus2Byte7[channelQuantity] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}; //message 7th byte


void setup() { 
Serial.begin(comPortSpeed);

pinMode(comServiceBut, INPUT);

int channelNum = 0;
 do{ 
      pinMode(channelPin[channelNum],OUTPUT); //button 2 pin 
      digitalWrite(channelPin[channelNum], channelDefaultStatus[channelNum]); // button disable output      
      channelNum++;
   }while (channelNum <= channelQuantity);
   
delay(10);
//sends device box numer on a start
//deviceBoxNumber();

}




//MINE LOOP------------------------------------------------------------
void loop() {

 netMessageReceive();

 channelStatus();

 //service button
if(digitalRead(comServiceBut) == 1){//service button pushed
  // Serial.println("service button " + String(digitalRead(comServiceBut)));//test
         if(channelTempStatus[0] == 0 ){ 
          channelDefaultStatus[0] = 0;//on
          channelTempStatus[0] = 1;}
        else{
          channelDefaultStatus[0] = 1; //off
          channelTempStatus[0] = 0;
       }
  
 }


}
//********************************************************************



void channelStatus(){ 
  int channelNum = 0;  
  do{ 
      digitalWrite(channelPin[channelNum], channelDefaultStatus[channelNum]); // button disable output      
      channelNum++;
   }while (channelNum <= channelQuantity);
}



//---Serial port receive function-------------------------------------------------------
void netMessageReceive(){
  
 digitalWrite(pinComStatus,0);//set com port to listening mode

 if(Serial.available() > 0) {
  byte serialData[7];
  Serial.readBytes(serialData, 8);

//1C  serialdata[0] message type, [1] logical area, [2] fade time, [3] scene number, [4] message type, [5] message type, [6] join
//5C  serialdata[0] message type, [1] Device Type, [2] Box Number, [3] Opc, [4] message type, [5] message type, [6] join

unsigned int checkSum; //Checking the sum number
for(int i =0; i++; i==6){ checkSum =+ (serialData[i], HEX);}
checkSum = 0x200 - (checkSum,HEX);

if((checkSum,HEX) == (serialData[7],HEX)){
        //first byte - message type
        switch (serialData[0]) {
          
              case 0x1C://logical message type

 /*
                      if(serialData[1] ==  0x02 && buttonToggleStatus[buttonNum] == 0){       
                        buttonToggleStatus[buttonNum] = 1;      //Statuse 1                                   
                        buttonLEDStatus[buttonNum] = buttonPush1FuncLEDStatus[buttonNum];                                             
                        netMessageSend(buttonPush1FuncByte1[buttonNum], buttonPush1FuncByte2[buttonNum], buttonPush1FuncByte3[buttonNum], buttonPush1FuncByte4[buttonNum], buttonPush1FuncByte5[buttonNum], buttonPush1FuncByte6[buttonNum], buttonPush1FuncByte7[buttonNum]);                                                  
                      }else if(digitalRead(buttonPin[buttonNum]) ==  0 && buttonToggleStatus[buttonNum] == 1){                       
                        buttonToggleStatus[buttonNum] = 0;      //Statuse 2                                   
                        buttonLEDStatus[buttonNum] = buttonPush2FuncLEDStatus[buttonNum];                                             
                        netMessageSend(buttonPush2FuncByte1[buttonNum], buttonPush2FuncByte2[buttonNum], buttonPush2FuncByte3[buttonNum], buttonPush2FuncByte4[buttonNum], buttonPush2FuncByte5[buttonNum], buttonPush2FuncByte6[buttonNum], buttonPush2FuncByte7[buttonNum]);
                       }

                    if(channelStatus1[1] == 0){
                       digitalWrite(channelPin[1], 1);
                       channelStatus1[1] = 1;
                       Serial.println("0");
  
                      }else{
                        digitalWrite(channelPin[1], 0);
                       channelStatus1[1] = 0; 
                       Serial.println("1");
                      }
*/
                    Serial.println(serialData[3]);
                    
                      if(serialData[3] ==  0x03){
                       digitalWrite(channelPin[0], 1);
                   //    channelStatus1[1] = 1;
                    //   Serial.println("0");
  
                      }
                      if(serialData[3] ==  0x00){
                        digitalWrite(channelPin[0], 0);
                    //   channelStatus1[1] = 0; 
                    //   Serial.println("1");
                      }
                    
              break;


        
              case 0x5C://physical message type
                  //checking the box number addressing
                  if(serialData[1] == deviceType && serialData[2] == deviceNumber){
                       //Checking Opc
                       switch(serialData[3]){
                        case 0x01: // set device to the programming mode 
                      //    progModeEnabled = true;
                        break;
                        case 0x02: // reset the device to the programming mode 
                    //     resetDevice(); //call reset  
                        break;   
                        case 0x40: //request device  serial Number 
                            if(serialData[6] == 0x0D){//status LoWord
                              delay(10);
                         //     netMessageSend(0x5C,deviceType,deviceNumber,0x41,serialNumLW1,serialNumLW0,0x0D);
                            }
                            if(serialData[6] == 0x0E){//status HiWord
                              delay(10);
                         //     netMessageSend(0x5C,deviceType,deviceNumber,0x41,serialNumHW1,serialNumHW0,0x0E);                              
                            }                            
                        break; 
                         
                        case 0x80: //request device  firmware version
                              delay(10);
                          //    netMessageSend(0x5C,deviceType,deviceNumber,0x00,deviceFWvar,deviceFWgeneration,0x00);
                        break; 
                         
                        case 0x03: //read EEPROM
                           //adress serialData[4] serialData[5] data serialData[6]
                              delay(10);
                          //    netMessageSend(0x5C,deviceType,deviceNumber,0x05,address,address,0xdata);
                        break; 
                      }                    
                    }                 
                break;
             }

    }
       
  }
}
//*********************************************************************************

