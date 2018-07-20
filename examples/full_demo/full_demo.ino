/*
 *  This sketch shows all possible functions of using Cytron Servo Shield.
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

SoftwareSerial mySerial(2, 3); // ON_BOARD_TX, ON_BOARD_RX
CytronServoShield servo;

void callback(uint8_t servoId, servo_status_t status) {
  String error="";
  switch(status) {
    case CYTRON_SHIELD_SERVO_CHECKSUM_ERROR:
      error = "Checksum error";
      break;
    case CYTRON_SHIELD_SERVO_TIMEOUT_ERROR:
      error = "Timeout error";
      break;
    default:
      error = "Unknown error";
  }
  Serial.println(error + " from servo channel " + String(servoId) + " !");
}

void setup()
{
  //  Debug output from Serial Monitor with baudrate 9.6kbps
  Serial.begin(9600); 

  while(!Serial);
  
  Serial.println(F("Cytron Servo Shield Demo"));
  Serial.println(F("Make sure to set RX=D3 and TX=D2."));
  
  //  Initialize serial communication for UART with baudrate 9.6kbps
  mySerial.begin(9600);

  //  Initialize servo shield
  servo.init(&mySerial);

  //  Register error callback for troubleshooting purpose (optional)
  servo.registerErrorCallback(callback);

  // Set initial position for servo from all channels to 4000 (approximately 90 deg)
  // When next call servo.position or servo.angle, it will move to initial position/angle first
  // then move to target position/angle
  // You can either use setStartPosition or setStartAngle
  // by default, initial position/angle is 4000/90deg
  Serial.println(F("Set initial position to 0 deg for all channels."));
  servo.setStartAngle(ALL_CHANNELS, 0);
  // servo.setStartPosition(ALL_CHANNELS, CYTRON_SHIELD_180_LOWER_CAP);
  
  // Serial.println("Set initial position to 180 deg ...");
  // servo.setStartAngle(ALL_CHANNELS, 180);
  // servo.setStartPosition(ALL_CHANNELS, CYTRON_SHIELD_180_UPPER_CAP);
  
  // Set specific initial position/angle for single channel
  // eg:
  // Serial.println("Set channel 2 servo initial position to 45 deg ...");
  // servo.setStartAngle(CHANNEL_2, 45);

  //  Deactivate all servo motor channels on Shield servo, except for channel 1
  //  CHANNEL 1 - CHANNEL_8
  //  or use 1 - 8 as channels
  //  0 - OFF, 1 - ON
  Serial.println(F("Enable channel 1 only."));
  servo.setChannel(ALL_CHANNELS, OFF); 
  servo.setChannel(CHANNEL_1, ON);
  // servo.setChannel(1, 1);
  
  //  We set servo from Channel 1 to move to position 6000 (range 0 - 8000)
  //  We can also specify speed for servo movement (range 1 - 100, slowest - fastest)
  //  provide 0 to speed will cause servo to move instantly to target position

  //  Set position = 6000, speed = 25
  Serial.println(F("Move to position 6000 with speed 25."));
  servo.position(CHANNEL_1, 6000, 25); 
  // Wait until servo reaches around the target position (tolerance 100)
  while(servo.getPosition(CHANNEL_1) <= 6000 - 100);
  delay(1000);

  // Move to position 1000 instantly
  Serial.println(F("Move to position 1000 instantly."));
  servo.position(CHANNEL_1, 1000);
  // Wait until servo reaches around the target position
  while(servo.getPosition(CHANNEL_1) > 1000 + 100);
  delay(1000);

  // Now we try to move servo with angle input
  // We try to move servo to 180 deg with speed 100
  Serial.println(F("Move to angle 180 deg with speed 100."));
  servo.angle(CHANNEL_1, 180, 100);
  // Wait until servo reaches around the target position with tolerance 5 deg
  while(servo.getAngle(CHANNEL_1) <= 180 - 5);
  delay(1000);
  
  // We try to move servo to 0 deg instantly
  Serial.println(F("Move to angle 0 deg instantly."));
  servo.angle(CHANNEL_1, 0);
  // Wait until servo reaches around the target position
  while(servo.getAngle(CHANNEL_1) > 0 + 5);
  delay(1000);

  Serial.println(F("Demo completed!"));
  
}

//Main loop
void loop()
{
}

