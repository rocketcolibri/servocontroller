/**
 * @addtogroup  BKGR-Timer fd
 *
 * @short BKGR timer file descriptor
 *
 * The timer file descriptor was introduced for the first time in the kernel version 2.6.22.
 *
 * Because the API from 2.6.22 is is not powerful enough, the API has been removed in 2.6.23 and
 * a new has been introduced in kernel version 2.6.25.
 * The new API is supported by the glibc (include/linux/timerfd.h )
 *
 * Since we use the kernel 2.6.22 for the ACCEED devices we have to deal with the old timerfd functions
 * which are not supported by the glibc.
 *
 * This module provides the BKGR_TimerFd_Create and BKGR_TimerFd_Read function to create and read the
 * timer file descriptors directly with the linux system calls.
 *
 * @see BKGR_TimerFd_MonCmd to have an example how to use the timer fd.
 *
 * @note This code runs only on kernel 2.6.22
 * @note This code runs only on ARM platform (since syscall IDs are platform dependent)
 *
 * @{
 */


#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <sys/time.h>
#include <sys/timerfd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <poll.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include "GEN.h"
#include "DBG.h"
#include "TRC.h"
#include "POLL.h"


static unsigned long long TimerFdGetUsTime(int clockid) {
  struct timespec tp;

  if (clock_gettime((clockid_t) clockid, &tp))
  {
    DBG_MAKE_ENTRY_MSG(FALSE, strerror(errno));
    return 0;
  }

  return 1000000ULL * tp.tv_sec + tp.tv_nsec / 1000;
}

static void TimerFdSetTimespec(struct timespec *tmr, unsigned long long ustime)
{
  tmr->tv_sec = (time_t) (ustime / 1000000ULL);
  tmr->tv_nsec = (long) (1000ULL * (ustime % 1000000ULL));
}

/**
 * @short Create a timer file descriptor
 *
 * @param us time in micro seconds
 * @return timer file descriptor
 */
int TIMERFD_Create(UINT32 us)
{
	int fd = timerfd_create(CLOCK_MONOTONIC, 0);
	struct itimerspec newValue;
	struct itimerspec oldValue;
	bzero(&newValue,sizeof(newValue));
	bzero(&oldValue,sizeof(oldValue));
	struct timespec ts;
	ts.tv_sec = us / 1000000;
	ts.tv_nsec = 1000*(us % 1000000);

  //both interval and value have been set
  newValue.it_value = ts;
  newValue.it_interval = ts;
  if( timerfd_settime(fd,0,&newValue,&oldValue) <0)
  {
     fprintf(stderr, "settime error %s", strerror(errno));;
  }
  return fd;
}

/**
 * @short Reads the from the timer file descriptor
 * @param timerfd file descriptor
 * @return number of expired timer events
 */
UINT32 TIMERFD_Read(int timerfd)
{
  unsigned long long timer=0;
  int len = read(timerfd, &timer, sizeof(timer));
  if(len  < 0)
    DBG_MAKE_ENTRY_MSG(FALSE, strerror(errno));

  return (UINT32)timer;
}

#if 0
static void *hBkgrPollTimerFdTest;
static void ReadTimerFdTest(int fd, void *pData)
{
  UINT32 timer = BKGR_TimerFd_Read(fd);

  if(timer)
    MON_WriteInfof("Timer %d Eventd: %d\n",fd, timer);

  BKGR_POLL_RemoveFd(hBkgrPollTimerFdTest);
  hBkgrPollTimerFdTest = NULL;
  close(fd);
}

/** monitor command called from in BKGR main init function */
BOOL TIMERFD_MonCmd(int argc, char **argv)
{
  static const char *pCmdTestTimerfd = "test";

  if ((argc == 2) && (0 == strcmp(argv[0], pCmdTestTimerfd)))
  {
    if(NULL == hBkgrPollTimerFdTest)
    {
      // create timer fd and add it to the poll list
      hBkgrPollTimerFdTest = BKGR_POLL_AddReadFd(
          BKGR_TimerFd_Create(atoi(argv[1])),
          ReadTimerFdTest, NULL, pCmdTestTimerfd );
    }
    else
      MON_WriteInfo("Timer already running\n");
  }
  else
  {
    MON_WriteInfof("usage:\n\n");
    MON_WriteInfof("%s %s %s <time-us>", pBkgrMonCmd, pBkgrTimerfdCmd, pCmdTestTimerfd);
  }
  return TRUE;
}
#endif
