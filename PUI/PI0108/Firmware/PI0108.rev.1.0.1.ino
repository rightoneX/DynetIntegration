//rev.1.0.1 2017.12.01 15:49
/*
 TO DO:
 - Send & Receive  function
 - Receiving command via bus follow function
 - change panel settings over bus command
 - eeprom write & read
 - eeprom update via U-bus
 */
 
#include <EEPROM.h>
//#include <EEPROM_SAVE.h>

//device settings--------------------------------------------------------------------------------
#define serialNumLW0 0x01 //last two numbers is the device network number (25) as default
#define serialNumLW1 0x00 //last two numbers is the device network number (25) as default
#define serialNumHW1 0x00 //last two numbers is the device network number (25) as default
#define serialNumHW0 0x00 //last two numbers is the device network number (25) as default

#define deviceType  EEPROM.read(1) //0x80       //switch panel, dimmer, relay, bridge and etc.
#define deviceNumber EEPROM.read(2) //0x5F      //device box number
#define devicePCB EEPROM.read(3) //0x00         //PCB revision
#define deviceFWgeneration EEPROM.read(4) //0x02//Firmware revision
#define deviceFWvar EEPROM.read(5)//0x01       //Firmware revision

//RS485Bus Connection Setup
#define comPortSpeed 9600
#define pinComStatus 13 // RS485 Communication
#define delayTransmition 10 // Delay on transmition
//#define netMessage_type 0x1c

//Button Hardware Setup
#define comBut 2
#define comLED 12
#define buttonQuantity 8 //panel type button quantity from 1 to 8

int buttonPin[buttonQuantity] = {3,4,5,6,7,8,9,10};
bool progModeEnabled = false; //the device in the operational mode
int flashLEDProgMode = 0;

//Button Scan Settings
int buttonScanNumber = 0;  
int delayButtonScan = 0; 
int buttonScanReset = 56;//56
//******************************************************************************************************

//Button Function Declaration----------------------------------------------------------------------------
// 0 - disable, 1 - push, 2 - toggle, 3- push with release, 4 - toggle with release, 5 - run task
int buttonFunction[buttonQuantity] = {EEPROM.read(6),EEPROM.read(7),EEPROM.read(8),EEPROM.read(9),EEPROM.read(10),EEPROM.read(11),EEPROM.read(12),EEPROM.read(13)};
//Default after boot LED ON - 0; led OFF - 1
int buttonLEDStatus[buttonQuantity] = {EEPROM.read(14),EEPROM.read(15),EEPROM.read(16),EEPROM.read(17),EEPROM.read(18),EEPROM.read(19),EEPROM.read(20),EEPROM.read(21)}; 
//Not Pushed - 0; Pushed - 1
int buttonToggleStatus[buttonQuantity] = {EEPROM.read(22),EEPROM.read(23),EEPROM.read(24),EEPROM.read(25),EEPROM.read(26),EEPROM.read(27),EEPROM.read(28),EEPROM.read(29)}; 
//Not Pushed - 0; Pushed - 1
int buttonReleaseStatus[buttonQuantity] = {EEPROM.read(30),EEPROM.read(31),EEPROM.read(32),EEPROM.read(33),EEPROM.read(34),EEPROM.read(35),EEPROM.read(36),EEPROM.read(37)}; 
int buttonReadingStatus[buttonQuantity]; //keeps temp reading value

//Button Push Function 1 (first push)
//LED ON - 0; led OFF - 1
int buttonPush1FuncLEDStatus[buttonQuantity] = {EEPROM.read(38),EEPROM.read(39),EEPROM.read(40),EEPROM.read(41),EEPROM.read(42),EEPROM.read(43),EEPROM.read(44),EEPROM.read(45)}; 
int buttonPush1FuncByte1[buttonQuantity] = {EEPROM.read(46),EEPROM.read(47),EEPROM.read(48),EEPROM.read(49),EEPROM.read(50),EEPROM.read(51),EEPROM.read(52),EEPROM.read(53)};  //message Type
int buttonPush1FuncByte2[buttonQuantity] = {EEPROM.read(54),EEPROM.read(55),EEPROM.read(56),EEPROM.read(57),EEPROM.read(58),EEPROM.read(59),EEPROM.read(60),EEPROM.read(61)}; //message Area
int buttonPush1FuncByte3[buttonQuantity] = {EEPROM.read(62),EEPROM.read(63),EEPROM.read(64),EEPROM.read(65),EEPROM.read(66),EEPROM.read(67),EEPROM.read(68),EEPROM.read(69)}; //message 3rd byte
int buttonPush1FuncByte4[buttonQuantity] = {EEPROM.read(70),EEPROM.read(71),EEPROM.read(72),EEPROM.read(73),EEPROM.read(74),EEPROM.read(75),EEPROM.read(76),EEPROM.read(77)}; //message Opcode
int buttonPush1FuncByte5[buttonQuantity] = {EEPROM.read(78),EEPROM.read(79),EEPROM.read(80),EEPROM.read(81),EEPROM.read(82),EEPROM.read(83),EEPROM.read(84),EEPROM.read(85)}; //message 5th byte
int buttonPush1FuncByte6[buttonQuantity] = {EEPROM.read(86),EEPROM.read(87),EEPROM.read(88),EEPROM.read(89),EEPROM.read(90),EEPROM.read(91),EEPROM.read(92),EEPROM.read(93)}; //message 6th byte
int buttonPush1FuncByte7[buttonQuantity] = {EEPROM.read(94),EEPROM.read(95),EEPROM.read(96),EEPROM.read(97),EEPROM.read(98),EEPROM.read(99),EEPROM.read(100),EEPROM.read(101)}; //message 7th byte

//Button Push Function 2 (second push toggle)
int buttonPush2FuncLEDStatus[buttonQuantity] = {EEPROM.read(102),EEPROM.read(103),EEPROM.read(104),EEPROM.read(105),EEPROM.read(106),EEPROM.read(107),EEPROM.read(108),EEPROM.read(109)}; 
int buttonPush2FuncByte1[buttonQuantity] = {EEPROM.read(110),EEPROM.read(111),EEPROM.read(112),EEPROM.read(113),EEPROM.read(114),EEPROM.read(115),EEPROM.read(116),EEPROM.read(117)};  //message Type
int buttonPush2FuncByte2[buttonQuantity] = {EEPROM.read(118),EEPROM.read(119),EEPROM.read(120),EEPROM.read(121),EEPROM.read(122),EEPROM.read(123),EEPROM.read(124),EEPROM.read(125)}; //message Area
int buttonPush2FuncByte3[buttonQuantity] = {EEPROM.read(126),EEPROM.read(127),EEPROM.read(128),EEPROM.read(129),EEPROM.read(130),EEPROM.read(131),EEPROM.read(132),EEPROM.read(133)}; //message 3rd byte
int buttonPush2FuncByte4[buttonQuantity] = {EEPROM.read(134),EEPROM.read(135),EEPROM.read(136),EEPROM.read(137),EEPROM.read(138),EEPROM.read(139),EEPROM.read(140),EEPROM.read(141)}; //message Opcode
int buttonPush2FuncByte5[buttonQuantity] = {EEPROM.read(142),EEPROM.read(143),EEPROM.read(144),EEPROM.read(145),EEPROM.read(146),EEPROM.read(147),EEPROM.read(148),EEPROM.read(149)}; //message 5th byte
int buttonPush2FuncByte6[buttonQuantity] = {EEPROM.read(150),EEPROM.read(151),EEPROM.read(152),EEPROM.read(153),EEPROM.read(154),EEPROM.read(155),EEPROM.read(156),EEPROM.read(157)}; //message 6th byte
int buttonPush2FuncByte7[buttonQuantity] = {EEPROM.read(158),EEPROM.read(159),EEPROM.read(160),EEPROM.read(161),EEPROM.read(162),EEPROM.read(163),EEPROM.read(164),EEPROM.read(165)}; //message 7th byte


