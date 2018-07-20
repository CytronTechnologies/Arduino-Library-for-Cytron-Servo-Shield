/******************************************************************************
CytronServoShield.cpp
Cytron Servo Shield Library Main Source File
Created by Ng Beng Chet @ Cytron Technologies Sdn Bhd 
Original Creation Date: June 12, 2018
https://github.com/CytronTechnologies/Arduino-Library-for-Cytron-Servo-Shield

!!! Description Here !!!

Development environment specifics:
	IDE: Arduino 1.8.1
	Hardware Platform: Arduino Uno
	Cytron Servo Shield Version: 1.0.0

Distributed as-is; no warranty is given.
******************************************************************************/

#include "CytronServoShield.h"

CytronServoShield::CytronServoShield()
{
}

/**
 * public
 */

/**
 * @brief This function initialises UART communication between Arduino board and Servo Shield
 * 
 * @param[in] serial
 * 			-	Values: HardwareSerial Object or SoftwareSerial Object, see examples
 * 
 * @param[in] ptr (optional)
 * 			- Useful if multiple SoftwareSerial ports are used in single Arduino board
 * 			- can add listen() function, see examples
 * 
 * @return None
 */
#ifdef __AVR__
void CytronServoShield::init(HardwareSerial *serial)
{
	this->_serial = serial;
}

void CytronServoShield::init(SoftwareSerial *serial, void (*ptr)())
{
	this->_serial = serial;
	if (ptr)
	{
		this->_listen = ptr;
	}
	else
	{
		this->_listen = [](SoftwareSerial *s) {
			s->listen();
		};
	}
}
#else
void CytronServoShield::init(Stream *serial, void (*ptr)())
{
	this->_serial = serial;
	if (ptr)
	{
		this->_listen = ptr;
	}
}
#endif

/**
 * @brief This function initialises the channels
 * 
 * @param[in] chnl 
 * 			- Channels to be activated.
 * 			- Values:
 * 				- CHANNEL_1 to CHANNEL_8 for separate channels
 * 				- ALL_CHANNEL to activate all channels
 * 
 * @param[in] on 	
 * 			-	Activate or Deactivate the channel
 * 			- Values: ON or OFF
 * 
 * @return None
 */

void CytronServoShield::setChannel(servo_channel_t chnl, servo_pwr_t on)
{
	uint8_t first_byte = 0;

	if (chnl > 8)
		return;

	first_byte = ACTIVATE_CHNL_CMD << 5 | chnl; //make up 1st byte
	_serial->write(first_byte);					//send 1st byte use UART
	_serial->write(on);							//send 2nd byte use UART
}

/**
 * @brief This function allows servos to move to target position with desired speed
 * 
 * @param[in] chnl 
 * 			- Channel to be targeted.
 * 			- Value range: CHANNEL_1 to CHANNEL_8
 * 
 * @param[in] pos 	
 * 			-	Target position
 * 			- Value range: 0 - 8000
 * 
 * @param[in] spd	
 * 			-	Speed to reach target position
 * 			- Value range: 0 - 100, by default 0 (Full speed)
 * 				0: Full speed
 * 				1: Slowest
 * 				...
 * 				100: Fastest (but not full speed)
 * 
 * @return None
 */

void CytronServoShield::position(servo_channel_t chnl, uint16_t pos, uint8_t spd)
{
	uint8_t first_byte = 0;
	uint8_t high_byte = 0;
	uint8_t low_byte = 0;

	pos = (pos > 8000) ? 8000: pos;

	first_byte = POS_SPEED_CMD << 5 | chnl; //make up the 1st byte
	high_byte = (pos >> 6) & 0b01111111;	//obtain the 7 MSB of 13 bits position value
	low_byte = pos & 0b00111111;			//obtain the 6 LSB of 13 bits position value
	_serial->write(first_byte);				//send the 1st byte
	_serial->write(high_byte);				//send the 2nd byte
	_serial->write(low_byte);				//send the 3rd byte
	_serial->write(spd);					// send the 4th byte
}

/**
 * @brief This function get the current position from servos
 * 
 * @param[in] chnl 
 * 			- Channel to be targeted.
 * 			- Value range: CHANNEL_1 to CHANNEL_8
 * 
 * @return current position of servo corresponding to the channel
 */

