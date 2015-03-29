/**
 * Debug modul contains error handling, assertions and singnal handler
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <syslog.h>
#ifdef NO_BACKTRACE
#include <execinfo.h>
#endif

#include "GEN.h"
#include "DBG.h"


void DBG_PrintTraceToFile(void)
{
	// backtrace isn't working with uclibc
#ifdef NO_BACKTRACE
	int tracefile = open("/tmp/servocontroller.backtrace", O_WRONLY | O_CREAT | O_TRUNC);
	if (tracefile)
	{
		void *array[10];
		size_t size = backtrace(array, 10);
		backtrace_symbols_fd(array, size, tracefile);
		close(tracefile);
	}
#endif

}

typedef void (*DBG_SigHandler_t)(int);
static DBG_SigHandler_t DBG_InstallSignalHandler(int sig_nr,
		DBG_SigHandler_t signalhandler)
{
	struct sigaction neu_sig, alt_sig;

	neu_sig.sa_handler = signalhandler;
	sigemptyset(&neu_sig.sa_mask);
	neu_sig.sa_flags = SA_RESTART;
	if (sigaction(sig_nr, &neu_sig, &alt_sig) < 0)
	{
		return SIG_ERR ;
	}

	return alt_sig.sa_handler;
}

static void CatchSigFpe(int sig_nr)
{
	DBG_MAKE_ENTRY(FALSE);
}
static void CatchSigIll(int sig_nr)
{
	DBG_MAKE_ENTRY(FALSE);
}
static void CatchSigBus(int sig_nr)
{
	DBG_MAKE_ENTRY(FALSE);
}
static void CatchSigSys(int sig_nr)
{
	DBG_MAKE_ENTRY(FALSE);;
}
static void CatchSigTrap(int sig_nr)
{
	DBG_MAKE_ENTRY(FALSE);
}
static void CatchSigAbrt(int sig_nr)
{
	DBG_PrintTraceToFile();
	DBG_MAKE_ENTRY(TRUE);
}
static void CatchSigSegv(int sig_nr)
{
	DBG_PrintTraceToFile();
	DBG_MAKE_ENTRY(TRUE);
	signal(sig_nr, SIG_DFL );
	kill(getpid(), sig_nr);
}
static void CatchSigTerm(int sig_nr)
{
	DBG_PrintTraceToFile();
	DBG_MAKE_ENTRY(TRUE);
}
static void CatchSigInt(int sig_nr)
{
	DBG_MAKE_ENTRY(FALSE);
}
static void CatchSigHup(int sig_nr)
{
	DBG_MAKE_ENTRY(FALSE);
}
static void CatchSigKill(int sig_nr)
{
	DBG_PrintTraceToFile();
	DBG_MAKE_ENTRY(TRUE);
}
static void CatchSigQuit(int sig_nr)
{
	DBG_PrintTraceToFile();
	DBG_MAKE_ENTRY(TRUE);
}
static void CatchSigAlrm(int sig_nr)
{
	DBG_MAKE_ENTRY(FALSE);
}
static void CatchSigVtalrm(int sig_nr)
{
	DBG_MAKE_ENTRY(FALSE);
}
static void CatchSigProf(int sig_nr)
{
	DBG_MAKE_ENTRY(FALSE);
}
static void CatchSigPoll(int sig_nr)
{
	DBG_MAKE_ENTRY(FALSE);
}
static void CatchSigPipe(int sig_nr)
{
	DBG_MAKE_ENTRY(FALSE);
}
static void CatchSigXcpu(int sig_nr)
{
	DBG_PrintTraceToFile();
	DBG_MAKE_ENTRY(TRUE);
}
static void CatchSigXfsz(int sig_nr)
{
	DBG_PrintTraceToFile();
	DBG_MAKE_ENTRY(TRUE);
}
static void CatchSigUsr1(int sig_nr)
{
	DBG_MAKE_ENTRY(FALSE);
}
static void CatchSigUsr2(int sig_nr)
{
	DBG_MAKE_ENTRY(FALSE);
}

//*****************************************************************************
// PURPOSE: Initialisierung der Debug-Fkt
// INPUT:   phase: Phase der Initialisierung
// OUTPUT:  -
// RETURN:  -
//*****************************************************************************

void DBG_Init()
{
	DBG_InstallSignalHandler(SIGFPE, CatchSigFpe);
	DBG_InstallSignalHandler(SIGILL, CatchSigIll);
	DBG_InstallSignalHandler(SIGBUS, CatchSigBus);
	DBG_InstallSignalHandler(SIGSYS, CatchSigSys);
	DBG_InstallSignalHandler(SIGTRAP, CatchSigTrap);
	DBG_InstallSignalHandler(SIGABRT, CatchSigAbrt);
	DBG_InstallSignalHandler(SIGSEGV, CatchSigSegv); // install handler for SIGSEGV if running from flash
	DBG_InstallSignalHandler(SIGTERM, CatchSigTerm);
	DBG_InstallSignalHandler(SIGINT, CatchSigInt);
	DBG_InstallSignalHandler(SIGHUP, CatchSigHup);
	DBG_InstallSignalHandler(SIGKILL, CatchSigKill);
	DBG_InstallSignalHandler(SIGQUIT, CatchSigQuit);
	DBG_InstallSignalHandler(SIGALRM, CatchSigAlrm);
	DBG_InstallSignalHandler(SIGVTALRM, CatchSigVtalrm);
	DBG_InstallSignalHandler(SIGPROF, CatchSigProf);
	DBG_InstallSignalHandler(SIGPOLL, CatchSigPoll);
	DBG_InstallSignalHandler(SIGPIPE, CatchSigPipe);
	DBG_InstallSignalHandler(SIGXCPU, CatchSigXcpu);
	DBG_InstallSignalHandler(SIGXFSZ, CatchSigXfsz);
	DBG_InstallSignalHandler(SIGUSR1, CatchSigUsr1);
	DBG_InstallSignalHandler(SIGUSR2, CatchSigUsr2);
}

/**
 * DBG_ErrorHandler
 */
void DBG_ErrorHandler(UINT8 reset, UINT8 err_code, char *file, UINT16 fileLine,	char *msg)
{
	char errorMsg[1024];
 snprintf(errorMsg, sizeof(errorMsg), "%c %s@%d %s",
				reset==DBG_NO_RESET ? 'N' :
				reset==DBG_RESET ?    'R' : 'L',
				file, fileLine, msg);

	syslog(0, "%s",errorMsg);
	fprintf(stderr, "\n%s", errorMsg);
	if(DBG_RESET == reset)
		exit(1);
}
