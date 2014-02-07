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
#include "base/AD.h"
#include "base/DBG.h"
#include "base/Reactor.h"

#include "Connection.h"
#include "ConnectionContainer.h"
#include "TransmitTelemetryTimerHandler.h"

/** data structure of a TransmitTelemetryTimerHanlder object */
typedef struct
{
	void *hTransmitTelemetryTimerPoll;
} transmitTelemetryTimerHandler_t;

/**
 * @Override POLL_CallbackFunction_t
 * @param socketfd
 * @param connectionContainerObject this
 */
static void TransmitTelemetryTimerHandler(int socketfd, ConnectionContainerObject_t connectionContainerObject)
{
	transmitTelemetryTimerHandler_t *pTransmitTelemetryTimer =(transmitTelemetryTimerHandler_t *)connectionContainerObject;
	DBG_ASSERT(pTransmitTelemetryTimer);
	//fprintf(stderr, ".");
	TIMERFD_Read(socketfd);
}

TransmitTelemetryTimerHandlerObject_t NewTransmitTelemetryTimerHandler(ConnectionContainerObject_t connectionContainerObject)
{
	transmitTelemetryTimerHandler_t *pTransmitTelemetryTimerHandler = malloc(sizeof(transmitTelemetryTimerHandler_t));
	bzero(pTransmitTelemetryTimerHandler, sizeof(pTransmitTelemetryTimerHandler));
	pTransmitTelemetryTimerHandler->hTransmitTelemetryTimerPoll
		= Reactor_AddReadFd(TIMERFD_Create(100*1000), TransmitTelemetryTimerHandler, pTransmitTelemetryTimerHandler, "TransmitTelemetryHanlder");

	return (TransmitTelemetryTimerHandlerObject_t)pTransmitTelemetryTimerHandler;
}

void DeleteTransmitTelemetryTimerHandler(TransmitTelemetryTimerHandlerObject_t deleteTransmitTelemetryHandlerObject)
{
	transmitTelemetryTimerHandler_t *pTransmitTelemetryTimerHandler = (transmitTelemetryTimerHandler_t *)deleteTransmitTelemetryHandlerObject;
	Reactor_RemoveFdAndClose(pTransmitTelemetryTimerHandler->hTransmitTelemetryTimerPoll);
	free(pTransmitTelemetryTimerHandler);
}
