/*
 * main.c
 *
 *  Created on: 13.10.2013
 *      Author: lorenz
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "base/GEN.h"
#include "base/AD.h"
#include "base/MON.h"
#include "base/DBG.h"
#include "base/Reactor.h"
#include "base/TRC.h"

#include "CommandLineArguments.h"
#include "Connection.h"
#include "ConnectionContainer.h"
#include "ControlCommandRxSocket.h"
#include "TransmitTelemetryTimerHandler.h"
#include "ServoDriver.h"
#include "ServoDriverRPi.h"

int main(int argc, char**argv)
{
	CommandLineArgumentsObject_t args = NewCommandLineArguments(argc, argv);
	if(CommandLineArguments_getParseError(args))
	{
		fprintf(stderr, "%s\n", CommandLineArguments_getUsageStr(args));
		exit(1);
	}
	else
	{
		DBG_LOG_ENTRY("Starting ServoController");
		DBG_Init();
		Reactor_Init();

		TRC_Init();
		if(CommandLineArguments_getMonitorEnable(args))
		{
			MON_Init();
			MON_AddMonCmd("poll", Reactor_MonCmd, 0 );
			MON_AddMonCmd("trc",TRC_ExecMonCmd, 0);
		}

		ServoDriverRegister(ServoDriverRPiSetServos, NULL);
		ConnectionContainerObject_t connectionContainerObject = NewConnectionContainer();
		ControlCommandRxSocketObject_t controlCommandRxSocketObject = NewControlCommandRxSocket(connectionContainerObject);
		TransmitTelemetryTimerHandlerObject_t tranmitTelemetryTimerHandlerObject = NewTransmitTelemetryTimerHandler(connectionContainerObject);

		// main loop
		Reactor_Dispatch();

		DeleteControlCommandRxSocket(controlCommandRxSocketObject);
		DeleteTransmitTelemetryTimerHandler(tranmitTelemetryTimerHandlerObject);
		DeleteConnectionContainer(connectionContainerObject);
		return 0;
	}
}
