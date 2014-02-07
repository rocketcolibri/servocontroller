/*
 * ServoDriverRPi.c
 *
 *  Created on: 27.10.2013
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


static UINT32 oldChannels[MAX_SERVOS] = { 0,0,0,0,0,0,0,0};

static void SetServo(UINT32 s, UINT32 pos)
{
	ServoDriver_t *pServoDriver = ServoDriverGetInstance();
	char cmd[64];
	sprintf(cmd, "echo %d=%d > /dev/servoblaster", s, pos);
	DBG_ASSERT_NO_RES(0==system(cmd));
	TRC_INFO(pServoDriver->hTrc, cmd);
}

void ServoDriverRPiSetServos(UINT32 servoc, UINT32 *servov)
{
	UINT s=0;
	for(s=0; s < MAX_SERVOS; s++)
	{
		if(s < servoc )
		{
			if(oldChannels[s] != servov[s])
			{
				SetServo(s, servov[s]);
				oldChannels[s] = servov[s];
			}
		}
		else
		{
			if(oldChannels[s] != 0)
			{
				SetServo(s, 0);
				oldChannels[s] = 0;
			}
		}
	}
}
