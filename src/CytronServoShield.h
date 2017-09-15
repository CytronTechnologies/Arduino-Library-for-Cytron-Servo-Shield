/******************************************************************************
CytronServoShield.h
Cytron Servo Shield Library Main Header File
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

#ifndef _CYTRON_SHIELD_SERVO_H_
#define _CYTRON_SHIELD_SERVO_H_

#include <Arduino.h>
#include <Stream.h>
#include <SoftwareSerial.h>

#define CYTRON_SHIELD_SERVO_TIMEOUT_ERROR -1
#define CYTRON_SHIELD_SERVO_OK 0
#define CYTRON_SHIELD_SERVO_DATA_ERROR -2
#define CYTRON_SHIELD_SERVO_TIMEOUT 200
#define ALL_CHANNELS 0

class CytronServoShield {

public:
	CytronServoShield();
	void init(Stream *serial, void (*ptr)()=[]{});
	void on_off_motor(unsigned char ch, unsigned char on);
	void set_ch_pos_spd(unsigned char ch, unsigned int pos, unsigned char spd);
	unsigned int get_current_pos(unsigned char ch);
	void set_ch_initial_pos(unsigned char ch, unsigned int pos);

private:
	Stream* _serial;
	int getResponseFromCmd(uint8_t *buf, uint16_t len);
	int getResponseFromCmdUntil(unsigned char terminator);
	void flush();
};

#endif