//Button Push Function 3 (relaes after first push)
int buttonPush3FuncLEDStatus[buttonQuantity] = {EEPROM.read(166),EEPROM.read(167),EEPROM.read(168),EEPROM.read(169),EEPROM.read(170),EEPROM.read(171),EEPROM.read(172),EEPROM.read(173)}; 
int buttonPush3FuncByte1[buttonQuantity] = {EEPROM.read(174),EEPROM.read(175),EEPROM.read(176),EEPROM.read(177),EEPROM.read(178),EEPROM.read(179),EEPROM.read(180),EEPROM.read(181)};  //message Type
int buttonPush3FuncByte2[buttonQuantity] = {EEPROM.read(182),EEPROM.read(183),EEPROM.read(184),EEPROM.read(185),EEPROM.read(186),EEPROM.read(187),EEPROM.read(188),EEPROM.read(189)}; //message Area
int buttonPush3FuncByte3[buttonQuantity] = {EEPROM.read(190),EEPROM.read(191),EEPROM.read(192),EEPROM.read(193),EEPROM.read(194),EEPROM.read(195),EEPROM.read(196),EEPROM.read(197)}; //message 3rd byte
int buttonPush3FuncByte4[buttonQuantity] = {EEPROM.read(198),EEPROM.read(199),EEPROM.read(200),EEPROM.read(201),EEPROM.read(202),EEPROM.read(203),EEPROM.read(204),EEPROM.read(205)}; //message Opcode
int buttonPush3FuncByte5[buttonQuantity] = {EEPROM.read(206),EEPROM.read(207),EEPROM.read(208),EEPROM.read(209),EEPROM.read(210),EEPROM.read(211),EEPROM.read(212),EEPROM.read(213)}; //message 5th byte
int buttonPush3FuncByte6[buttonQuantity] = {EEPROM.read(214),EEPROM.read(215),EEPROM.read(216),EEPROM.read(217),EEPROM.read(218),EEPROM.read(219),EEPROM.read(220),EEPROM.read(221)}; //message 6th byte
int buttonPush3FuncByte7[buttonQuantity] = {EEPROM.read(222),EEPROM.read(223),EEPROM.read(224),EEPROM.read(225),EEPROM.read(226),EEPROM.read(227),EEPROM.read(228),EEPROM.read(229)}; //message 7th byte


//Button Push Function 4 (relaes after second push toggle)
//int buttonPush4FuncLEDStatus[buttonQuantity] = {1,0,0,0,0,0,0,0}; //LED ON - 0; led OFF - 1  230
//int buttonPush4FuncByte1[buttonQuantity] = {0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C}; //message Type
//int buttonPush4FuncByte2[buttonQuantity] = {0x02,0x03,0x03,0x03,0x03,0x03,0x03,0x03}; //message Area
//int buttonPush4FuncByte3[buttonQuantity] = {0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //message 3rd byte
//int buttonPush4FuncByte4[buttonQuantity] = {0x76,0x03,0x03,0x03,0x03,0x03,0x03,0x03}; //message Opcode
//int buttonPush4FuncByte5[buttonQuantity] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //message 5th byte
//int buttonPush4FuncByte6[buttonQuantity] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //message 6th byte
//int buttonPush4FuncByte7[buttonQuantity] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}; //message 7th byte

int buttonPush4FuncLEDStatus[buttonQuantity] = {EEPROM.read(230),EEPROM.read(231),EEPROM.read(232),EEPROM.read(233),EEPROM.read(234),EEPROM.read(235),EEPROM.read(236),EEPROM.read(237)}; 
int buttonPush4FuncByte1[buttonQuantity] = {EEPROM.read(238),EEPROM.read(239),EEPROM.read(240),EEPROM.read(241),EEPROM.read(242),EEPROM.read(243),EEPROM.read(244),EEPROM.read(245)};  //message Type
int buttonPush4FuncByte2[buttonQuantity] = {EEPROM.read(246),EEPROM.read(247),EEPROM.read(248),EEPROM.read(249),EEPROM.read(250),EEPROM.read(251),EEPROM.read(252),EEPROM.read(253)}; //message Area
int buttonPush4FuncByte3[buttonQuantity] = {EEPROM.read(254),EEPROM.read(255),EEPROM.read(256),EEPROM.read(257),EEPROM.read(258),EEPROM.read(259),EEPROM.read(260),EEPROM.read(261)}; //message 3rd byte
int buttonPush4FuncByte4[buttonQuantity] = {EEPROM.read(262),EEPROM.read(263),EEPROM.read(264),EEPROM.read(265),EEPROM.read(266),EEPROM.read(267),EEPROM.read(268),EEPROM.read(269)}; //message Opcode
int buttonPush4FuncByte5[buttonQuantity] = {EEPROM.read(270),EEPROM.read(271),EEPROM.read(272),EEPROM.read(273),EEPROM.read(274),EEPROM.read(275),EEPROM.read(276),EEPROM.read(277)}; //message 5th byte
int buttonPush4FuncByte6[buttonQuantity] = {EEPROM.read(278),EEPROM.read(279),EEPROM.read(280),EEPROM.read(281),EEPROM.read(282),EEPROM.read(283),EEPROM.read(284),EEPROM.read(285)}; //message 6th byte
int buttonPush4FuncByte7[buttonQuantity] = {EEPROM.read(286),EEPROM.read(287),EEPROM.read(288),EEPROM.read(289),EEPROM.read(290),EEPROM.read(291),EEPROM.read(292),EEPROM.read(293)}; //message 7th byte
//************************************************************************************************************