uint16_t CytronServoShield::getPosition(servo_channel_t chnl)
{
	uint8_t first_byte = 0;
	uint8_t high_byte = 0;
	uint8_t low_byte = 0;
	uint8_t buffer[2];

	first_byte = POS_REPORT_CMD << 5 | chnl; //make up the 1st byte

	if (this->_listen)
#ifdef __AVR__
		this->_listen(this->_serial);
#else
		this->_listen();
#endif

	flush();

	_serial->write(first_byte);

	servo_status_t status = getResponseFromCmd(buffer, sizeof(buffer));

	if (status == CYTRON_SHIELD_SERVO_OK)
	{
		return buffer[0] << 6 | buffer[1];
	}
	else
	{
		if (this->errorCallback)
			this->errorCallback((uint8_t)chnl, status);
		return 0;
	}
}

/**
 * @brief This function set initial position of servo
 * 
 * @param[in] chnl 
 * 			- Channel to be targeted.
 * 			- Value range: CHANNEL_1 to CHANNEL_8
 * 
 * @param[in] pos 	
 * 			-	Target position
 * 			- Value range: 0 - 8000
 * 
 * @return None
 */

void CytronServoShield::setStartPosition(servo_channel_t chnl, uint16_t pos)
{
	uint8_t first_byte = 0;
	uint8_t high_byte = 0;
	uint8_t low_byte = 0;

	first_byte = POS_START_CMD << 5 | chnl; //make up the 1st byte
	high_byte = (pos >> 6) & 0b01111111;	//make up the high byte
	low_byte = pos & 0b00111111;			//make up the low byte

	if (this->_listen)
#ifdef __AVR__
		this->_listen(this->_serial);
#else
		this->_listen();
#endif

	flush();

	_serial->write(first_byte);
	_serial->write(high_byte);
	_serial->write(low_byte);

	servo_status_t status = getResponseFromCmdUntil(0x04);

	if (status != CYTRON_SHIELD_SERVO_OK)
		if (this->errorCallback)
			this->errorCallback((uint8_t)chnl, status);
	return;
}

/**
 * @brief This function set error callback function, see examples
 * 
 * @param[in] custom callback function
 * 
 * @return None
 */

void CytronServoShield::registerErrorCallback(void (*cb)(uint8_t, servo_status_t))
{
	this->errorCallback = cb;
}

/**
 * private
 */

/**
 * @brief This function gets response with specific length after sending serial command and 
 * 		  store data into buffer
 * 
 * @param[in] buffer
 * 
 * @param[in] buffer length
 * 
 * @return Status
 */

servo_status_t CytronServoShield::getResponseFromCmd(uint8_t *buf, uint16_t len)
{

	uint32_t timeout = millis();
	uint16_t _start = 0;
	while (millis() - timeout < CYTRON_SHIELD_SERVO_TIMEOUT)
	{
		if (_serial->available())
		{
			buf[_start++] = _serial->read();
			timeout = millis();
			continue;
		}
		if (_start == len)
		{
			return CYTRON_SHIELD_SERVO_OK;
		}
		delay(1);
	}
	if (_start > 0 && _start != len)
		return CYTRON_SHIELD_SERVO_CHECKSUM_ERROR;

	return CYTRON_SHIELD_SERVO_TIMEOUT_ERROR;
}

/**
 * @brief This function gets response until terminator character after sending serial command
 * 
 * @param[in] terminator character
 * 
 * @return Status
 */

servo_status_t CytronServoShield::getResponseFromCmdUntil(uint8_t terminator)
{

	uint32_t timeout = millis();
	uint16_t _start = 0;
	while (millis() - timeout < CYTRON_SHIELD_SERVO_TIMEOUT + 4900)
	{ // 5s
		if (_serial->available())
		{
			uint8_t c = _serial->read();
			if (c == terminator)
				return CYTRON_SHIELD_SERVO_OK;
		}
		delay(1);
	}
	return CYTRON_SHIELD_SERVO_TIMEOUT_ERROR;
}

/**
 * @brief This function flush and cleanup serial buffer used by servo shield
 * 
 * @param[in] None
 * 
 * @return None
 */

void CytronServoShield::flush()
{
	while (_serial->available())
	{
		_serial->read();
	}
}
