/******************************************************************************
CytronServoShield.cpp
Cytron Servo Shield Library Main Source File
Created by Ng Beng Chet @ Cytron Technologies Sdn Bhd 
Original Creation Date: Sept 15, 2017
https://github.com/CytronTechnologies/Arduino-Library-for-Cytron-Servo-Shield

!!! Description Here !!!

Development environment specifics:
	IDE: Arduino 1.8.1
	Hardware Platform: Arduino Uno
	Cytron Servo Shield Version: 1.0.0

Distributed as-is; no warranty is given.
******************************************************************************/

#include "CytronServoShield.h"

void (*CytronServoShield_listen)() = NULL;

CytronServoShield::CytronServoShield(){
}

// public

void CytronServoShield::init(Stream *serial, void (*ptr)()){
	_serial = serial;
	CytronServoShield_listen = ptr;
}

void CytronServoShield::on_off_motor(unsigned char ch, unsigned char on){
	/*****Activate servo channel command*****
	    - 2 bytes involved
	    - 1st byte: Mode + Servo motor channel
	    eg:
		Ob 1 1 0 x x x x x;
		  |Mode |Servo channel
	    -the 3 MSB bits (110) is mode to activate servo channels
	    -the last xxxxx can be assigned with value 0-16, where
	    0 ---> activate all channels
	    1 ---> activate channel 1
	    2 ---> activate channel 2
	    ....
	    ....
	    16 ---> activate channel 16
	    - 2nd byte: On/off
	    eg:
		0b 0 0 0 0 0 0 0 x
	    x = 1 --> on selected and activated channel/channels
	    x = 0 --> off selected or activated channel/channels
	  ****************************************/
	
	unsigned char first_byte = 0;

	first_byte = 0b11000000 | ch; //make up 1st byte
	_serial->write(first_byte); //send 1st byte use UART
	_serial->write(on); //send 2nd byte use UART
}

void CytronServoShield::set_ch_pos_spd(unsigned char ch, unsigned int pos, unsigned char spd){
	 /*****Position and Speed Command*****
	    - 4 bytes involved
	    - 1st byte: Mode + Servo motor channel
	    eg:
	      0b 1 1 1 x x x x x
		 Mode |Servo channel
	    - 3 MSBs (111) is mode for position and speed command
	    - the last xxxxx can be assigned with value 1-16, where
	    1 ---> select channel 1
	    2 ---> select channel 2
	    ....
	    ....
	    16 ---> select channel 16
	    - 2nd byte: Position (High byte) higher 7-bit
	    eg:
	      0b 0 x x x x x x x
	    - the last xxxxxxx can be assigned with value 0 - 127
	    - 3nd byte: Position (Low byte) lower 6-bit
	    eg:
	      0b 0 0 x x x x x x
	    - the last xxxxxx can be assigned with value 0 - 63
	    **2nd byte and 3byte is the position value when combined together into 13 bits position
	    - 4th byte: Speed (0-100)
	    eg:
	      0b 0 x x x x x x x
	    - the last xxxxxx can be assigned with value 0 - 100
	  ************************************/

	unsigned char first_byte = 0;
  	unsigned char high_byte = 0;
  	unsigned char low_byte = 0;

  	first_byte = 0b11100000 | ch; //make up the 1st byte
  	high_byte = (pos >> 6) & 0b01111111; //obtain the high byte of 13 bits position value
  	low_byte = pos & 0b00111111; //obtain the low byte of 13 bits position value
  	_serial->write(first_byte); //send the 1st byte
  	_serial->write(high_byte); //send the 2nd byte
  	_serial->write(low_byte); //send the 3rd byte
  	_serial->write(spd); // send the 4th byte
}