void defaultConfigSettings(){
   EEPROM.write(0,1); // 0    //Save at first booting
   
   EEPROM.write(1,0x80); // #define deviceType         //switch panel, dimmer, relay, bridge and etc.
   EEPROM.write(2,0x5F); // #define deviceNumber       //device box number
   EEPROM.write(3,0x01); // #define devicePCB          //PCB revision  
   EEPROM.write(4,0x02); // #define deviceFWgeneration //Firmware revision
   EEPROM.write(5,0x01); // #define deviceFWvar        //Firmware revision
 
   //BUTTON FUNCTION
   //0 - disable, 1 - push, 2 - toggle, 3- push with release, 4 - toggle with release, 5 - run task
   EEPROM.write(6,2);    //button 1 function
   EEPROM.write(7,2);    //button 2 function
   EEPROM.write(8,2);    //button 3 function 
   EEPROM.write(9,2);    //button 4 function 
   EEPROM.write(10,2);   //button 5 function 
   EEPROM.write(11,2);   //button 6 function 
   EEPROM.write(12,2);   //button 7 function
   EEPROM.write(13,2);   //button 8 function 
    // LED ON - 0; led OFF - 1
   //int buttonLEDStatus[buttonQuantity] = {1,1,1,1,1,1,1,1}; //LED ON - 0; led OFF - 1
   EEPROM.write(14,1);    //button 1 function
   EEPROM.write(15,1);    //button 2 function
   EEPROM.write(16,1);    //button 3 function 
   EEPROM.write(17,1);    //button 4 function 
   EEPROM.write(18,1);   //button 5 function 
   EEPROM.write(19,1);   //button 6 function 
   EEPROM.write(20,1);   //button 7 function
   EEPROM.write(21,1);   //button 8 function 
   //buttonToggleStatus[buttonQuantity] = {0,0,0,0,0,0,0,0};    
   EEPROM.write(22,0);   //button 1 ToggleStatus   
   EEPROM.write(23,0);   //button 2 ToggleStatus   
   EEPROM.write(24,0);   //button 3 ToggleStatus   
   EEPROM.write(25,0);   //button 4 ToggleStatus   
   EEPROM.write(26,0);   //button 5 ToggleStatus   
   EEPROM.write(27,0);   //button 6 ToggleStatus   
   EEPROM.write(28,0);   //button 7 ToggleStatus   
   EEPROM.write(29,0);   //button 8 ToggleStatus 
   //int buttonReleaseStatus[buttonQuantity] = {0,0,0,0,0,0,0,0}; //Not Pushed - 0; Pushed - 1
   EEPROM.write(30,0);   //button 1 ReleaseStatus
   EEPROM.write(31,0);   //button 2 ReleaseStatus 
   EEPROM.write(32,0);   //button 3 ReleaseStatus  
   EEPROM.write(33,0);   //button 4 ReleaseStatus 
   EEPROM.write(34,0);   //button 5 ReleaseStatus   
   EEPROM.write(35,0);   //button 6 ReleaseStatus  
   EEPROM.write(36,0);   //button 7 ReleaseStatus  
   EEPROM.write(37,0);   //button 8 ReleaseStatus
   //buttonPush1FuncLEDStatus[buttonQuantity] = {1,0,0,0,0,0,0,0}; //LED ON - 0; led OFF - 1  
   EEPROM.write(38,1);   //button 1 Func LEDStatus
   EEPROM.write(39,0);   //button 1 Func LEDStatus
   EEPROM.write(40,0);   //button 1 Func LEDStatus
   EEPROM.write(41,0);   //button 1 Func LEDStatus
   EEPROM.write(42,0);   //button 1 Func LEDStatus
   EEPROM.write(43,0);   //button 1 Func LEDStatus
   EEPROM.write(44,0);   //button 1 Func LEDStatus
   EEPROM.write(45,0);   //button 1 Func LEDStatus
//int buttonPush1FuncByte1[buttonQuantity] = {0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C}; //message Type
   EEPROM.write(46,0x1C);   //button 1 Func LEDStatus
   EEPROM.write(47,0x1C);   //button 1 Func LEDStatus
   EEPROM.write(48,0x1C);   //button 1 Func LEDStatus
   EEPROM.write(49,0x1C);   //button 1 Func LEDStatus
   EEPROM.write(50,0x1C);   //button 1 Func LEDStatus
   EEPROM.write(51,0x1C);   //button 1 Func LEDStatus
   EEPROM.write(52,0x1C);   //button 1 Func LEDStatus
   EEPROM.write(53,0x1C);   //button 1 Func LEDStatus
//int buttonPush1FuncByte2[buttonQuantity] = {0x02,0x03,0x02,0x02,0x02,0x02,0x02,0x02}; //message Area
   EEPROM.write(54,0x02);   //button 1 Func LEDStatus
   EEPROM.write(55,0x02);   //button 1 Func LEDStatus
   EEPROM.write(56,0x02);   //button 1 Func LEDStatus
   EEPROM.write(57,0x02);   //button 1 Func LEDStatus
   EEPROM.write(58,0x02);   //button 1 Func LEDStatus
   EEPROM.write(59,0x02);   //button 1 Func LEDStatus
   EEPROM.write(60,0x02);   //button 1 Func LEDStatus
   EEPROM.write(61,0x02);   //button 1 Func LEDStatus
//int buttonPush1FuncByte3[buttonQuantity] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //message 3rd byte
   EEPROM.write(62,0x00);   //button 1 Func LEDStatus
   EEPROM.write(63,0x00);   //button 1 Func LEDStatus
   EEPROM.write(64,0x00);   //button 1 Func LEDStatus
   EEPROM.write(65,0x00);   //button 1 Func LEDStatus
   EEPROM.write(66,0x00);   //button 1 Func LEDStatus
   EEPROM.write(67,0x00);   //button 1 Func LEDStatus
   EEPROM.write(68,0x00);   //button 1 Func LEDStatus
   EEPROM.write(69,0x00);   //button 1 Func LEDStatus
//int buttonPush1FuncByte4[buttonQuantity] = {0x03,0x03,0x02,0x03,0x04,0x05,0x06,0x00}; //message Opcode
   EEPROM.write(70,0x03);   //button 1 Func LEDStatus
   EEPROM.write(71,0x03);   //button 1 Func LEDStatus
   EEPROM.write(72,0x02);   //button 1 Func LEDStatus
   EEPROM.write(73,0x03);   //button 1 Func LEDStatus
   EEPROM.write(74,0x04);   //button 1 Func LEDStatus
   EEPROM.write(75,0x05);   //button 1 Func LEDStatus
   EEPROM.write(76,0x06);   //button 1 Func LEDStatus
   EEPROM.write(77,0x00);   //button 1 Func LEDStatus
//int buttonPush1FuncByte5[buttonQuantity] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //message 5th byte
   EEPROM.write(78,0x00);   //button 1 Func LEDStatus
   EEPROM.write(79,0x00);   //button 1 Func LEDStatus
   EEPROM.write(80,0x00);   //button 1 Func LEDStatus
   EEPROM.write(81,0x00);   //button 1 Func LEDStatus
   EEPROM.write(82,0x00);   //button 1 Func LEDStatus
   EEPROM.write(83,0x00);   //button 1 Func LEDStatus
   EEPROM.write(84,0x00);   //button 1 Func LEDStatus
   EEPROM.write(85,0x00);   //button 1 Func LEDStatus
//int buttonPush1FuncByte6[buttonQuantity] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //message 6th byte
   EEPROM.write(86,0x00);   //button 1 Func LEDStatus
   EEPROM.write(87,0x00);   //button 1 Func LEDStatus
   EEPROM.write(88,0x00);   //button 1 Func LEDStatus
   EEPROM.write(89,0x00);   //button 1 Func LEDStatus
   EEPROM.write(90,0x00);   //button 1 Func LEDStatus
   EEPROM.write(91,0x00);   //button 1 Func LEDStatus
   EEPROM.write(92,0x00);   //button 1 Func LEDStatus
   EEPROM.write(93,0x00);   //button 1 Func LEDStatus
//int buttonPush1FuncByte7[buttonQuantity] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}; //message 7th byte
   EEPROM.write(94,0xff);   //button 1 Func LEDStatus
   EEPROM.write(95,0xff);   //button 1 Func LEDStatus
   EEPROM.write(96,0xff);   //button 1 Func LEDStatus
   EEPROM.write(97,0xff);   //button 1 Func LEDStatus
   EEPROM.write(98,0xff);   //button 1 Func LEDStatus
   EEPROM.write(99,0xff);   //button 1 Func LEDStatus
   EEPROM.write(100,0xff);   //button 1 Func LEDStatus
   EEPROM.write(101,0xff);   //button 1 Func LEDStatus
//int buttonPush2FuncLEDStatus[buttonQuantity] = {0,1,1,1,1,1,1,1}; //LED ON - 0; led OFF - 1 102
   EEPROM.write(102,0x00);   //button 1 Func LEDStatus
   EEPROM.write(103,0x01);   //button 1 Func LEDStatus
   EEPROM.write(104,0x01);   //button 1 Func LEDStatus
   EEPROM.write(105,0x01);   //button 1 Func LEDStatus
   EEPROM.write(106,0x01);   //button 1 Func LEDStatus
   EEPROM.write(107,0x01);   //button 1 Func LEDStatus
   EEPROM.write(108,0x01);   //button 1 Func LEDStatus
   EEPROM.write(109,0x01);   //button 1 Func LEDStatus
//int buttonPush2FuncByte1[buttonQuantity] = {0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C}; //message Type
   EEPROM.write(110,0x1C);   //button 1 Func LEDStatus
   EEPROM.write(111,0x1C);   //button 1 Func LEDStatus
   EEPROM.write(112,0x1C);   //button 1 Func LEDStatus
   EEPROM.write(113,0x1C);   //button 1 Func LEDStatus
   EEPROM.write(114,0x1C);   //button 1 Func LEDStatus
   EEPROM.write(115,0x1C);   //button 1 Func LEDStatus
   EEPROM.write(116,0x1C);   //button 1 Func LEDStatus
   EEPROM.write(117,0x1C);   //button 1 Func LEDStatus
//int buttonPush2FuncByte2[buttonQuantity] = {0x02,0x03,0x02,0x02,0x02,0x02,0x02,0x02}; //message Area
   EEPROM.write(118,0x02);   //button 1 Func LEDStatus
   EEPROM.write(119,0x03);   //button 1 Func LEDStatus
   EEPROM.write(120,0x02);   //button 1 Func LEDStatus
   EEPROM.write(121,0x02);   //button 1 Func LEDStatus
   EEPROM.write(122,0x02);   //button 1 Func LEDStatus
   EEPROM.write(123,0x02);   //button 1 Func LEDStatus
   EEPROM.write(124,0x02);   //button 1 Func LEDStatus
   EEPROM.write(125,0x02);   //button 1 Func LEDStatus
//int buttonPush2FuncByte3[buttonQuantity] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //message 3rd byte
   EEPROM.write(126,0x00);   //button 1 Func LEDStatus
   EEPROM.write(127,0x00);   //button 1 Func LEDStatus
   EEPROM.write(128,0x00);   //button 1 Func LEDStatus
   EEPROM.write(129,0x00);   //button 1 Func LEDStatus
   EEPROM.write(130,0x00);   //button 1 Func LEDStatus
   EEPROM.write(131,0x00);   //button 1 Func LEDStatus
   EEPROM.write(132,0x00);   //button 1 Func LEDStatus
   EEPROM.write(133,0x00);   //button 1 Func LEDStatus
//int buttonPush2FuncByte4[buttonQuantity] = {0x00,0x00,0x03,0x03,0x03,0x03,0x03,0x03}; //message Opcode
   EEPROM.write(134,0x00);   //button 1 Func LEDStatus
   EEPROM.write(135,0x00);   //button 1 Func LEDStatus
   EEPROM.write(136,0x03);   //button 1 Func LEDStatus
   EEPROM.write(137,0x03);   //button 1 Func LEDStatus
   EEPROM.write(138,0x03);   //button 1 Func LEDStatus
   EEPROM.write(139,0x03);   //button 1 Func LEDStatus
   EEPROM.write(140,0x03);   //button 1 Func LEDStatus
   EEPROM.write(141,0x03);   //button 1 Func LEDStatus
//int buttonPush2FuncByte5[buttonQuantity] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //message 5th byte
   EEPROM.write(142,0x00);   //button 1 Func LEDStatus
   EEPROM.write(143,0x00);   //button 1 Func LEDStatus
   EEPROM.write(144,0x00);   //button 1 Func LEDStatus
   EEPROM.write(145,0x00);   //button 1 Func LEDStatus
   EEPROM.write(146,0x00);   //button 1 Func LEDStatus
   EEPROM.write(147,0x00);   //button 1 Func LEDStatus
   EEPROM.write(148,0x00);   //button 1 Func LEDStatus
   EEPROM.write(149,0x00);   //button 1 Func LEDStatus
//int buttonPush2FuncByte6[buttonQuantity] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //message 6th byte
   EEPROM.write(150,0x00);   //button 1 Func LEDStatus
   EEPROM.write(151,0x00);   //button 1 Func LEDStatus
   EEPROM.write(152,0x00);   //button 1 Func LEDStatus
   EEPROM.write(153,0x00);   //button 1 Func LEDStatus
   EEPROM.write(154,0x00);   //button 1 Func LEDStatus
   EEPROM.write(155,0x00);   //button 1 Func LEDStatus
   EEPROM.write(156,0x00);   //button 1 Func LEDStatus
   EEPROM.write(157,0x00);   //button 1 Func LEDStatus
//int buttonPush2FuncByte7[buttonQuantity] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}; //message 7th byte 165
   EEPROM.write(158,0xff);   //button 1 Func LEDStatus
   EEPROM.write(159,0xff);   //button 1 Func LEDStatus
   EEPROM.write(160,0xff);   //button 1 Func LEDStatus
   EEPROM.write(161,0xff);   //button 1 Func LEDStatus
   EEPROM.write(162,0xff);   //button 1 Func LEDStatus
   EEPROM.write(163,0xff);   //button 1 Func LEDStatus
   EEPROM.write(164,0xff);   //button 1 Func LEDStatus
   EEPROM.write(165,0xff);   //button 1 Func LEDStatus

// int buttonPush3FuncLEDStatus[buttonQuantity] = {1,0,0,0,0,0,0,0}; //LED ON - 0; led OFF - 1 //166
   EEPROM.write(166,0x01);   //button 1 Func LEDStatus
   EEPROM.write(167,0x00);   //button 1 Func LEDStatus
   EEPROM.write(168,0x00);   //button 1 Func LEDStatus
   EEPROM.write(169,0x00);   //button 1 Func LEDStatus
   EEPROM.write(170,0x00);   //button 1 Func LEDStatus
   EEPROM.write(171,0x00);   //button 1 Func LEDStatus
   EEPROM.write(172,0x00);   //button 1 Func LEDStatus
   EEPROM.write(173,0x00);   //button 1 Func LEDStatus
//int buttonPush3FuncByte1[buttonQuantity] = {0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C}; //message Type
   EEPROM.write(174,0x1C);   //button 1 Func LEDStatus
   EEPROM.write(175,0x1C);   //button 1 Func LEDStatus
   EEPROM.write(176,0x1C);   //button 1 Func LEDStatus
   EEPROM.write(177,0x1C);   //button 1 Func LEDStatus
   EEPROM.write(178,0x1C);   //button 1 Func LEDStatus
   EEPROM.write(179,0x1C);   //button 1 Func LEDStatus
   EEPROM.write(180,0x1C);   //button 1 Func LEDStatus
   EEPROM.write(181,0x1C);   //button 1 Func LEDStatus
//int buttonPush3FuncByte2[buttonQuantity] = {0x02,0x03,0x03,0x03,0x03,0x03,0x03,0x03}; //message Area
   EEPROM.write(182,0x02);   //button 1 Func LEDStatus
   EEPROM.write(183,0x03);   //button 1 Func LEDStatus
   EEPROM.write(184,0x03);   //button 1 Func LEDStatus
   EEPROM.write(185,0x03);   //button 1 Func LEDStatus
   EEPROM.write(186,0x03);   //button 1 Func LEDStatus
   EEPROM.write(187,0x03);   //button 1 Func LEDStatus
   EEPROM.write(188,0x03);   //button 1 Func LEDStatus
   EEPROM.write(189,0x03);   //button 1 Func LEDStatus
//int buttonPush3FuncByte3[buttonQuantity] = {0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //message 3rd byte
   EEPROM.write(190,0xff);   //button 1 Func LEDStatus
   EEPROM.write(191,0x00);   //button 1 Func LEDStatus
   EEPROM.write(192,0x00);   //button 1 Func LEDStatus
   EEPROM.write(193,0x00);   //button 1 Func LEDStatus
   EEPROM.write(194,0x00);   //button 1 Func LEDStatus
   EEPROM.write(195,0x00);   //button 1 Func LEDStatus
   EEPROM.write(196,0x00);   //button 1 Func LEDStatus
   EEPROM.write(197,0x00);   //button 1 Func LEDStatus
//int buttonPush3FuncByte4[buttonQuantity] = {0x79,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //message Opcode
   EEPROM.write(198,0x79);   //button 1 Func LEDStatus
   EEPROM.write(199,0x00);   //button 1 Func LEDStatus
   EEPROM.write(200,0x00);   //button 1 Func LEDStatus
   EEPROM.write(201,0x00);   //button 1 Func LEDStatus
   EEPROM.write(202,0x00);   //button 1 Func LEDStatus
   EEPROM.write(203,0x00);   //button 1 Func LEDStatus
   EEPROM.write(204,0x00);   //button 1 Func LEDStatus
   EEPROM.write(205,0x00);   //button 1 Func LEDStatus
//int buttonPush3FuncByte5[buttonQuantity] = {0x64,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //message 5th byte
   EEPROM.write(206,0x64);   //button 1 Func LEDStatus
   EEPROM.write(207,0x00);   //button 1 Func LEDStatus
   EEPROM.write(208,0x00);   //button 1 Func LEDStatus
   EEPROM.write(209,0x00);   //button 1 Func LEDStatus
   EEPROM.write(210,0x00);   //button 1 Func LEDStatus
   EEPROM.write(211,0x00);   //button 1 Func LEDStatus
   EEPROM.write(212,0x00);   //button 1 Func LEDStatus
   EEPROM.write(213,0x00);   //button 1 Func LEDStatus
//int buttonPush3FuncByte6[buttonQuantity] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //message 6th byte
   EEPROM.write(214,0x00);   //button 1 Func LEDStatus
   EEPROM.write(215,0x00);   //button 1 Func LEDStatus
   EEPROM.write(216,0x00);   //button 1 Func LEDStatus
   EEPROM.write(217,0x00);   //button 1 Func LEDStatus
   EEPROM.write(218,0x00);   //button 1 Func LEDStatus
   EEPROM.write(219,0x00);   //button 1 Func LEDStatus
   EEPROM.write(220,0x00);   //button 1 Func LEDStatus
   EEPROM.write(221,0x00);   //button 1 Func LEDStatus
//int buttonPush3FuncByte7[buttonQuantity] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}; //message 7th byte 229
   EEPROM.write(222,0xff);   //button 1 Func LEDStatus
   EEPROM.write(223,0xff);   //button 1 Func LEDStatus
   EEPROM.write(224,0xff);   //button 1 Func LEDStatus
   EEPROM.write(225,0xff);   //button 1 Func LEDStatus
   EEPROM.write(226,0xff);   //button 1 Func LEDStatus
   EEPROM.write(227,0xff);   //button 1 Func LEDStatus
   EEPROM.write(228,0xff);   //button 1 Func LEDStatus
   EEPROM.write(229,0xff);   //button 1 Func LEDStatus
   
   //Button Push Function 4 (relaes after second push toggle)
//int buttonPush4FuncLEDStatus[buttonQuantity] = {1,0,0,0,0,0,0,0}; //LED ON - 0; led OFF - 1  230
   EEPROM.write(230,0x00);   //button 1 Func LEDStatus
   EEPROM.write(231,0x00);   //button 1 Func LEDStatus
   EEPROM.write(232,0x00);   //button 1 Func LEDStatus
   EEPROM.write(233,0x00);   //button 1 Func LEDStatus
   EEPROM.write(234,0x00);   //button 1 Func LEDStatus
   EEPROM.write(235,0x00);   //button 1 Func LEDStatus
   EEPROM.write(236,0x00);   //button 1 Func LEDStatus
   EEPROM.write(237,0x00);   //button 1 Func LEDStatus
//int buttonPush4FuncByte1[buttonQuantity] = {0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C}; //message Type
   EEPROM.write(238,0x1C);   //button 1 Func LEDStatus
   EEPROM.write(239,0x1C);   //button 1 Func LEDStatus
   EEPROM.write(240,0x1C);   //button 1 Func LEDStatus
   EEPROM.write(241,0x1C);   //button 1 Func LEDStatus
   EEPROM.write(242,0x1C);   //button 1 Func LEDStatus
   EEPROM.write(243,0x1C);   //button 1 Func LEDStatus
   EEPROM.write(244,0x1C);   //button 1 Func LEDStatus
   EEPROM.write(245,0x1C);   //button 1 Func LEDStatus
//int buttonPush4FuncByte2[buttonQuantity] = {0x02,0x03,0x03,0x03,0x03,0x03,0x03,0x03}; //message Area
   EEPROM.write(246,0x01);   //button 1 Func LEDStatus
   EEPROM.write(247,0x01);   //button 1 Func LEDStatus
   EEPROM.write(248,0x01);   //button 1 Func LEDStatus
   EEPROM.write(249,0x01);   //button 1 Func LEDStatus
   EEPROM.write(250,0x01);   //button 1 Func LEDStatus
   EEPROM.write(251,0x01);   //button 1 Func LEDStatus
   EEPROM.write(252,0x01);   //button 1 Func LEDStatus
   EEPROM.write(253,0x01);   //button 1 Func LEDStatus
//int buttonPush4FuncByte3[buttonQuantity] = {0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //message 3rd byte
   EEPROM.write(254,0x00);   //button 1 Func LEDStatus
   EEPROM.write(255,0x00);   //button 1 Func LEDStatus
   EEPROM.write(256,0x00);   //button 1 Func LEDStatus
   EEPROM.write(257,0x00);   //button 1 Func LEDStatus
   EEPROM.write(258,0x00);   //button 1 Func LEDStatus
   EEPROM.write(259,0x00);   //button 1 Func LEDStatus
   EEPROM.write(260,0x00);   //button 1 Func LEDStatus
   EEPROM.write(261,0x00);   //button 1 Func LEDStatus
//int buttonPush4FuncByte4[buttonQuantity] = {0x76,0x03,0x03,0x03,0x03,0x03,0x03,0x03}; //message Opcode
   EEPROM.write(262,0x03);   //button 1 Func LEDStatus
   EEPROM.write(263,0x03);   //button 1 Func LEDStatus
   EEPROM.write(264,0x03);   //button 1 Func LEDStatus
   EEPROM.write(265,0x03);   //button 1 Func LEDStatus
   EEPROM.write(266,0x03);   //button 1 Func LEDStatus
   EEPROM.write(267,0x03);   //button 1 Func LEDStatus
   EEPROM.write(268,0x03);   //button 1 Func LEDStatus
   EEPROM.write(269,0x03);   //button 1 Func LEDStatus
//int buttonPush4FuncByte5[buttonQuantity] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //message 5th byte
   EEPROM.write(270,0x00);   //button 1 Func LEDStatus
   EEPROM.write(271,0x00);   //button 1 Func LEDStatus
   EEPROM.write(272,0x00);   //button 1 Func LEDStatus
   EEPROM.write(273,0x00);   //button 1 Func LEDStatus
   EEPROM.write(274,0x00);   //button 1 Func LEDStatus
   EEPROM.write(275,0x00);   //button 1 Func LEDStatus
   EEPROM.write(276,0x00);   //button 1 Func LEDStatus
   EEPROM.write(277,0x00);   //button 1 Func LEDStatus
//int buttonPush4FuncByte6[buttonQuantity] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //message 6th byte
   EEPROM.write(278,0x00);   //button 1 Func LEDStatus
   EEPROM.write(279,0x00);   //button 1 Func LEDStatus
   EEPROM.write(280,0x00);   //button 1 Func LEDStatus
   EEPROM.write(281,0x00);   //button 1 Func LEDStatus
   EEPROM.write(282,0x00);   //button 1 Func LEDStatus
   EEPROM.write(283,0x00);   //button 1 Func LEDStatus
   EEPROM.write(284,0x00);   //button 1 Func LEDStatus
   EEPROM.write(285,0x00);   //button 1 Func LEDStatus
//int buttonPush4FuncByte7[buttonQuantity] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}; //message 7th byte
   EEPROM.write(286,0xff);   //button 1 Func LEDStatus
   EEPROM.write(287,0xff);   //button 1 Func LEDStatus
   EEPROM.write(288,0xff);   //button 1 Func LEDStatus
   EEPROM.write(289,0xff);   //button 1 Func LEDStatus
   EEPROM.write(290,0xff);   //button 1 Func LEDStatus
   EEPROM.write(291,0xff);   //button 1 Func LEDStatus
   EEPROM.write(292,0xff);   //button 1 Func LEDStatus
   EEPROM.write(293,0xff);   //button 1 Func LEDStatus
}



