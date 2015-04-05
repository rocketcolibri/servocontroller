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
#include "ConnectionFsm.h"
#include "SystemFsm.h"
#include "ConnectionContainer.h"
#include "MessageReceiver.h"
#include "TransmitTelemetry.h"
#include "ServoDriver.h"
#include "ServoDriverRPi.h"
#include "ServoDriverLinino.h"
#include "ServoDriverMock.h"

CommandLineArgumentsObject_t args;

int main(int argc, char**argv)
{
	args = NewCommandLineArguments(argc, argv);
	if(CommandLineArguments_getParseError(args))
	{
		fprintf(stderr, "%s\n", CommandLineArguments_getUsageStr(args));
		exit(1);
	}
	else
	{
		if(CommandLineArguments_getExecUnitTests(args))
		{
			ConnectionFsmTest();
			SystemFsmTest();
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

		if(0==strcmp("simulate", CommandLineArguments_getServoDriver(args)))
		{
			TRC_Log_Print(TRC_log, "Register mock servo driver");
			ServoDriverRegister(ServoDriverMockSetServos,
			    ServoDriverMockStoreFailsafePosition,
			    ServoDriverMockSetFailsafe,NULL);
		}
		else if(0==strcmp("linino", CommandLineArguments_getServoDriver(args)))
		{
			TRC_Log_Print(TRC_log, "Register Linino servo driver");
			ServoDriverRegister(ServoDriverLininoSetServos,
			    ServoDriverLininoStoreFailsafePosition,
				ServoDriverLininoSetFailsafe,NULL);
		}
		else // rpi
		{
			TRC_Log_Print(TRC_log, "Register RasperryPi servo driver");
			ServoDriverRegister(ServoDriverRPiSetServos,
			    ServoDriverRPiStoreFailsafePosition,
			    ServoDriverRPiSetFailsafe,NULL);
		}

		ConnectionContainerObject_t connectionContainerObject = NewConnectionContainer();
		MessageReceiverObject_t MessageReceiverObject = NewMessageReceiver(connectionContainerObject);
		TransmitTelemetryObject_t tranmitTelemetryObject = NewTransmitTelemetry(connectionContainerObject);

		// main loop
		Reactor_Dispatch();

		DeleteMessageReceiver(MessageReceiverObject);
		DeleteTransmitTelemetry(tranmitTelemetryObject);
		DeleteConnectionContainer(connectionContainerObject);
		return 0;
	}
}
