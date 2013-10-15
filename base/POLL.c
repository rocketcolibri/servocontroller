/**
 * @addtogroup  BKGR-Poll
 *
 * @short BKGR poll thread
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
#include "TRC.h"
#include "POLL.h"

#define POLL_NOF  100

static const char* pPollName = "poll";

/** module data */
static struct
{
    struct epoll_event events[POLL_NOF]; // epoll event struct
    int epollfd; // epoll file descriptor
    pthread_t hArpingThread; // thread handle
    pthread_mutex_t pStateMachineMutex; // state machnie mutex
    UINT8 hTrc; // trace handle
    UINT8 traceLevel; // and level
} bkgr_poll;


/** module data */
typedef struct
{
    int fd;
    POLL_CallbackFunction_t pCallBackFn;
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
void * POLL_AddReadFd(int fd, POLL_CallbackFunction_t pCallbackFn,void *pData, const char *pDescription)
{
  DBG_ASSERT(pCallbackFn);
  DBG_ASSERT(pDescription);
  DBG_ASSERT(fd);

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
  if(-1 == epoll_ctl(bkgr_poll.epollfd, EPOLL_CTL_ADD, pPollData->fd, &pPollData->event))
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
void POLL_RemoveFd(void *pPollDataHandle)
{
  DBG_ASSERT(pPollDataHandle);
  PollData_t * pPollData = (PollData_t*)pPollDataHandle;
  TRC_INFO(bkgr_poll.hTrc, "%s:0x%x removed fd:%d",pPollData->pPollDescription,  pPollData, pPollData->fd );

    // close pipe
  if(pPollData->fd)
  {
    epoll_ctl(bkgr_poll.epollfd, EPOLL_CTL_DEL, pPollData->fd, NULL);
  }

  free(pPollData);
}

/**
 * @short remove a file descriptor from the poll list and close the file
 *
 * @param pPollDataHandle
 */
void POLL_RemoveFdAndClose(void *pPollDataHandle)
{
  int fd = ((PollData_t*)pPollDataHandle)->fd;
  POLL_RemoveFd(pPollDataHandle);
  close(fd);
}

/**
 * Waits for an event in the poll list and calls the callback function associated with
 * the file descriptor.
 */
void* POLL_Dispatch()
{
  for (;;)
  {
    // wait until at least one file descriptor is ready
    UINT ready = epoll_wait(bkgr_poll.epollfd, bkgr_poll.events, POLL_NOF, -1);
    if (ready == -1)
    {
      DBG_MAKE_ENTRY_MSG(FALSE, strerror(errno));
      continue;
    }

    TRC_INFO(bkgr_poll.hTrc, "Epoll ready:%d", ready);

    // go trough the file descriptor list
    int i;
    for (i = 0; i < ready; i++)
    {
      // check if the listen file descriptor is ready for a new connection request
      if (bkgr_poll.events[i].data.ptr)
      {
        PollData_t *pInst = (PollData_t *)bkgr_poll.events[i].data.ptr;

        TRC_INFO(bkgr_poll.hTrc, "%s:0x%x fd %d", pInst->pPollDescription, pInst, pInst->fd);

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
        TRC_INFO(bkgr_poll.hTrc, "no data in event:%d", i);
      }
    }
  }
  return NULL;
}

void POLL_Init()
{
  bzero(&bkgr_poll, sizeof(bkgr_poll));
  bkgr_poll.epollfd = epoll_create(POLL_NOF);
  bkgr_poll.hTrc=TRC_AddTraceGroup((char *)pPollName);
}
#if 0
/** monitor command called from in BKGR main init function */
BOOL POLL_MonCmd(int argc, char **argv)
{
  static const char *pCmdToggleTrace = "t";
  if ((argc == 1) && (0 == strcmp(argv[0], pCmdToggleTrace)))
  {
    MON_WriteInfof("\nTrace level set to %d", bkgr_poll.traceLevel);
    bkgr_poll.traceLevel = TRC_SetTraceLevel(bkgr_poll.hTrc, bkgr_poll.traceLevel);
  }
  else
  {
    MON_WriteInfof("\n%s %s %s\ttoggle trace level", pBkgrMonCmd, pBkgrPollCmd, pCmdToggleTrace);
  }
  return TRUE;
}
#endif