void setup() { 
//if(EEPROM.read(0) == 1){ //save configuration settings once
  defaultConfigSettings();
//}
 
Serial.begin(comPortSpeed);

pinMode(comBut,OUTPUT);//button common pin
digitalWrite(comBut, 0);
pinMode(comLED,OUTPUT);//LED common pin
digitalWrite(comLED, 1);

delay(10);
//sends device box numer on a start
deviceBoxNumber();

}




//MINE LOOP------------------------------------------------------------
void loop() {
//Serial.println(EEPROM.read(13));
 if(!progModeEnabled){ 
  
        ledStatus();
      
        //Button Scan 
        delayButtonScan++;
        if(delayButtonScan >= (20 + (5 * buttonScanNumber)))
        {     
          buttonScan(buttonScanNumber); //check if button pressed   
          buttonScanNumber ++; //next button     
        }else if(delayButtonScan > buttonScanReset)// && buttonScanNumber = buttonQuantity)
        { 
          delayButtonScan = 0;  //start again
          buttonScanNumber = 0; //start from first button  
        }

 }else{
      deviceProgramMode();
      
}

 
   //check if any message arrived
   netMessageReceive();
   
}
//********************************************************************


void ledStatus(){   
  digitalWrite(comBut, 1);
  int buttonNum = 0;  
  do{ pinMode(buttonPin[buttonNum],OUTPUT); //button 2 pin 
      digitalWrite(buttonPin[buttonNum], buttonLEDStatus[buttonNum]); // button disable output      
      buttonNum++;
   }while (buttonNum <= buttonQuantity);
}



