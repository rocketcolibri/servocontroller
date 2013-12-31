/*
 * ControlCommandRx.c
 *
 *  Created on: 13.10.2013
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
#include "base/DBG.h"
#include "base/POLL.h"
#include "base/AD.h"
#include "base/TRC.h"

#include "Connection.h"
#include "ConnectionContainer.h"
#include "ControlCommandRxSocket.h"

#define BUFLEN 512
#define NPACK 10
#define CONTROL_CMD_PORT 30001

/**
 * internal data structure of the ControlCommand socket object
 */
typedef struct
{
	UINT8 hTrc;
	void *hControlCmdRxPoll;
	void *hTimeoutControlCmdRxPoll;
	ConnectionContainerObject_t connectionContainer;
} getControlCommandRx_t;

/**
 * Creates a ControlCmd socket and binds to the port.
 */
static int GetControlCmdSocket()
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
	sock_name.sin_port = htons(CONTROL_CMD_PORT);
	sock_name.sin_addr.s_addr = htonl(INADDR_ANY );
	if (bind(sock, (struct sockaddr *) &sock_name, sizeof(sock_name)) < 0)
	{
		perror("bind");
		DBG_MAKE_ENTRY(TRUE);
	}
	return sock;
}

/**
 * This method is called on reception of a message on the ControlComand socket
 * @Override POLL_CallbackFunction_t
 * @param socketfd socket file descriptor
 * @param socketObj this
 */
static void ControlCommandRxSocketHandler(int socketfd, ControlCommandRxSocketObject_t socketObj)
{
	getControlCommandRx_t *pControlCommandRxSocket=(getControlCommandRx_t *)socketObj;

	DBG_ASSERT(pControlCommandRxSocket);
	struct sockaddr_in srcAddr;
	socklen_t srcAddrLen;
	char buffer[1500];
	ssize_t rxLen=0;
	if(0 < (rxLen=recvfrom(socketfd, buffer, sizeof(buffer), MSG_DONTWAIT,
			(struct sockaddr*)&srcAddr, &srcAddrLen)))
	{
		ConnectionObject_t pConn=ConnectionContainerFindConnection(pControlCommandRxSocket->connectionContainer, &srcAddr);

		if (!pConn)
		{
			pConn = NewConnection(pControlCommandRxSocket->connectionContainer, &srcAddr, pControlCommandRxSocket->hTrc);
			ConnectionContainerAddConnection(pControlCommandRxSocket->connectionContainer, pConn, &srcAddr);
			TRC_INFO(pControlCommandRxSocket->hTrc, "connection created:%s",inet_ntoa(srcAddr.sin_addr));
		}
		else
		{
			TRC_INFO(pControlCommandRxSocket->hTrc, "connection found:%s",inet_ntoa(srcAddr.sin_addr));
		}

		buffer[rxLen]=0;
		HandleJsonMessage(pConn, buffer);

		TRC_INFO(pControlCommandRxSocket->hTrc, "received %d bytes containing:%s from %s",rxLen, buffer, inet_ntoa(srcAddr.sin_addr));
	}
}

ControlCommandRxSocketObject_t NewControlCommandRxSocket(ConnectionContainerObject_t pConnectionContainerObject)
{
	getControlCommandRx_t *pControlCommandRx = malloc(sizeof(getControlCommandRx_t));
	bzero(pControlCommandRx, sizeof(pControlCommandRx));
	pControlCommandRx->connectionContainer = pConnectionContainerObject;
	pControlCommandRx->hControlCmdRxPoll = POLL_AddReadFd(GetControlCmdSocket(),
			ControlCommandRxSocketHandler, pControlCommandRx, "ControlCmdRx");
	pControlCommandRx->hTrc = TRC_AddTraceGroup("rxSocket");
	return (ControlCommandRxSocketObject_t)pControlCommandRx;
}

void DeleteControlCommandRxSocket(ControlCommandRxSocketObject_t controlCommandRxHandlerObject)
{
  getControlCommandRx_t *pControlCommandRx =
			(getControlCommandRx_t *) controlCommandRxHandlerObject;
	POLL_RemoveFdAndClose(pControlCommandRx->hControlCmdRxPoll);
	POLL_RemoveFdAndClose(pControlCommandRx->hTimeoutControlCmdRxPoll);
	DeleteConnectionContainer(pControlCommandRx->connectionContainer);
	free(controlCommandRxHandlerObject);
}
