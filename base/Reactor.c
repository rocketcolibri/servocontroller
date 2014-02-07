/**
 * @addtogroup  Poll
 *
 * @{
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/epoll.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "GEN.h"
#include "DBG.h"
#include "MON.h"
#include "TRC.h"
#include "Reactor.h"

#define REACTOR_NOF  100

static const char* pPollName = "reactor";

/** module data */
static struct
{
    struct epoll_event events[REACTOR_NOF]; // epoll event struct
    int epollfd; // epoll file descriptor
    pthread_t hArpingThread; // thread handle
    pthread_mutex_t pStateMachineMutex; // state machnie mutex
    UINT8 hTrc; // trace handle
    UINT8 traceLevel; // and level
} reactor;


/** module data */
typedef struct
{
    int fd;
    Reactor_CallbackFunction_t pCallBackFn;
    void *pData;
    const char *pPollDescription;
    struct epoll_event event; // event for epoll wait
} PollData_t;

/**
 * @short Add a file descriptor to the poll thread
 * If the file descriptor is ready to be read the pCallback function is called. The read must be done
 * inside this function. The data registered as pData is called in the pCallbackFn as the pData parameter
 * and may be casted to the data type of the consumer module.
 *
 * @param fd file descriptor
 * @param pCallbackFn callback function
 * @param pData void pointer to the data of the consumer
 * @param pDescription description for trace
 */
void * Reactor_AddReadFd(int fd, Reactor_CallbackFunction_t pCallbackFn,void *pData, const char *pDescription)
{
  DBG_ASSERT(pCallbackFn);
  DBG_ASSERT(pDescription);
  if(fd<0)
  {
  	DBG_MAKE_ENTRY_FMT(TRUE, "invalid file handle:%d from %s",fd, pDescription);
  }


  PollData_t * pPollData = malloc(sizeof(PollData_t));
  bzero(pPollData, sizeof(pPollData));

  pPollData->fd = fd;
  pPollData->pCallBackFn = pCallbackFn;
  pPollData->pData = pData;
  pPollData->pPollDescription = pDescription;

  // set file descriptor to non blocking
  fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);

  // set events
  pPollData->event.events =  EPOLLIN;
  pPollData->event.data.ptr = pPollData;

  // add to poll list
  if(-1 == epoll_ctl(reactor.epollfd, EPOLL_CTL_ADD, pPollData->fd, &pPollData->event))
  {
    DBG_MAKE_ENTRY_MSG(FALSE, strerror(errno));
    free(pPollData);
    pPollData = NULL;
  }

  return (void *)pPollData;
}

/**
 * @short remove a file descriptor from the poll list
 *
 * @param pPollDataHandle
 */
void Reactor_RemoveFd(void *pPollDataHandle)
{
  DBG_ASSERT(pPollDataHandle);
  PollData_t * pPollData = (PollData_t*)pPollDataHandle;
  TRC_INFO(reactor.hTrc, "%s:0x%x removed fd:%d",pPollData->pPollDescription,  pPollData, pPollData->fd );

    // close pipe
  if(pPollData->fd)
  {
    epoll_ctl(reactor.epollfd, EPOLL_CTL_DEL, pPollData->fd, NULL);
  }

  free(pPollData);
}

/**
 * @short remove a file descriptor from the poll list and close the file
 *
 * @param pPollDataHandle
 */
void Reactor_RemoveFdAndClose(void *pPollDataHandle)
{
  int fd = ((PollData_t*)pPollDataHandle)->fd;
  Reactor_RemoveFd(pPollDataHandle);
  close(fd);
}

/**
 * Waits for an event in the poll list and calls the callback function associated with
 * the file descriptor.
 */
void* Reactor_Dispatch()
{
  for (;;)
  {
    // wait until at least one file descriptor is ready
    UINT ready = epoll_wait(reactor.epollfd, reactor.events, REACTOR_NOF, -1);
    if (ready == -1)
    {
      DBG_MAKE_ENTRY_MSG(FALSE, strerror(errno));
      continue;
    }

    TRC_INFO(reactor.hTrc, "Epoll ready:%d", ready);

    // go trough the file descriptor list
    int i;
    for (i = 0; i < ready; i++)
    {
      // check if the listen file descriptor is ready for a new connection request
      if (reactor.events[i].data.ptr)
      {
        PollData_t *pInst = (PollData_t *)reactor.events[i].data.ptr;

        TRC_INFO(reactor.hTrc, "%s:0x%x fd %d", pInst->pPollDescription, pInst, pInst->fd);

        if(pInst->pCallBackFn)
        {
          // call read function
          pInst->pCallBackFn(pInst->fd, pInst->pData);
        }
        else
        {
          DBG_MAKE_ENTRY(FALSE);
        }
      }
      else
      {
        TRC_INFO(reactor.hTrc, "no data in event:%d", i);
      }
    }
  }
  return NULL;
}

void Reactor_DispatchAbort()
{
	// TODO
}

static const char *pPollCmd="poll";

/** monitor command called from in BKGR main init function */
BOOL Reactor_MonCmd(void * dummy, char * cmdLine)
{
	UINT8 argc;
	char** argv;
	MON_SplitArgs(cmdLine, &argc, &argv);
  static const char *pCmdToggleTrace = "t";
  if ((argc == 2) && (0 == strcmp(argv[1], pCmdToggleTrace)))
  {
    MON_WriteInfof("\nTrace level set to %d", reactor.traceLevel);
    reactor.traceLevel = TRC_SetTraceLevel(reactor.hTrc, reactor.traceLevel);
  }
  else
  {
    MON_WriteInfof("\n%s %s\ttoggle trace level", pPollCmd, pCmdToggleTrace);
  }
  return TRUE;
}

void Reactor_Init()
{
  bzero(&reactor, sizeof(reactor));
  reactor.epollfd = epoll_create(REACTOR_NOF);
  reactor.hTrc=TRC_AddTraceGroup((char *)pPollName);

}

