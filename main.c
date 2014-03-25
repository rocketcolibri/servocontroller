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
#include "ConnectionStateMachine.h"
#include "SystemStateMachine.h"
#include "ConnectionContainer.h"
#include "ControlCommandRxSocket.h"
#include "TransmitTelemetryTimerHandler.h"
#include "ServoDriver.h"
#include "ServoDriverRPi.h"
#include "ServoDriverMock.h"

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
		if(CommandLineArguments_getExecUnitTests(args))
		{
			ConnectionStateMachineTest();
			SystemStateMachineTest();
			exit(1);
		}

		DBG_Init();
		Reactor_Init();
		TRC_Init();

		TRC_log = TRC_File_Create("/tmp/servocontroller.log", CommandLineArguments_getLogEnabled(args));
		TRC_Log_Print(TRC_log, "Starting ServoController");
		if(CommandLineArguments_getMonitorEnable(args))
		{
			MON_Init();
			MON_AddMonCmd("poll", Reactor_MonCmd, 0 );
			MON_AddMonCmd("trc",TRC_ExecMonCmd, 0);
		}

		if(CommandLineArguments_getSimEnabled(args))
		{
			TRC_Log_Print(TRC_log, "Register mock servo driver");
			ServoDriverRegister(ServoDriverMockSetServos, NULL);
		}
		else
		{
			TRC_Log_Print(TRC_log, "Register RPi servo driver");
			ServoDriverRegister(ServoDriverRPiSetServos, NULL);
		}

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