//button action function------------------------------
void buttonScan(int buttonNum){
  
  digitalWrite(comBut, 0); //scan button mode
  pinMode(buttonPin[buttonNum],INPUT); //button pin in to the input mode
  digitalWrite(buttonPin[buttonNum], 1); // button disable output 
  
  switch (buttonFunction[buttonNum]) { 
       case 0://Button Disabled
          if(digitalRead(buttonPin[buttonNum]) == 0){
             pinMode(buttonPin[buttonNum],OUTPUT); //button dispabled
            }
       break;
    
      case 1://Push Once Function
          if(digitalRead(buttonPin[buttonNum]) == 0){
               buttonLEDStatus[buttonNum] = buttonPush1FuncLEDStatus[buttonNum];
              //message Type, Area ,3rd byte, Opcode, 5th byte, 6th byte, 7th byte
              netMessageSend(buttonPush1FuncByte1[buttonNum], buttonPush1FuncByte2[buttonNum], buttonPush1FuncByte3[buttonNum], buttonPush1FuncByte4[buttonNum], buttonPush1FuncByte5[buttonNum], buttonPush1FuncByte6[buttonNum], buttonPush1FuncByte7[buttonNum]);
         //    Serial.print(" << Button " + String(buttonNum) + " ON >> ");
            }
       break;
          
       case 2://Toggle Function
          if(digitalRead(buttonPin[buttonNum]) ==  0 && buttonToggleStatus[buttonNum] == 0){       
            buttonToggleStatus[buttonNum] = 1;      //Statuse 1                                   
            buttonLEDStatus[buttonNum] = buttonPush1FuncLEDStatus[buttonNum];                                             
            netMessageSend(buttonPush1FuncByte1[buttonNum], buttonPush1FuncByte2[buttonNum], buttonPush1FuncByte3[buttonNum], buttonPush1FuncByte4[buttonNum], buttonPush1FuncByte5[buttonNum], buttonPush1FuncByte6[buttonNum], buttonPush1FuncByte7[buttonNum]);                                
       //     Serial.print(" << Button toggle " + String(buttonNum) + " ON >> ");
          }else if(digitalRead(buttonPin[buttonNum]) ==  0 && buttonToggleStatus[buttonNum] == 1){                       
            buttonToggleStatus[buttonNum] = 0;      //Statuse 2                                   
            buttonLEDStatus[buttonNum] = buttonPush2FuncLEDStatus[buttonNum];                                             
            netMessageSend(buttonPush2FuncByte1[buttonNum], buttonPush2FuncByte2[buttonNum], buttonPush2FuncByte3[buttonNum], buttonPush2FuncByte4[buttonNum], buttonPush2FuncByte5[buttonNum], buttonPush2FuncByte6[buttonNum], buttonPush2FuncByte7[buttonNum]);
        //    Serial.print(" << Button toggle " + String(buttonNum) + " OFF >> ");
           }
        break;  

        case 3://Push With Release 
          if(digitalRead(buttonPin[buttonNum]) == 0 && buttonReleaseStatus[buttonNum] == 0){ //pushed                              
            buttonLEDStatus[buttonNum] = buttonPush1FuncLEDStatus[buttonNum];
            buttonReleaseStatus[buttonNum] = 1;      //Pushed                   
            netMessageSend(buttonPush1FuncByte1[buttonNum], buttonPush1FuncByte2[buttonNum], buttonPush1FuncByte3[buttonNum], buttonPush1FuncByte4[buttonNum], buttonPush1FuncByte5[buttonNum], buttonPush1FuncByte6[buttonNum], buttonPush1FuncByte7[buttonNum]);     
       //     Serial.print(" << Button with release " + String(buttonNum) + " Ramp UP >> ");
          }else if(digitalRead(buttonPin[buttonNum]) == 1 && buttonReleaseStatus[buttonNum] == 1){  //Released               
            buttonLEDStatus[buttonNum] = buttonPush2FuncLEDStatus[buttonNum];
            buttonReleaseStatus[buttonNum] = 0;      //Pushed                     
            netMessageSend(buttonPush2FuncByte1[buttonNum], buttonPush2FuncByte2[buttonNum], buttonPush2FuncByte3[buttonNum], buttonPush2FuncByte4[buttonNum], buttonPush2FuncByte5[buttonNum], buttonPush2FuncByte6[buttonNum], buttonPush2FuncByte7[buttonNum]);   
       //     Serial.print(" << Release " + String(buttonNum) + " Dim Stop >> ");
          }
         break;
         
         case 4://Toggle With Release                             
          //Pushed First Time
          if(digitalRead(buttonPin[buttonNum]) == 0 && buttonReleaseStatus[buttonNum] == 0 && buttonToggleStatus[buttonNum] == 0){ //pushed                              
            buttonLEDStatus[buttonNum] = buttonPush1FuncLEDStatus[buttonNum];
            buttonReleaseStatus[buttonNum] = 1;      //Pushed    
            buttonToggleStatus[buttonNum] = 0;      //Pushed                              
            netMessageSend(buttonPush1FuncByte1[buttonNum], buttonPush1FuncByte2[buttonNum], buttonPush1FuncByte3[buttonNum], buttonPush1FuncByte4[buttonNum], buttonPush1FuncByte5[buttonNum], buttonPush1FuncByte6[buttonNum], buttonPush1FuncByte7[buttonNum]);     
       //     Serial.print(" << Button with release " + String(buttonNum) + " Ramp UP >> ");      
          }else if(digitalRead(buttonPin[buttonNum]) == 1 && buttonReleaseStatus[buttonNum] == 1 && buttonToggleStatus[buttonNum] == 0){  //Released               
            buttonLEDStatus[buttonNum] = buttonPush2FuncLEDStatus[buttonNum];
            buttonReleaseStatus[buttonNum] = 0;      //Pushed     
            buttonToggleStatus[buttonNum] = 1;      //Pushed                    
            netMessageSend(buttonPush2FuncByte1[buttonNum], buttonPush2FuncByte2[buttonNum], buttonPush2FuncByte3[buttonNum], buttonPush2FuncByte4[buttonNum], buttonPush2FuncByte5[buttonNum], buttonPush2FuncByte6[buttonNum], buttonPush2FuncByte7[buttonNum]);   
      //      Serial.print(" << Release " + String(buttonNum) + " Dim Stop 1 >> ");     
          }
          //Pushed Second Time
          if(digitalRead(buttonPin[buttonNum]) == 0 && buttonReleaseStatus[buttonNum] == 0 && buttonToggleStatus[buttonNum] == 1){ //pushed                              
            buttonLEDStatus[buttonNum] = buttonPush3FuncLEDStatus[buttonNum];
            buttonReleaseStatus[buttonNum] = 1;      //Pushed    
            buttonToggleStatus[buttonNum] = 1;      //Pushed                              
            netMessageSend(buttonPush3FuncByte1[buttonNum], buttonPush3FuncByte2[buttonNum], buttonPush3FuncByte3[buttonNum], buttonPush3FuncByte4[buttonNum], buttonPush3FuncByte5[buttonNum], buttonPush3FuncByte6[buttonNum], buttonPush3FuncByte7[buttonNum]);     
     //       Serial.print(" << Button with release " + String(buttonNum) + " Ramp DOWN >> ");      
          }else if(digitalRead(buttonPin[buttonNum]) == 1 && buttonReleaseStatus[buttonNum] == 1 && buttonToggleStatus[buttonNum] == 1){  //Released               
            buttonLEDStatus[buttonNum] = buttonPush4FuncLEDStatus[buttonNum];
            buttonReleaseStatus[buttonNum] = 0;      //Pushed     
            buttonToggleStatus[buttonNum] = 0;      //Pushed                    
            netMessageSend(buttonPush4FuncByte1[buttonNum], buttonPush4FuncByte2[buttonNum], buttonPush4FuncByte3[buttonNum], buttonPush4FuncByte4[buttonNum], buttonPush4FuncByte5[buttonNum], buttonPush4FuncByte6[buttonNum], buttonPush4FuncByte7[buttonNum]);   
      //      Serial.print(" << Release " + String(buttonNum) + " Dim Stop 2>> ");   
          }
         break;    
  }//switch ends 
      if(digitalRead(buttonPin[buttonNum])==0){longPushDeviceBoxNumber(buttonNum);}//reset        
}
//*******************************************


