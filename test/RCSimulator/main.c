/*
 * main.c
 *
 *  Created on: 11.06.2014
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

CommandLineArgumentsObject_t args;

int main(int argc, char**argv)
{

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
