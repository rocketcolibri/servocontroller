/**
 * @addtogroup TRC
 *
 * @short Trace Module for debug purpose
 *
 * The TRC module offers the possibility to write debug output to the debug
 * monitor. The application firmware has to pass a trace level as well as
 * a format string and variables as needed by the format string. The output
 * is supplemented with time, trace group name, source filename and source
 * line number. It's up to the user to configure the filter to get the output
 * formatted to his needs.
 * @{
 */

#ifndef TRC_H
#define TRC_H

#include "stdio.h"
#include "GEN.h"


extern char *TRC_varargs(const char *fmt, ...);

/**
 * @short enumeration with the predefined trace levels
 */
typedef enum
{
    TL_SUCCESS,    /**< everybody is happy */
    TL_INFO,       /**< information */
    TL_WARNING,    /**< things are not going really well */
    TL_ERROR,      /**< error */
    TL_FAILURE,    /**< most dramatic message */
    TL_NOF_LVL
} TRC_TraceLevel;

/**
 * @short Initialize TRC module
 *
 * There are the well known three phases to initialize TRC.
 *
 * @param phase One of GEN_INIT_A, GEN_INIT_B, GEN_INIT_C
 */
extern void  TRC_Init();

/**
 * @short Execution of monitor commands
 *
 * This function gets a command line containing all parameters
 * received from the terminal. First it detects the command
 * and depending on the command a different number of para-
 * meters is expected. If no parameter except the command is
 * passed, the help text is displayed.
 *
 * @param sNr Instance number
 * @param cmdLine Pointer to command line string
 * @return TRUE in any case.
 */
extern BOOL  TRC_ExecMonCmd(void *dummy, char *cmdLine);

/**
 * @short Registering a new group for trace output
 *
 * The function registeres a new group, which is producing
 * some trace output. The function returns a group identifier
 * which has to be kept for later use.
 *
 * @param group_name A string which helps identifying the source
 * group of the trace output.
 * @return Trace group identifier created for group_name.
 */
extern UINT8 TRC_AddTraceGroup(char * group_name);

/**
 * @short set the trace level of a trace group
 * 
 * Set the trace level of a trace group. This function can be used
 * from a ExecMonCmd function of a module to set the trace level
 * of the module trace output.
 * 
 * To disable trace output, call the function with TL_NOF_LVL as
 * the trace level.
 * 
 * The function does not enable the 'global' trace output switch. 
 * To enable the trace output, use the 'trc t' command from the
 * debug monitor.
 * 
 * @param group trace group to set the trace level
 * @param lvl Trace level; to disable set to TL_NOF_LVL
 * @return returns the old trace level
 */
extern TRC_TraceLevel TRC_SetTraceLevel(const UINT8 group, const TRC_TraceLevel lvl);

/**
 * @short get the trace level of a trace group
 * 
 * Get the trace level of a trace group. This function can be used
 * from a module to get the trace level of the module trace output.
 * 
 * @param group trace group to set the trace level
 * @return returns the actual trace level
 */
extern TRC_TraceLevel TRC_GetTraceLevel(const UINT8 group);

/**
 * @short Trace output
 *
 * This function writes trace output to the serial interface.
 *
 * which has to be kept for later use.
 *
 * @param file Filename where the trace output is generated
 * @param line Line number inside the file
 * @param group Group identifier returned by TRC_AddTraceGroup()
 * @param lvl Trace level (severity), used to filter output
 * @param fmt Format string, same as for printf()
 * @return FALSE if output was discarded due to lack of memory.
 */
extern BOOL  TRC_Print(const char *file, const UINT16 line, const UINT8 group,
                       const TRC_TraceLevel lvl, FILE *fout, const char *fmt, ...);


/**
 *
 * Write trace print line to output stream. Argument <c> takes the form ("%d, %s", 1, "string")
 * as per the vararglist format of fprintf and printf C functions.
 *
 * Example:
 *
 * TRC_PRINT(UNIT_FOO, TRC0, ("i=%d", 0));
 *
 * @param a unsigned int software unit identifier
 * @param b t_level_t trace level
 * @param c char * Formatted vararglist of fn argument values
 * @return void
 */
#define TRC_PRINT(a,b,c...) {TRC_Print(__FILE__, __LINE__, (a), (b), ##c);}


#define TRC_SUCCESS(a,c...) {TRC_Print(__FILE__, __LINE__, (a), TL_SUCCESS, NULL, ##c);}
#define TRC_INFO(a,c...)    {TRC_Print(__FILE__, __LINE__, (a), TL_INFO,    NULL, ##c);}
#define TRC_WARNING(a,c...) {TRC_Print(__FILE__, __LINE__, (a), TL_WARNING, NULL, ##c);}
#define TRC_ERROR(a,c...)   {TRC_Print(__FILE__, __LINE__, (a), TL_ERROR,   NULL, ##c);}
#define TRC_FAILURE(a,c...) {TRC_Print(__FILE__, __LINE__, (a), TL_FAILURE, NULL, ##c);}

#define TRCF_SUCCESS(a,b,c...) {TRC_Print(__FILE__, __LINE__, (a), TL_SUCCESS, (b), ##c);}
#define TRCF_INFO(a,b,c...)    {TRC_Print(__FILE__, __LINE__, (a), TL_INFO,    (b), ##c);}
#define TRCF_WARNING(a,b,c...) {TRC_Print(__FILE__, __LINE__, (a), TL_WARNING, (b), ##c);}
#define TRCF_ERROR(a,b,c...)   {TRC_Print(__FILE__, __LINE__, (a), TL_ERROR,   (b), ##c);}
#define TRCF_FAILURE(a,b,c...) {TRC_Print(__FILE__, __LINE__, (a), TL_FAILURE, (b), ##c);}

typedef struct
{
  BOOL doLog;
  const char *pFilename;
  FILE *file;
} TRC_File_t;

/**
 * @short Create a new Trace File
 *
 * @param pFilename
 * @param startLogging, TRUE if logging start, FALSE if not, can be started later with TRC_File_SetLogging
 * @return TRC_File handle
 */
extern TRC_File_t* TRC_File_Create(const char *pFilename, BOOL startLogging);

/**
 * @short set logging on/off
 *
 * suspend/resume logging
 *
 * @param fileTrc TRC_File handle
 * @param logging
 */
extern void TRC_File_SetLogging(TRC_File_t* fileTrc, BOOL logging);

/**
 * @short End logging and free resources
 * @param fileTrc
 */
extern void TRC_File_Close(TRC_File_t* fileTrc);

/**
 * Same as TRC_File_Close, but the fileTrc handle is set to NULL
 * @param fileTrc
 * @param fmt
 */
#define TRC_File_CloseAndNull(fileTrc) { TRC_File_Close(fileTrc); fileTrc=NULL;}

/**
 * @short write message to the trace file
 *
 * @param fileTrc
 * @param fmt
 */
extern void TRC_File_Print(TRC_File_t* fileTrc, const char *fmt, ...);

/**
 * @short write log message (with timestamp and newline) to the trace file
 *
 * @param fileTrc
 * @param fmt
 */
extern void TRC_Log_Print(TRC_File_t* fileTrc, const char *fmt, ...);

/**
 * Macro that can be used to ease the use of TRC_File_Print
 */
#define TRC_FILE_PRINT(trc, c...) {TRC_File_Print(trc, ##c);}

/** globel trace file */
extern TRC_File_t* TRC_log;

/** @} */
#endif
