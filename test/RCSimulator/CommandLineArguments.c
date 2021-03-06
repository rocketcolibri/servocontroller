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
	BOOL logEnabled;
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
	CommandLineArguments_t *this = malloc(sizeof(CommandLineArguments_t));
	bzero(this, sizeof(CommandLineArguments_t));

	char pTmp[1000];
	snprintf(pTmp, sizeof(pTmp), "Usage: %s [-m] [-c <cfg-file>] \
			\n\ti\tinteractive mode \
			\n\tb\tbatch file\
			\n\tl\tlogfile", argv[0]);
	this->pUsage = strdup(pTmp);

	int c=0;
	while ((c = getopt(argc, argv, "ilb:")) != -1)
	{
		switch (c)
		{
		case 'i':
			this->monitorEnabled = TRUE;
			break;
		case 'b':
			this->pConfigFile = strdup(optarg);
			break;
		case 'l':
			this->logEnabled = TRUE;
			break;
		case '?':
		default:
			this->parseError = TRUE;
			break;
		}
	}

	/* Now set the values of "argc" and "argv" to the values after the
	   options have been processed, above. */
	argc -= optind;
	argv += optind;

	/* Now do something with the remaining command-line arguments, if
	   necessary. */

	return this;
}

BOOL CommandLineArguments_getInteractiveModeEnable(CommandLineArgumentsObject_t obj)
{
	return ((CommandLineArguments_t*)obj)->monitorEnabled;
}

BOOL CommandLineArguments_getParseError(CommandLineArgumentsObject_t obj)
{
	return ((CommandLineArguments_t*)obj)->parseError;
}

char * CommandLineArguments_getBatchFileName(CommandLineArgumentsObject_t obj)
{
	return ((CommandLineArguments_t*)obj)->pConfigFile;
}

BOOL CommandLineArguments_getLogEnabled(CommandLineArgumentsObject_t obj)
{
	return ((CommandLineArguments_t*)obj)->logEnabled;
}


char * CommandLineArguments_getUsageStr(CommandLineArgumentsObject_t obj)
{
	return ((CommandLineArguments_t*)obj)->pUsage;
}
