/*
 * main.c
 *
 *  Created on: 11.06.2014
 *      Author: lorenz
 */

// setting multiple IP addresses on wlan0 interface
//$ sudo ip addr add 192.168.10.120/24  dev wlan0
//$ sudo ip addr add 192.168.10.121/24  dev wlan0
//$ sudo ip addr add 192.168.10.122/24  dev wlan0
//$ sudo ip addr add 192.168.10.123/24  dev wlan0


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

CommandLineArgumentsObject_t args;

int main(int argc, char**argv)
{
	args = NewCommandLineArguments(argc, argv);
	if(CommandLineArguments_getParseError(args))
	{
		fprintf(stderr, "%s\n", CommandLineArguments_getUsageStr(args));
		exit(1);
	}

	const char *pCfgFile = CommandLineArguments_getCfgFileName(args);
	if(pCfgFile)
	{

	}

	DBG_Init();
	Reactor_Init();
	TRC_Init();

	MON_Init();
	MON_AddMonCmd("poll", Reactor_MonCmd, 0 );
	MON_AddMonCmd("trc",TRC_ExecMonCmd, 0);

	// main loop
	Reactor_Dispatch();

	return 0;
}
