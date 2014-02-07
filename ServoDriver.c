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
#include "base/Reactor.h"
#include "base/AD.h"

#include "ServoDriver.h"

ServoDriver_t *pServoDriver = NULL;

static void SetServo(UINT32 servoc, UINT32 servov[])
{
	char msg[1024];
	msg[0]='\000';
	while(servoc)
	{
		char submsg[40];
		servoc--;
		sprintf(submsg, "c%d=%d ", servoc, servov[servoc]);
		strcat(msg, submsg);

	}
	TRC_INFO(pServoDriver->hTrc, "%s", msg);
}

ServoDriver_t *ServoDriverGetInstance()
{
	if(NULL == pServoDriver)
	{
		pServoDriver = malloc(sizeof(ServoDriver_t));
		bzero(pServoDriver, sizeof(ServoDriver_t));
		pServoDriver->SetServos = SetServo;
		pServoDriver->hTrc = TRC_AddTraceGroup("servo");
	}
	return pServoDriver;
}


void ServoDriverRegister(void (* SetServos)(UINT32, UINT32 *), void *pServoDriverObject)
{
	ServoDriver_t *pServoDriver = ServoDriverGetInstance();
	TRC_INFO(pServoDriver->hTrc, "Servo driver registered");
	pServoDriver->SetServos = SetServos;
	pServoDriver->pServoDriverObject = pServoDriverObject;
}
