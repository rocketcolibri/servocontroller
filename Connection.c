/*
 * Connection.h
 *
 *  Created on: 15.10.2013
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




void HandleJsonMessage(const Connection_t *pConn, const char *pJsonString)
{

}

static void ConnectionTimeoutHandler(int timerfd20ms, void *pData)
{
	Connection_t *pConn = (Connection_t *) pData;
	DBG_ASSERT(pConn);
	fprintf(stderr, "C");
	UINT32 expiredTime = TIMERFD_Read(timerfd20ms);
}


Connection_t *NewConnection(const ConnectionContainer_t *pContainerConnection, const struct sockaddr_in *pSrcAddr)
{
	DBG_ASSERT(pContainerConnection);
	DBG_ASSERT(pSrcAddr);
	Connection_t *pConn = malloc(sizeof(Connection_t));
	bzero(pConn, sizeof(Connection_t));
	pConn->pConnectionContainer = pContainerConnection;
	pConn->srcAddress = *pSrcAddr;
	pConn->state = CONN_IDLE;
	pConn->hConnectionTimeoutPoll = POLL_AddReadFd(
			TIMERFD_Create(40 * 1000), ConnectionTimeoutHandler, pConn, "ConnectionTimeoutRx");

	return pConn;
}


