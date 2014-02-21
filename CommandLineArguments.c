/*
 * CommandLineArguments.c
 *
 *  Created on: 21.02.2014
 *      Author: lorenz
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "base/GEN.h"

#include "CommandLineArguments.h"

typedef struct
{
	BOOL monitorEnabled;
	char *pConfigFile;
	BOOL parseError;
	char *pUsage;
} CommandLineArguments_t;

/**
   * Creates a new CommandLineArgument object
   *
   * @param argc argument count from main
   * @param argv argument vector from main
   * @return command line argument object
   */
CommandLineArgumentsObject_t NewCommandLineArguments(int argc, char ** argv)
{
	CommandLineArguments_t *pArguments = malloc(sizeof(CommandLineArguments_t));
	bzero(pArguments, sizeof(CommandLineArguments_t));

	char pTmp[1000];
	snprintf(pTmp, sizeof(pTmp), "Usage: %s [-m] [-c <cfg-file>] \
			\n\tm\tenable debug monitor \
			\n\tc\tconfing file", argv[0]);
	pArguments->pUsage = strdup(pTmp);

	int c=0;
	while ((c = getopt(argc, argv, "mc:")) != -1)
	{
		switch (c)
		{
		case 'm':
			pArguments->monitorEnabled = TRUE;
			break;
		case 'c':
			pArguments->pConfigFile = strdup(optarg);
			break;
		case '?':
		default:
			pArguments->parseError = TRUE;
			break;
		}
		fprintf(stderr, "c:%c=%d ", c,c);
	}

	/* Now set the values of "argc" and "argv" to the values after the
	   options have been processed, above. */
	argc -= optind;
	argv += optind;

	/* Now do something with the remaining command-line arguments, if
	   necessary. */

	return pArguments;
}

BOOL CommandLineArguments_getMonitorEnable(CommandLineArgumentsObject_t obj)
{
	return ((CommandLineArguments_t*)obj)->monitorEnabled;
}

BOOL CommandLineArguments_getParseError(CommandLineArgumentsObject_t obj)
{
	return ((CommandLineArguments_t*)obj)->parseError;
}

char * CommandLineArguments_getCfgFileName(CommandLineArgumentsObject_t obj)
{
	return ((CommandLineArguments_t*)obj)->pConfigFile;
}

char * CommandLineArguments_getUsageStr(CommandLineArgumentsObject_t obj)
{
	return ((CommandLineArguments_t*)obj)->pUsage;
}
