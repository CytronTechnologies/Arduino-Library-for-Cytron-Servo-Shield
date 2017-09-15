#include <CytronServoShield.h>

SoftwareSerial mySerial(2, 3); // RX, TX
CytronServoShield servo;

//const char tx = 3;  //assign tx with pin 1 (transmit)
//const char rx = 2;  //assign rx with pin 0 (receive)
const char led_red = 13;  // assign led_red with pin 13 (there is an on-board red led)

// Uncomment this only if you are using multiple softwareSerial
/*void listen() {
  if (!mySerial.isListening())
    mySerial.listen();
}*/

//Setup
void setup()
{
  Serial.begin(9600); //initialize serial communication for UART with baudrate 9.6kbps
  mySerial.begin(9600);
  // set the digital pin as output:
  pinMode(led_red, OUTPUT);

  //Initialize servo shield
  // Use this if only one softwareSerial is used in this sketch
  servo.init(&mySerial);
  
  // Use this if multiple softwareSerial are used
  //servo.init(&mySerial, listen);

  Serial.println("Shield-Servo Example");

  servo.on_off_motor(ALL_CHANNELS, 1); //activate all servo motor channels on Shield servo
}

//Main loop
void loop()
{
  Serial.println("Make sure to set RX=D3 and TX=D2.");
  servo.on_off_motor(ALL_CHANNELS, 1); //activate all servo motor channels on Shield servo

  Serial.println("Check ALL Servo motor.");

  Serial.println("Servo move to Right. ");
  for (int count = 0; count < 8; count++) //set 1st position, speed for all servo motor channels (servo motors), assignable speed value (1-100)
  { //In position reporting operation, setting speed = 0 will cause the servo motor oscillates due to limitation of software position feedback
    servo.set_ch_pos_spd(count + 1, 6000, 50); //set position = 7000, speed = 50
  }
  digitalWrite(led_red, HIGH); //led is on (indicaton)
  while (servo.get_current_pos(1) < 5000); //wait untill the (position-100) is reached   (Optional)
  delay(1000);

  Serial.println("Servo move to Left.  ");
  for (int count = 0; count < 8; count++) //set 2nd position, speed for all servo motor channels (servo motors), assignable speed value (1-100)
  { //In position reporting operation, setting speed = 0 will cause the servo motor oscillates due to limitation of software position feedback
    servo.set_ch_pos_spd(count + 1, 1000, 50); //set position = 4000, speed = 50
  }

  digitalWrite(led_red, LOW); //led is off (indicaton)
  while (servo.get_current_pos(1) > 2000); //wait untill the (position + 100) is reached   (Optional)
  delay(1000);
}

