/*
 * ControlCommandRx.c
 *
 *  Created on: 13.10.2013
 *      Author: lorenz
 */
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <memory.h>
#include <unistd.h>
#include <fcntl.h>
#include <json-c/json.h>

#include "base/GEN.h"
#include "base/DBG.h"
#include "base/Reactor.h"
#include "base/AD.h"
#include "base/TRC.h"

#include "SystemFsm.h"
#include "Connection.h"
#include "ConnectionContainer.h"
#include "MessageReceiver.h"

#define BUFLEN 512
#define NPACK 10
#define CONTROL_CMD_PORT 30001
#define CONTROL_CMD_PORT_STR "30001"
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
	//int sock=0;
	const char* hostname=0; /* wildcard */
	const char* portname="daytime";
	struct addrinfo hints;
	memset(&hints,0,sizeof(hints));
	hints.ai_family=AF_UNSPEC;
	hints.ai_socktype=SOCK_DGRAM;
	hints.ai_protocol=0;
	hints.ai_flags=AI_PASSIVE|AI_ADDRCONFIG;
	struct addrinfo* res=0;
	int err=getaddrinfo(hostname,CONTROL_CMD_PORT_STR,&hints,&res);
	if (err!=0)
	{
		perror("getaddrinfo");
		exit(EXIT_FAILURE);
	}

	int sock=socket(res->ai_family,res->ai_socktype,res->ai_protocol);
	if (sock==-1) {
		perror("socket");
		exit(EXIT_FAILURE);
	}
	int flag = 1;
	/* Set the reuse flag. */
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) < 0)
	{
		perror("setsockopt(SOL_SOCKET, SO_REUSEADDR)");
		DBG_MAKE_ENTRY(TRUE);
	}

	if (bind(sock,res->ai_addr,res->ai_addrlen)==-1)
	{
		perror("bind");
		DBG_MAKE_ENTRY(TRUE);
	}
	freeaddrinfo(res);

	return sock;
}

/**
 * This method is called on reception of a message on the ControlComand socket
 * @Override Reactor_CallbackFunction_t
 * @param socketfd socket file descriptor
 * @param socketObj this
 */
static void MessageReceiverHandler(int socketfd, MessageReceiverObject_t socketObj)
{
	getControlCommandRx_t *pMessageReceiver=(getControlCommandRx_t *)socketObj;

	DBG_ASSERT(pMessageReceiver);
	struct sockaddr srcAddr;
	socklen_t srcAddrLen =  sizeof(srcAddr);
	char buffer[1500];
	ssize_t rxLen=0;

	if(0 < (rxLen=recvfrom(socketfd, buffer, sizeof(buffer), MSG_DONTWAIT, &srcAddr, &srcAddrLen)))
	{

		ConnectionObject_t pConn=ConnectionContainerFindConnection(pMessageReceiver->connectionContainer, &srcAddr);

		if (!pConn)
		{

			pConn = NewConnection(pMessageReceiver->connectionContainer, &srcAddr, srcAddrLen, socketfd, pMessageReceiver->hTrc);
			TRC_Log_Print(TRC_log, "%s: new connection created:%s",__PRETTY_FUNCTION__, ConnectionGetAddressName(pConn));
		}
		else
		{
			TRC_INFO(pMessageReceiver->hTrc, "connection found:%s",ConnectionGetAddressName(pConn));
		}

		buffer[rxLen]=0;
		HandleJsonMessage(pConn, buffer);
		TRC_INFO(pMessageReceiver->hTrc, "received %d bytes containing:%s from %s",rxLen, buffer, ConnectionGetAddressName(pConn));
	}
}

MessageReceiverObject_t NewMessageReceiver(ConnectionContainerObject_t pConnectionContainerObject)
{
	getControlCommandRx_t *pControlCommandRx = malloc(sizeof(getControlCommandRx_t));
	bzero(pControlCommandRx, sizeof(pControlCommandRx));
	pControlCommandRx->connectionContainer = pConnectionContainerObject;
	pControlCommandRx->hControlCmdRxPoll = Reactor_AddReadFd(GetControlCmdSocket(),
			MessageReceiverHandler, pControlCommandRx, "ControlCmdRx");
	pControlCommandRx->hTrc = TRC_AddTraceGroup("rxSocket");
	return (MessageReceiverObject_t)pControlCommandRx;
}

void DeleteMessageReceiver(MessageReceiverObject_t controlCommandRxHandlerObject)
{
  getControlCommandRx_t *pControlCommandRx =
			(getControlCommandRx_t *) controlCommandRxHandlerObject;
	Reactor_RemoveFdAndClose(pControlCommandRx->hControlCmdRxPoll);
	Reactor_RemoveFdAndClose(pControlCommandRx->hTimeoutControlCmdRxPoll);
	DeleteConnectionContainer(pControlCommandRx->connectionContainer);
	free(controlCommandRxHandlerObject);
}
