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

#include "Connection.h"
#include "ConnectionContainer.h"

#define BUFLEN 512
#define NPACK 10
#define CONTROL_CMD_PORT 30001

typedef struct
{
	UINT8 dummy;
	void *hControlCmdRxPoll;
	void *hTimeoutControlCmdRxPoll;
	ConnectionContainer_t *connectionContainer;
} getControlCommandRx_t;

int GetControlCmdSocket()
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

static void ControlCommandRxSocketHandler(int socketfd, void *pData)
{
	getControlCommandRx_t *pControlCommandRxSocket=(getControlCommandRx_t *)pData;

	DBG_ASSERT(pControlCommandRxSocket);
	struct sockaddr_in srcAddr;
	socklen_t srcAddrLen;
	char buffer[1500];
	ssize_t rxLen=0;
	if(0 < (rxLen=recvfrom(socketfd, buffer, sizeof(buffer), MSG_DONTWAIT,
			&srcAddr, &srcAddrLen)))
	{
		Connection_t *pConn=ConnectionContainerFindConnection(pControlCommandRxSocket->connectionContainer, &srcAddr);

		if (!pConn)
		{
			pConn = NewConnection(pControlCommandRxSocket->connectionContainer, &srcAddr);
			ConnectionContainerAddConnection(pControlCommandRxSocket->connectionContainer, pConn, &srcAddr);
		}

		buffer[rxLen]=0;
		HandleJsonMessage(pConn, buffer);

		fprintf(stderr, "\nreceived %d bytes containing:%s from %s",rxLen, buffer, inet_ntoa(srcAddr.sin_addr));
	}
}


void *NewControlCommandRxSocket()
{
	getControlCommandRx_t *pControlCommandRx = malloc(sizeof(getControlCommandRx_t));
	bzero(pControlCommandRx, sizeof(pControlCommandRx));
	pControlCommandRx->connectionContainer = NewConnectionContainer();
	pControlCommandRx->hControlCmdRxPoll = POLL_AddReadFd(GetControlCmdSocket(),
			ControlCommandRxSocketHandler, pControlCommandRx, "ControlCmdRx");


	return pControlCommandRx;
}

void DeleteControlCommandRxSocket(void *pControlCommandRxHandlerHandle)
{
	getControlCommandRx_t *pControlCommandRx =
			(getControlCommandRx_t *) pControlCommandRxHandlerHandle;
	POLL_RemoveFdAndClose(pControlCommandRx->hControlCmdRxPoll);
	POLL_RemoveFdAndClose(pControlCommandRx->hTimeoutControlCmdRxPoll);
	DeleteConnectionContainer(pControlCommandRx->connectionContainer);
	free(pControlCommandRxHandlerHandle);
}
