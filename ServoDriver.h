/*
 * ServoDriver.h
 *
 *  Created on: 25.10.2013
 *      Author: lorenz
 */

#ifndef SERVODRIVER_H_
#define SERVODRIVER_H_

#define MAX_SERVOS 8

typedef struct
{
	void (* SetServos)(UINT32 channelc, UINT32 *channelv);
	void *pServoDriverObject;
	UINT8 hTrc;
} ServoDriver_t;

extern ServoDriver_t *ServoDriverGetInstance();

extern void ServoDriverRegister(void (* SetServos)(UINT32, UINT32 *), void *pServoDriverObject);

#endif /* SERVODRIVER_H_ */
