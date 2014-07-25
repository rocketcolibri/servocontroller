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

#define  SERVO_MIN_POS  50
#define  SERVO_MAX_POS  250
#define  CHANNEL_MIN_POS  0
#define  CHANNEL_MAX_POS  1000
#define  CHANNEL_TO_SERVO(channel) \
	(channel/((CHANNEL_MAX_POS-CHANNEL_MIN_POS)/(SERVO_MAX_POS-SERVO_MIN_POS))+SERVO_MIN_POS)

/**
 * converts the channel value from the RocketColibri remote control to the value needed by the servoblaster device
 */
static UINT32 ConvertChannelToServoHw(UINT32 channelPosition)
{
	return (channelPosition < CHANNEL_MAX_POS) ? CHANNEL_TO_SERVO(channelPosition) : CHANNEL_MAX_POS;
}

static UINT32 oldChannels[MAX_SERVOS] = { 0,0,0,0,0,0,0,0};
static UINT32 failsafe[MAX_SERVOS] = { 0,0,0,0,0,0,0,0};

static void SetServo(UINT32 s, UINT32 pos)
{
	FILE *f= fopen("/dev/servoblaster", "w");
	if(f)
	{
		fprintf(f, "%d=%d\n", s, ConvertChannelToServoHw(pos));
		fclose(f);
	}
	else
	{
		DBG_MAKE_ENTRY_FMT(TRUE, "no permissions to write to '/dev/servoblaster' (%d=%d)", s, ConvertChannelToServoHw(pos));
	}
}

static void PrintServoStates()
{
	DBG_ASSERT(MAX_SERVOS == 8);
	FILE *f = fopen("/tmp/servostates", "w");
	if(f)
	{
		fprintf(f, "{\"channels\":[%d, %d, %d, %d, %d, %d, %d, %d]}\n",
			oldChannels[0], oldChannels[1], oldChannels[2], oldChannels[3],
			oldChannels[4], oldChannels[5], oldChannels[6], oldChannels[7]);
		fclose(f);
	}
}

void ServoDriverRPiSetServos(UINT32 servoc, UINT32 *servov)
{
	UINT s=0;
	BOOL servoChanged = FALSE;
	for(s=0; s < MAX_SERVOS; s++)
	{
		if(s < servoc )
		{
			if(oldChannels[s] != servov[s])
			{
				SetServo(s, servov[s]);
				oldChannels[s] = servov[s];
				servoChanged = TRUE;
			}
		}
		else
		{
			if(oldChannels[s] != 0)
			{
				SetServo(s, 0);
				oldChannels[s] = 0;
				servoChanged = TRUE;
			}
		}
	}
	if(servoChanged)
		PrintServoStates();
}

void ServoDriverRPiStoreFailsafePosition(UINT32 servoc, UINT32 *servov)
{
  memcpy(failsafe, servov, sizeof(UINT32)*servoc);
}

void ServoDriverRPiSetFailsafe()
{
  ServoDriverRPiSetServos(failsafe, MAX_SERVOS);
}
