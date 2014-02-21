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
#include <fcntl.h>
#include <json-c/json.h>

#include "base/GEN.h"
#include "base/AD.h"
#include "base/DBG.h"
#include "base/Reactor.h"

#include "Connection.h"
#include "ConnectionContainer.h"
#include "TransmitTelemetryTimerHandler.h"
#define TRANSMIT_TELEMETRY_CMD_PORT 30002

/**
 * Creates a ControlCmd socket and binds to the port.
 */
static int GetTransmitTelemetryCmdSocket()
{
	int sock, flag = 1;
	struct sockaddr_in sock_name;

	/* Create a datagram socket*/
	sock = socket(PF_INET, SOCK_DGRAM, 0);
	/* Setting the socket to non blocking*/
	fcntl(sock, F_SETFL, O_NONBLOCK);

	if (sock < 0)
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}
	/* Set the reuse flag. */
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) < 0)
	{
		perror("setsockopt(SOL_SOCKET, SO_REUSEADDR)");
		DBG_MAKE_ENTRY(TRUE);
	}
	/* Give the socket a name. */
	sock_name.sin_family = AF_INET;
	sock_name.sin_port = htons(TRANSMIT_TELEMETRY_CMD_PORT);
	sock_name.sin_addr.s_addr = htonl(INADDR_ANY );
	if (bind(sock, (struct sockaddr *) &sock_name, sizeof(sock_name)) < 0)
	{
		perror("bind");
		DBG_MAKE_ENTRY(TRUE);
	}
	return sock;
}

/** data structure of a TransmitTelemetryTimerHanlder object */
typedef struct
{
	int trasmitTelemetryFd;
	ConnectionContainerObject_t connectionContainerObject;
	void *hTransmitTelemetryTimerPoll;
} transmitTelemetryTimerHandler_t;

/**
 * @Override POLL_CallbackFunction_t
 * @param socketfd
 * @param connectionContainerObject this
 */
static void TransmitTelemetryTimerHandler(int socketfd, TransmitTelemetryTimerHandlerObject_t transmitTelemetryTimerHandlerObject)
{
	transmitTelemetryTimerHandler_t *pTransmitTelemetryTimer =(transmitTelemetryTimerHandler_t *)transmitTelemetryTimerHandlerObject;
	DBG_ASSERT(pTransmitTelemetryTimer);

	ConnectionObject_t activeConnection = ConnectionContainerGetActiveConnection(pTransmitTelemetryTimer->connectionContainerObject);

	AVLTREE connections = ConnectionContainerGetAllConnections(pTransmitTelemetryTimer->connectionContainerObject);

	void printConnection(ConnectionObject_t connection)
	{
#if 0
		if(activeConnection == connection)
		{
			fprintf(stderr, "\nActiveUser:%s", ConnectionGetUserName(connection));
		}
		else
			fprintf(stderr, "\nPassiveUser:%s", ConnectionGetUserName(connection));
#endif
	}

	if(avlSize(connections))
		avlWalkAscending(connections, printConnection);
	else
		fprintf(stderr, ".");

	if(activeConnection)
	{
		fprintf(stderr, "\nActiveUser:%s", ConnectionGetUserName(activeConnection));
	}
	TIMERFD_Read(socketfd);
}

TransmitTelemetryTimerHandlerObject_t NewTransmitTelemetryTimerHandler(ConnectionContainerObject_t connectionContainerObject)
{
	transmitTelemetryTimerHandler_t *pTransmitTelemetryTimerHandler = malloc(sizeof(transmitTelemetryTimerHandler_t));
	bzero(pTransmitTelemetryTimerHandler, sizeof(pTransmitTelemetryTimerHandler));
	pTransmitTelemetryTimerHandler->connectionContainerObject = connectionContainerObject;
	pTransmitTelemetryTimerHandler->hTransmitTelemetryTimerPoll
		= Reactor_AddReadFd(TIMERFD_Create(1000*1000), TransmitTelemetryTimerHandler, pTransmitTelemetryTimerHandler, "TransmitTelemetryHanlder");

	// open UDP socket for the Transmit telemetry commands
	pTransmitTelemetryTimerHandler->trasmitTelemetryFd = GetTransmitTelemetryCmdSocket();

	return (TransmitTelemetryTimerHandlerObject_t)pTransmitTelemetryTimerHandler;
}

void DeleteTransmitTelemetryTimerHandler(TransmitTelemetryTimerHandlerObject_t deleteTransmitTelemetryHandlerObject)
{
	transmitTelemetryTimerHandler_t *pTransmitTelemetryTimerHandler = (transmitTelemetryTimerHandler_t *)deleteTransmitTelemetryHandlerObject;
	Reactor_RemoveFdAndClose(pTransmitTelemetryTimerHandler->hTransmitTelemetryTimerPoll);
	free(pTransmitTelemetryTimerHandler);
}