//Send Command over RS485 network by Dynet1 protocol-------------------------------------------------------
void netMessageSend(byte commandType, byte areaCode, byte data1, byte opCode, byte data2, byte data3, byte joinCode){

  unsigned int checkSum = commandType + areaCode + data1 + opCode + data2 + data3 + joinCode;

  digitalWrite(pinComStatus, 1);

    Serial.flush();
    Serial.write(commandType);
    Serial.write(areaCode);
    Serial.write(data1);
    Serial.write(opCode);
    Serial.write(data2);
    Serial.write(data3);
    Serial.write(joinCode);
    Serial.write(0x200-checkSum);

  while (!(UCSR0A & (1 << TXC0)));

  digitalWrite(pinComStatus, 0);  //Complete the transmition
}
//*****************************************************************************************************



//---Serial port receive function-------------------------------------------------------
void netMessageReceive(){
 digitalWrite(pinComStatus,0);//set com port to listening mode

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
              
                     logicalMessageChecking(serialData[1],serialData[2],serialData[3],serialData[4],serialData[5],serialData[6]);

              break;


                    
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
                               
                              case 0x03: //read EEPROM      //address serialData[4] serialData[5]           //  data serialData[6]                        
                                    delay(delayTransmition);
                                    netMessageSend(0x5C,deviceType,deviceNumber,0x05,EEPROM.read(serialData[4]),EEPROM.read(serialData[5]),EEPROM.read(serialData[6]));
                              break; 
                      }                    
                    }                 
                break;
             }
    }       
  }
}
//*********************************************************************************


