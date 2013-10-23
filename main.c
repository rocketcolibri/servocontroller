/*
 * main.c
 *
 *  Created on: 13.10.2013
 *      Author: lorenz
 */

#include <stdio.h>
#include <unistd.h>
#include "base/GEN.h"
#include "base/MON.h"
#include "base/DBG.h"
#include "base/POLL.h"
#include "base/TRC.h"

#include "ControlCommandRxSocket.h"
#include "TransmitTelemetryTimerHandler.h"

int main(int argc, char**argv)
{
	DBG_LOG_ENTRY("Starting ServoController");

	DBG_Init();
	POLL_Init();
	MON_Init();
	TRC_Init();
	MON_AddMonCmd("poll", POLL_MonCmd, 0 );
	MON_AddMonCmd("trc",TRC_ExecMonCmd, 0);

	void *pControlCommandRxSocket = NewControlCommandRxSocket();
	void *pTranmitTelemetryTimerHandler = NewTransmitTelemetryTimerHandler();

	POLL_Dispatch();

	DeleteControlCommandRxSocket(pControlCommandRxSocket);
	DeleteTransmitTelemetryTimerHandler(pTranmitTelemetryTimerHandler);

	return 0;
}
