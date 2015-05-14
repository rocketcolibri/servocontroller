/*
 * ServoDriverRPi.c
 *
 *  Created on: 27.10.2013
 *      Author: lorenz
 */

#include <stdio.h>
#include <stdlib.h>
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
	printf("\rServo Position:");
	for(s=0; s < MAX_SERVOS; s++)
	{
		printf("%d=%-4d ", s, servov[s]);
	}
}

extern void ServoDriverMockStoreFailsafePosition(UINT32 servoc, UINT32 *servov)
{
//  UINT s=0;
//  printf("\nset failsafe");
//  for(s=0; s < MAX_SERVOS; s++)
//  {
//    fprintf(stderr, "servo:%d=%d", s, servov[s]);
//  }
}

extern void ServoDriverMockSetFailsafe()
{
	DBG_LOG_ENTRY_FMT("Set setvos to failsafe poistion");
}
