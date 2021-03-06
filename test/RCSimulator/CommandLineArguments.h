/*
 * CommandLineArguments.h
 *
 *  Created on: 21.02.2014
 *      Author: lorenz
 */

#ifndef COMMANDLINEARGUMENTS_H_
#define COMMANDLINEARGUMENTS_H_

typedef void *CommandLineArgumentsObject_t;

extern CommandLineArgumentsObject_t args;

/**
   * Creates a new CommandLineArgument object
   *
   * @param argc argument count from main
   * @param argv argument vector from main
   * @return command line argument object
   */
extern CommandLineArgumentsObject_t NewCommandLineArguments(int argc, char ** argv);

// getters
extern BOOL CommandLineArguments_getInteractiveModeEnable(CommandLineArgumentsObject_t obj);

extern BOOL CommandLineArguments_getParseError(CommandLineArgumentsObject_t obj);

extern char * CommandLineArguments_getBatchFileName(CommandLineArgumentsObject_t obj);

extern BOOL CommandLineArguments_getLogEnabled(CommandLineArgumentsObject_t obj);

extern char * CommandLineArguments_getUsageStr(CommandLineArgumentsObject_t obj);

extern char * CommandLineArguments_getServoDriver(CommandLineArgumentsObject_t obj);
#endif /* COMMANDLINEARGUMENTS_H_ */
