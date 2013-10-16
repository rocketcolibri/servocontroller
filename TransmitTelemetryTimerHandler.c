/**
 * TransmitTelemetryTimerHandler.c
 *
 *  Created on: 14.10.2013
 *      Author: lorenz
 */

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <memory.h>
#include <unistd.h>

#include <json-c/json.h>

#include "base/GEN.h"
#include "base/DBG.h"
#include "base/POLL.h"

typedef struct
{
	void *hTransmitTelemetryTimerPoll;
} transmitTelemetryTimerHandler_t;

static void TransmitTelemetryTimerHandler(int socketfd, void *pData)
{
	transmitTelemetryTimerHandler_t *pTransmitTelemetryTimer =(transmitTelemetryTimerHandler_t *)pData;
	DBG_ASSERT(pTransmitTelemetryTimer);
	fprintf(stderr, ".");
	UINT32 expiredTime = TIMERFD_Read(socketfd);
}

void *NewTransmitTelemetryTimerHandler()
{
	transmitTelemetryTimerHandler_t *pTransmitTelemetryTimerHandler = malloc(sizeof(transmitTelemetryTimerHandler_t));
	bzero(pTransmitTelemetryTimerHandler, sizeof(pTransmitTelemetryTimerHandler));
	pTransmitTelemetryTimerHandler->hTransmitTelemetryTimerPoll
		= POLL_AddReadFd(TIMERFD_Create(100*1000), TransmitTelemetryTimerHandler, pTransmitTelemetryTimerHandler, "TransmitTelemetryHanlder");

	return pTransmitTelemetryTimerHandler;
}

void DeleteTransmitTelemetryTimerHandler(void *pDeleteTransmitTelemetryHandlerHandle)
{
	transmitTelemetryTimerHandler_t *pTransmitTelemetryTimerHandler = (transmitTelemetryTimerHandler_t *)pDeleteTransmitTelemetryHandlerHandle;
	POLL_RemoveFdAndClose(pTransmitTelemetryTimerHandler->hTransmitTelemetryTimerPoll);
	free(pTransmitTelemetryTimerHandler);
}
