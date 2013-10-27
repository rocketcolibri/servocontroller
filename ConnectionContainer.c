/*
 * ConnectionContainer.c
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

static int IpCmp(DSKEY ip1, DSKEY ip2)
{
	if(ip1 > ip2)
		return 1;
	else if (ip1 < ip2)
		return -1;
	else
		return 0;
}

ConnectionContainer_t *NewConnectionContainer()
{
	ConnectionContainer_t *pConnectioContainer = malloc(sizeof(ConnectionContainer_t));
	bzero(pConnectioContainer, sizeof(ConnectionContainer_t));
	pConnectioContainer->hAllConnections = avlNewTree(IpCmp, sizeof(UINT32), 0);
	return pConnectioContainer;
}

void DeleteConnectionContainer(ConnectionContainer_t *pConnectionContainer)
{
	DBG_ASSERT(pConnectionContainer);
}

BOOL ConnectionContainerHandover(ConnectionContainer_t *pConnectionContainer, struct sockaddr_in *pNewSrcAddr)
{
	DBG_ASSERT(pConnectionContainer);
	DBG_ASSERT(pNewSrcAddr);
	Connection_t *pNewActive = avlFind(pConnectionContainer->hAllConnections, (DSKEY)pNewSrcAddr->sin_addr.s_addr);
	if(pNewActive)
	{
		pConnectionContainer->pActiveConnection = pNewActive;
	}
	return FALSE;
}

Connection_t *ConnectionContainerFindConnection(ConnectionContainer_t *pConnectionContainer, struct sockaddr_in *pSrcAddr)
{
	DBG_ASSERT(pConnectionContainer);
	DBG_ASSERT(pSrcAddr);
	return (Connection_t *)avlFind(pConnectionContainer->hAllConnections, (DSKEY)pSrcAddr->sin_addr.s_addr);
}

void ConnectionContainerAddConnection(ConnectionContainer_t *pConnectionContainer, Connection_t *pConnection, struct sockaddr_in *pSrcAddr)
{
	DBG_ASSERT(pConnectionContainer);
	DBG_ASSERT(pConnection);
	DBG_ASSERT(pSrcAddr);
	DBG_ASSERT(NULL == avlFind(pConnectionContainer->hAllConnections, (DSKEY)pSrcAddr->sin_addr.s_addr));
	avlInsert(pConnectionContainer->hAllConnections, (DSKEY)pSrcAddr->sin_addr.s_addr, pConnection);
}

void ConnectionContainerRemoveConnection(ConnectionContainer_t *pConnectionContainer, Connection_t *pConnection)
{
	DBG_ASSERT(pConnectionContainer);
	DBG_ASSERT(pConnection);
	avlRemoveByKey(pConnectionContainer->hAllConnections, (DSKEY)pConnection->srcAddress.sin_addr.s_addr);
}
