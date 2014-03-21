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


void ServoDriverMockSetServos(UINT32 servoc, UINT32 *servov)
{
	UINT s=0;
	printf("\n");
	for(s=0; s < MAX_SERVOS; s++)
	{
		fprintf(stderr, "servo:%d=%d", s, servov[s]);
	}
}
