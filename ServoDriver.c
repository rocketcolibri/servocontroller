/*
 * ServoDriver.c
 *
 *  Created on: 25.10.2013
 *      Author: lorenz
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <memory.h>
#include <unistd.h>
#include <fcntl.h>
#include <json-c/json.h>

#include "base/GEN.h"
#include "base/DBG.h"
#include "base/TRC.h"
#include "base/POLL.h"
#include "base/AD.h"

#include "ServoDriver.h"

ServoDriver_t *pServoDriver = NULL;

static void SetServo(UINT32 servoc, UINT32 servov[])
{

}

ServoDriver_t *ServoDriverGetInstance()
{
	if(NULL == pServoDriver)
	{
		pServoDriver = malloc(sizeof(ServoDriver_t));
		bzero(pServoDriver, sizeof(ServoDriver_t));
		pServoDriver->SetServos = SetServo;
	}
	return pServoDriver;
}


void ServoDriverRegister(void (* SetServos)(UINT32, UINT32 *), void *pServoDriverObject)
{
	ServoDriver_t *pServoDriver = ServoDriverGetInstance();
	pServoDriver->SetServos = SetServos;
	pServoDriver->pServoDriverObject = pServoDriverObject;
}