unsigned int CytronServoShield::get_current_pos(unsigned char ch){
	  /*****Servo position reporting Command*****
	    - 3 bytes involved (send 1 byte, received 2 bytes)
	    - 1st byte: Mode + Servo motor channel (Send)
	    eg:
	      0b 1 0 1 x x x x x
		 Mode |Servo channel
	    - 3 MSBs (101) is mode for position reporting command
	    - the last xxxxx can be assigned with value 1-16, where
	    1 ---> select channel 1
	    2 ---> select channel 2
	    ....
	    ....
	    16 ---> select channel 16
	    - 2nd byte: Position (High byte) higher 7-bit (1st received byte)
	    eg:
	      0b 0 x x x x x x x
	    - Received value vary between 0 - 127
	    - 3nd byte: Position (Low byte) lower 6-bit (2nd received byte)
	    eg:
	      0b 0 0 x x x x x x
	    - Received value vary between 0 - 63
	  **2nd byte and 3byte is the position value when combined together into 13 bits
	  ******************************************/
	
	unsigned char first_byte = 0;
  	unsigned char high_byte = 0;
  	unsigned char low_byte = 0;
  	unsigned int reading_position = 0;
  	unsigned char buffer[2];

	first_byte =  0b10100000 | ch; //make up the 1st byte

	CytronServoShield_listen();
	flush();
	_serial->write(first_byte);

	int status = getResponseFromCmd(buffer, sizeof(buffer));

	if(status == CYTRON_SHIELD_SERVO_OK){
		return buffer[0] << 6 | buffer[1]; 
	}
	else{
		return 0;
	}
}

void CytronServoShield::set_ch_initial_pos(unsigned char ch, unsigned int pos){
	/*****Servo starting position Command*****\
	    - 3 bytes involved
	    - 1st byte: Mode + Servo motor channel
	    eg:
	      0b 1 0 0 x x x x x
		 Mode |Servo channel
	    - 3 MSBs (111) is mode for position and speed command
	    - the last xxxxx can be assigned with value 1-16, where
	     1 ---> select channel 1
	     2 ---> select channel 2
	     ....
	     ....
	     16 ---> select channel 16
	    - 2nd byte: Position (High byte) higher 7-bit
	    eg:
	      0b 0 x x x x x x x
	    - the last xxxxxxx can be assigned with value 0 - 127
	    - 3nd byte: Position (Low byte) lower 6-bit
	    eg:
	      0b 0 0 x x x x x x
	    - the last xxxxxx can be assigned with value 0 - 63
	  **2nd byte and 3byte is the position value when combined together into 13 bits
	  *****************************************/
	unsigned char first_byte = 0;
  	unsigned char high_byte = 0;
  	unsigned char low_byte = 0;
  	
	first_byte = 0b10000000 | ch; //make up the 1st byte
	high_byte = (pos >> 6) & 0b01111111; //make up the high byte
  	low_byte = pos & 0b00111111; //make up the low byte

	_serial->write(first_byte);
	_serial->write(high_byte);
	_serial->write(low_byte);
	
	int status = getResponseFromCmdUntil(0x04);

	// do nothing to status yet
}

// private

int CytronServoShield::getResponseFromCmd(uint8_t *buf, uint16_t len){

	uint32_t timeout = millis();
	uint16_t _start = 0;
	while(millis() - timeout < CYTRON_SHIELD_SERVO_TIMEOUT){
		if(_serial->available()){
			buf[_start++] = _serial->read();
			timeout = millis();
			continue;
		}
		if(_start == len){
			return CYTRON_SHIELD_SERVO_OK;
		}
		delay(1);
	}
	if(_start > 0 && _start != len)
		return CYTRON_SHIELD_SERVO_DATA_ERROR;

	return CYTRON_SHIELD_SERVO_TIMEOUT_ERROR;
}

int CytronServoShield::getResponseFromCmdUntil(unsigned char terminator){

	uint32_t timeout = millis();
	uint16_t _start = 0;
	while(millis() - timeout < CYTRON_SHIELD_SERVO_TIMEOUT + 4800){ // 5s
		if(_serial->available()){
			uint8_t c = _serial->read();
			if(c == terminator)
				return CYTRON_SHIELD_SERVO_OK;
		}
		delay(1);
	}
	return CYTRON_SHIELD_SERVO_TIMEOUT_ERROR;
}

void CytronServoShield::flush(){
	while(_serial->available()){
		_serial->read();
	}
}	