void resetDevice(){asm volatile ("  jmp 0");}  // Restarts device


//SERVICE Network commands----------------------------------------------------------------------------------------------------
void deviceProgramMode(){
flashLEDProgMode++;

digitalWrite(comBut, 1);
int buttonNum = 0; 
  Serial.print(" <<  " + String(flashLEDProgMode) + " >> ");  
 
      if(flashLEDProgMode == 1){ 
        do{ pinMode(buttonPin[buttonNum],OUTPUT); 
            digitalWrite(buttonPin[buttonNum], 0); // leds on      
            buttonNum++;
         }while (buttonNum <= buttonQuantity);
      }
      
      if(flashLEDProgMode == 10){ 
        do{ pinMode(buttonPin[buttonNum],OUTPUT);  
            digitalWrite(buttonPin[buttonNum], 1); // leds off    
            buttonNum++;
         }while (buttonNum <= buttonQuantity);
      } 
      
      if(flashLEDProgMode == 18){ 
        do{ pinMode(buttonPin[buttonNum],OUTPUT); 
            digitalWrite(buttonPin[buttonNum], 0); // leds on    
            buttonNum++;
         }while (buttonNum <= buttonQuantity);
      }
      
      if(flashLEDProgMode == 28){ 
        do{ pinMode(buttonPin[buttonNum],OUTPUT);     
            digitalWrite(buttonPin[buttonNum], 1); // leds off   
            buttonNum++;
         }while (buttonNum <= buttonQuantity);
      }
      
      if(flashLEDProgMode == 36){ 
        do{ pinMode(buttonPin[buttonNum],OUTPUT); 
            digitalWrite(buttonPin[buttonNum], 0); // leds on    
            buttonNum++;
         }while (buttonNum <= buttonQuantity);
      }
      
      if(flashLEDProgMode == 46){ 
        do{ pinMode(buttonPin[buttonNum],OUTPUT); 
            digitalWrite(buttonPin[buttonNum], 1); // leds off     
            buttonNum++;
         }while (buttonNum <= buttonQuantity);
      }

if(flashLEDProgMode >= 150){ flashLEDProgMode = 0;}
}
//****************************************************************************************************************************


