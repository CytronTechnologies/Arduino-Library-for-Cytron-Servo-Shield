/*
 *  This sketch shows example of using 2 Cytron Servo Shield simultaneously
 *  to control more servos.
 *
 *  You can check this shield out at
 *	https://www.cytron.io/p-shield-servo/
 *
 *  The Arduino library can be downloaded from 
 *	https://github.com/CytronTechnologies/Cytron_Servo_Shield_Library
 *
 *  modified 14 June 2018
 *  by Ng Beng Chet
 *  
 */

#include <SoftwareSerial.h>
#include <CytronServoShield.h>

SoftwareSerial swSerial1(2, 3);
SoftwareSerial swSerial2(10, 11);
CytronServoShield servo1;
CytronServoShield servo2;

void setup()
{
  //  Initialize serial communication for UART with baudrate 9.6kbps
  Serial.begin(9600); 
  //  for Leonardo only
  while(!Serial);

  //  Initialize servo shield
  swSerial1.begin(9600);
  servo1.init(&swSerial1);
  //  For boards in which compiler is not avr-gcc, and SoftwareSerial is used and listen() function is available
  //  use the function below instead to init servo shield
  //  servo1.init(&swSerial1, []{ swSerial1.listen() });
  swSerial2.begin(9600);
  servo2.init(&swSerial2);
  
  Serial.println(F("Cytron Servo Shield Multiple Shields Demo"));

  //  Activate all servo motor channels on all servo shields
  servo1.setChannel(ALL_CHANNELS, ON);
  servo2.setChannel(ALL_CHANNELS, ON);

  //  Set servos attached to shield 1 to move to position 6000
  //  servos attached to shield 2 to move to position 1000
  for (uint8_t channel = 1; channel < 9; channel++)   
  { 
    servo1.position(channel, 6000);
    servo2.position(channel, 1000);
  }

  delay(2000);

  //  Set servos attached to shield 1 to move to position 1000
  //  servos attached to shield 2 to move to position 6000
  for (uint8_t channel = 1; channel < 9; channel++)
  {
    servo1.position(channel, 1000);
    servo2.position(channel, 6000);
  }

  Serial.println(F("Demo completed!"));
}

void loop()
{
  /* do nothing */
}

