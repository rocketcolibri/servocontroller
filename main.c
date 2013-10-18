/*
 * main.c
 *
 *  Created on: 13.10.2013
 *      Author: lorenz
 */

#include <stdio.h>
#include <unistd.h>
#include "base/GEN.h"
#include "base/DBG.h"
#include "base/POLL.h"
#include "base/TRC.h"

#include "ControlCommandRxSocket.h"
#include "TransmitTelemetryTimerHandler.h"

int main(int argc, char**argv)
{
	DBG_LOG_ENTRY("Starting ServoController");

	DBG_Init();
	TRC_Init();
	POLL_Init();

	int tfd = TIMERFD_Create(1000);
	unsigned char value[8];
	read(tfd, value, 8);

	UINT32 t = TIMERFD_Read(tfd);


	fprintf(stderr, "\ntime:%d",t);

	void *pControlCommandRxSocket = NewControlCommandRxSocket();
	void *pTranmitTelemetryTimerHandler = NewTransmitTelemetryTimerHandler();

	POLL_Dispatch();

	DeleteControlCommandRxSocket(pControlCommandRxSocket);
	DeleteTransmitTelemetryTimerHandler(pTranmitTelemetryTimerHandler);

	return 0;
}