void longPushDeviceBoxNumber(int buttonNum){
int count = 0;
    do{
      count ++;
          if( count > 2500){
            deviceBoxNumber();
            count = 0;
            }
            delay(3);
      }while (digitalRead(buttonPin[buttonNum])==0); 
}


//SERVICE Network commands------//5C 80 63 00 02 19 00 A6 Device UPAN9 (0x80) Box 99 v02.19, Device Identify  In  UPAN9 (0x80), Box: 99
void deviceBoxNumber(){netMessageSend(0x5C,deviceType,deviceNumber,0x00,deviceFWgeneration,deviceFWvar,0x00);}
//****************************************************************************************************************************


void logicalMessageChecking(byte Area, byte Fade, byte Preset, byte Data1, byte Data2, byte Join ){
//serialData[1],serialData[2],serialData[3],serialData[4],serialData[5],serialData[6]
//1C  serialdata[0] message type, [1] logical area, [2] fade time, [3] scene number, [4] message type, [5] message type, [6] join

  int buttonNum = 0;  
  do{  
        if(buttonFunction[buttonNum] != 0){ //is button active?
          //First Button
           if(buttonPush1FuncByte1[buttonNum] == 0x1C){ //is it logical message
                if (buttonPush1FuncByte2[buttonNum] == Area){//is it button programmed to this area?
                    if (buttonPush1FuncByte4[buttonNum] == Preset){//is it same preset?
                        buttonPush1FuncLEDStatus[buttonNum] = 0; //LED ON - 0; led OFF - 1
                        buttonToggleStatus[buttonNum] = 1; //Not Pushed - 0; Pushed - 1
                    }
                }
           }
        }
     // digitalWrite(buttonPin[buttonNum], buttonLEDStatus[buttonNum]); // button disable output      
      buttonNum++;
   }while (buttonNum <= buttonQuantity);


  
}

