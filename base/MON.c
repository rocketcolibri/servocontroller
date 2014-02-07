#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

#include "AD.h"
#include "GEN.h"
#include "MON.h"
#include "DBG.h"
#include "Reactor.h"

// --- declarations of some ASCII-characters ...
#define NUL     0x00
#define BSP     0x08    // back space
#define HT      0x09    // Tab
#define LF      0x0A    // line feed
#define CR      0x0D    // carrige return
#define ESC     0x1B    // escape
#define DEL     0x7F    // delete
#define ENTER   0x0D    // ENTER character code
#define SPACE   0x20    // SPACE character code
#define SQBRO   0x5B    // square bracket open

#define CMDLINE_LENGTH 256

typedef struct
{
    MON_ExecMonCmdFunction execMonCmdFn;
    char *pCommandName;
    void *pObject;
} execMonCmdEntry_t;

static struct
{
    char rxBuffer[CMDLINE_LENGTH+1];
    UINT posRxBuffer;
    int pipeOutputFd[2];
    AVLTREE commands;
 } mon;

/**
 * add a command to the monitor
 */
BOOL MON_AddMonCmd(const char *cmdName, MON_ExecMonCmdFunction monCmdFn, void *pObject)
{
	execMonCmdEntry_t *pNewCmd = malloc(sizeof(execMonCmdEntry_t));
	bzero(pNewCmd, sizeof(execMonCmdEntry_t));
	pNewCmd->pCommandName = strdup(cmdName);
	pNewCmd->execMonCmdFn = monCmdFn;
	pNewCmd->pObject = pObject;
	avlInsert(mon.commands, (DSKEY)cmdName, pNewCmd);
	return TRUE;
}

/**
 * Print a non formated string
 */
BOOL MON_WriteInfo(const char outStr[])
{
  write(mon.pipeOutputFd[1], outStr, strlen(outStr));
  return TRUE;
}

/**
 * Write a formated string to the monitor
 */
BOOL MON_WriteInfof(const char *fmt, ...)
{
    va_list args;
    char pstr[CMDLINE_LENGTH];
    if (pstr == NULL)
      return FALSE;
    va_start(args, fmt);
    vsnprintf(pstr,CMDLINE_LENGTH, fmt, args);
    pstr[CMDLINE_LENGTH-1]='\000';
    va_end(args);
    return MON_WriteInfo(pstr);
}

/**
 * split command line string into argument count and argument vector
 */
void MON_SplitArgs(char* cmdLine, UINT8* argc, char** *argv)
{
    #define MAX_NOF_ARGS 32

    static char* va[MAX_NOF_ARGS];
    char dummy = '\0';
    UINT8 i;
    UINT8 count = 0;
    char* ptr = cmdLine;

    // skip leading spaces
    while(*ptr == ' ')
    {
        ptr++;
    }

    // buffer start is our first argument
    va[count] = ptr;
    // only increment count if *ptr != '\0'
    if( *ptr != '\0')
    {
      ++count;
    }
    
    while(*ptr!='\0')
    {
        switch(*ptr)
        {
            case ' ':   // argument separators here
            case ',':
                // terminate argument string
                *(ptr++)='\0';
                // skip whitespace
                while(*ptr == ' ')
                {
                    ptr++;
                }
                // our next argument
                va[count] = ptr;
                if(*ptr!='\0')
                {
                    count++;
                }
                break;

            default:
                ptr++;
                break;
        }
        DBG_ASSERT(count < MAX_NOF_ARGS);
    }

    for(i=count; i<MAX_NOF_ARGS; i++)
    {
        va[i] = &dummy;
    }

    *argc = count;
    *argv = va;
}

/**
 * write monitor prompt
 */
static void WritePrompt(void)
{
   MON_WriteInfo("\r\n>");
}

#define XON_CHAR '\021' // ^Q
#define XOFF_CHAR '\023' // ^S
#define EXIT_CHAR '\030' // ^X

static void HandleMonitorInput(char *rxBuffer)
{
  UINT8  argc = 0;
  char** argv = 0;
  char *tmpBuf = strdup(rxBuffer);
  MON_SplitArgs(tmpBuf, &argc, &argv);
  if(argc > 0)
  {
  	execMonCmdEntry_t *pCmd = avlFind(mon.commands, (DSKEY)argv[0]);
		if(pCmd)
		{
			fprintf(stderr, "\nfound MON command: %s", argv[0]);
			pCmd->execMonCmdFn(pCmd->pObject, rxBuffer );
		}
		else
		{
			// run internal monitor function if no one found above
			MON_WriteInfo("\nMonitor commands:");
			void traverseCommands(void *pCommandData)
			{
				execMonCmdEntry_t *pCmd = (execMonCmdEntry_t *)pCommandData;
				MON_WriteInfof("\n%s", pCmd->pCommandName);
		  }
			avlWalk(mon.commands,  traverseCommands, AVLPreWalk);
		}
  }
  free(tmpBuf);
}

/** set file descriptor to non blocking */
static void SetFdNDelay(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    flags |= O_NDELAY;
    fcntl(fd, F_SETFL, flags);
}

/**
 * Handle input from user
 */
static void HandleInput(int fd, void *pData)
{
	char c;
	size_t s = read(fd, &c, 1);
	if(1 == s)
	{
		switch(c)
		{
		case LF:
			mon.rxBuffer[mon.posRxBuffer] = '\000';
			HandleMonitorInput(mon.rxBuffer);
			mon.posRxBuffer = 0;
			WritePrompt();
			break;
		default:
			if(mon.posRxBuffer < CMDLINE_LENGTH )
			{
				mon.rxBuffer[mon.posRxBuffer] = c;
				mon.posRxBuffer++;
			}
			else
				mon.posRxBuffer = 0;
			break;
		}
	}
}

/**
 * Handle output from output pipe
 */
static void HandleOutput(int fd, void *pData)
{
	char buffer[128+1];
  UINT len = read(mon.pipeOutputFd[0], buffer, 128 );
	if(len>0)
	{
		buffer[len]='\000';
		fprintf(stderr, "%s", buffer);
	}
}

/**
 * @short Monitor modul init function
 */
void MON_Init()
{
	bzero(&mon, sizeof(mon));
	mon.commands =avlNewTreeStrKeys();
	DBG_ASSERT(0 == pipe(mon.pipeOutputFd));
	SetFdNDelay(mon.pipeOutputFd[0]);
	SetFdNDelay(mon.pipeOutputFd[1]);
	// add standard input to poll list
	Reactor_AddReadFd(fileno(stdin), HandleInput, &mon, "MON in");
	// add output pipe to poll list
	Reactor_AddReadFd(mon.pipeOutputFd[0], HandleOutput, &mon, "MON out");
	MON_WriteInfo("\n\nServoController monitor started");
	WritePrompt();
}
