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

#define CYTRON_SHIELD_LIBRARY_VERSION "2.0.0"

#include <Arduino.h>
#include <Stream.h>
#ifdef __AVR__
#include <SoftwareSerial.h>
#endif

#define custom_map(x, in_min, in_max, out_min, out_max) (int32_t)(round(((float)(x - in_min) * (float)(out_max - out_min) / (float)(in_max - in_min)) + out_min))

typedef enum
{
	POS_START_CMD = 4, //	0b100
	POS_REPORT_CMD,	//	0b101
	ACTIVATE_CHNL_CMD, //	0b110
	POS_SPEED_CMD	  //	0b111
} servo_command_t;

typedef enum
{
	CYTRON_SHIELD_SERVO_CHECKSUM_ERROR = -2,
	CYTRON_SHIELD_SERVO_TIMEOUT_ERROR,
	CYTRON_SHIELD_SERVO_OK
} servo_status_t;

typedef enum
{
	OFF = 0,
	ON
} servo_pwr_t;

typedef enum
{
	ALL_CHANNELS = 0,
	CHANNEL_1,
	CHANNEL_2,
	CHANNEL_3,
	CHANNEL_4,
	CHANNEL_5,
	CHANNEL_6,
	CHANNEL_7,
	CHANNEL_8
} servo_channel_t;

#define CYTRON_SHIELD_SERVO_TIMEOUT 100
#define CYTRON_SHIELD_180_UPPER_CAP (8000 - 740)
#define CYTRON_SHIELD_180_LOWER_CAP (0 + 156)

class CytronServoShield
{
	public:
		CytronServoShield();
#ifdef __AVR__
		void init(HardwareSerial *serial);
		void init(SoftwareSerial *serial, void (*ptr)()=NULL);
#else
		void init(Stream *serial, void (*ptr)() = NULL);
#endif

		void setChannel(servo_channel_t chnl, servo_pwr_t on);
		void position(servo_channel_t chnl, uint16_t pos, uint8_t spd = 0);
		void angle(servo_channel_t chnl, uint8_t angle, uint8_t spd = 0)
		{
			position(chnl, custom_map(angle, 0, 180, CYTRON_SHIELD_180_LOWER_CAP, CYTRON_SHIELD_180_UPPER_CAP), spd);
		}
		uint16_t getPosition(servo_channel_t chnl);
		void setStartPosition(servo_channel_t chnl, uint16_t pos);
		uint8_t getAngle(servo_channel_t chnl)
		{
			return custom_map(getPosition(chnl), CYTRON_SHIELD_180_LOWER_CAP, CYTRON_SHIELD_180_UPPER_CAP, 0, 180);
		}
		void setStartAngle(servo_channel_t chnl, uint8_t angle)
		{
			setStartPosition(chnl, custom_map(angle, 0, 180, CYTRON_SHIELD_180_LOWER_CAP, CYTRON_SHIELD_180_UPPER_CAP));
		}
		void registerErrorCallback(void (*cb)(uint8_t, servo_status_t));

	protected:
		Stream *_serial;

	private:
		void (*errorCallback)(uint8_t, servo_status_t);
#ifdef __AVR__
		void (*_listen)(SoftwareSerial *);
#else
		void (*_listen)();
#endif
		servo_status_t getResponseFromCmd(uint8_t *buf, uint16_t len);
		servo_status_t getResponseFromCmdUntil(uint8_t terminator);
		void flush();
};

#endif
