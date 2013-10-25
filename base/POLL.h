/**
 * @addtogroup POLL
 *
 * @short Poll dispatcher
 *
 * The poll function provides the infrastructure for multiplexed IO. Multiple modules
 * can make use of the POLL system.
 * The file descriptor can be registered with the function POLL_AddReadFd, if the
 * file is ready for reading, the callback function passed to POLL_AddReadFd is
 * called to read and process the data.
 * @{
 */

#ifndef POLL_H
#define POLL_H

#include "GEN.h"

/**
 * init POLL module
 */
extern void POLL_Init();

/**
 * monitor command called from in BKGR main init function
 */
extern BOOL POLL_MonCmd(void * dummy, char * cmdLine);

/**
 * @short BKGR_Functions Typ
 *
 * This is the signature of the callback function of the poll thread. The function must have
 * a parameter with the file descriptor and a pointer to the data of the consumer
 *
 */
typedef void (* POLL_CallbackFunction_t)(int fd, void *pData);

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
extern void * POLL_AddReadFd(int fd, POLL_CallbackFunction_t pCallbackFn,void *pData, const char *pDescription);

/**
 * @short remove a file descriptor from the poll thread.
 * @param pPollDataHandle
 */
extern void POLL_RemoveFd(void *pPollDataHandle);

/**
 * @short remove a file descriptor from the poll list and close the file
 *
 * @param pPollDataHandle
 */
extern void POLL_RemoveFdAndClose(void *pPollDataHandle);

/**
 * @short Dispatch function
 *
 * endless loop with a polling wait that calles the Event handler as soon as a
 * file description in the poll list is ready.
 */
extern void* POLL_Dispatch();

/**
 * quits the endless poll loop
 */
extern void POLL_DispatchAbort();

/**
 * Same as POLL_RemoveFdAndClose, but the pPollDataHandle handle is set to NULL
 * @param pPollDataHandle
 */
#define POLL_RemoveFdAndCloseAndNull(pPollDataHandle) { BKGR_POLL_RemoveFdAndClose(pPollDataHandle); pPollDataHandle=NULL;}

/**
 * @short Create a timer file descriptor
 *
 * @param us time in micro seconds
 * @return timer file descriptor
 */
extern int TIMERFD_Create(UINT32 us);

/**
 * @short Reads the from the timer file descriptor
 * @param timerfd file descriptor
 * @return number of expired timer events
 */
extern UINT32 TIMERFD_Read(int timerfd);
/** @} */

#endif
