/**
* @addtogroup MON
*
* Each softwre module may register its monitor function together with the
* command name to the MON module.
* This function will be called when the command name has been entered
* in the monitor console. Because of having no statically linked functions
* from the MON module has only little dependencies to other software modules.
*
* The MON module provides a driver API which allows a flexible interface
* to various character oriented hardware devices such as external serial interface (on the
* debug module), ASC0 (internal asynchronous serial interface) SSC0 (synchronous
* serial interface) and remote console (RCON) for debuging over the eoc channel or logical
* interfaces such as telnet, standard input/output.
*
* @{
*/

#ifndef PPG_MON0_H
#define PPG_MON0_H

#include "GEN.h"

/** 
* @short monitor function type
*
* Functions that may be registred to the MON module must be of this type.
*/
typedef BOOL (*MON_ExecMonCmdFunction)(void *pObject, char * cmdLine);

/**
* @short Init function
*/
extern void MON_Init();

/**
* @short pint a message to the monitor output
*
* @param out_str string to print
*/
extern BOOL MON_WriteInfo(const char out_str[]);

/**
* @short print a formated message to the monior output 
* 
* The syntax of the format string is the same as in the printf function
* from the C library.
* MON_WriteInfof uses its first argument to decide how many arguments follow and what
* their type is. It will get confused, and you will get wrong answers, if there are
* not enough arguments of if they are the wrong type.
*
* @param fmt format string including format conversions (%.)
* @param ... varible arguments depending on the fmt string
*/
extern BOOL MON_WriteInfof(const char *fmt, ...);

/**
* @short add a monitor function
*
* Each module may register its own monitor function to the MON module.
*
* @param cmdName command name. up to 5 characters
* @param monCmdFn function pointer to the monitor function of the external module
* @param inst instance, (used by the external module)
*/ 
extern BOOL MON_AddMonCmd(const char *cmdName, MON_ExecMonCmdFunction monCmdFn, void *pObject);


/**
* @short split the command line into an array of strings sepeated by ' ' or ','
*
* Note: This function is *NOT* reentrant.
*       This function modifies the memory in cmdLine variable.
* 
* <pre>
* 
* BOOL ExecMonCmd(UINT16 inst, char* cmdLine) {
* UINT8 argc ;
* char** argv ;
* MON_SplitArgs( cmdLine, &argc, &argv ) ;
* if ((1==argc) && (0==strcmp("list", argv[0]))) {
*   // Execute list command 
* } else if ((1==argc) && (0==strcmp("reset", argv[0]))) {
*   // Execute reset command
* } else {
*   // help / usage command
* }
* 
* </pre>
*
* @param cmdLine command line
* @param argc argument count. (how many substrings)
* @param argv argument vector (array of strings)
*/
extern void MON_SplitArgs(char* cmdLine, UINT8* argc, char** *argv);

/** @}*/
#endif
