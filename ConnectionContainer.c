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
#include "base/Reactor.h"
#include "base/AD.h"

#include "Connection.h"
#include "ConnectionContainer.h"

/** compare function for the AVLTREE, used IP addresses as the key (4 bytes) */
static int IpCmp(DSKEY ip1, DSKEY ip2)
{
	if(ip1 > ip2)
		return 1;
	else if (ip1 < ip2)
		return -1;
	else
		return 0;
}

/**
 * Data structure of the ConnectionContainer object
 */
typedef struct ConnectionContainer
{
  ConnectionObject_t *pActiveConnectionObject; // reference to the currently active connection
  AVLTREE hAllConnections; // storage for the connection objects
} ConnectionContainer_t;

ConnectionContainerObject_t NewConnectionContainer()
{
	ConnectionContainer_t *pConnectioContainer = malloc(sizeof(ConnectionContainer_t));
	bzero(pConnectioContainer, sizeof(ConnectionContainer_t));
	pConnectioContainer->hAllConnections = avlNewTree(IpCmp, sizeof(UINT32), 0);
	return (ConnectionContainerObject_t) pConnectioContainer;
}

void DeleteConnectionContainer(ConnectionContainerObject_t connectionContainerObject)
{
  ConnectionContainer_t* pConnectionContainer = (ConnectionContainer_t*)connectionContainerObject;
	DBG_ASSERT(pConnectionContainer);
	// TODO:
}

BOOL ConnectionContainerHandover(ConnectionContainerObject_t connectionContainerObject, struct sockaddr_in *pNewSrcAddr)
{
  ConnectionContainer_t* pConnectionContainer = (ConnectionContainer_t*)connectionContainerObject;
	DBG_ASSERT(pConnectionContainer);
	DBG_ASSERT(pNewSrcAddr);
	ConnectionContainerObject_t pNewActive = avlFind(pConnectionContainer->hAllConnections, (DSKEY)pNewSrcAddr->sin_addr.s_addr);
	if(pNewActive)
	{
		pConnectionContainer->pActiveConnectionObject = pNewActive;
	}
	return FALSE;
}

ConnectionContainerObject_t ConnectionContainerFindConnection(ConnectionContainerObject_t connectionContainerObject, struct sockaddr_in *pSrcAddr)
{
  ConnectionContainer_t* pConnectionContainer = (ConnectionContainer_t*)connectionContainerObject;
	DBG_ASSERT(pConnectionContainer);
	DBG_ASSERT(pSrcAddr);
	return (ConnectionContainerObject_t)avlFind(pConnectionContainer->hAllConnections, (DSKEY)pSrcAddr->sin_addr.s_addr);
}

void ConnectionContainerAddConnection(ConnectionContainerObject_t connectionContainerObject , ConnectionObject_t connectionObject, struct sockaddr_in *pSrcAddr)
{
  ConnectionContainer_t* pConnectionContainer = (ConnectionContainer_t*)connectionContainerObject;
	DBG_ASSERT(pConnectionContainer);
	DBG_ASSERT(connectionObject);
	DBG_ASSERT(pSrcAddr);
	DBG_ASSERT(NULL == avlFind(pConnectionContainer->hAllConnections, (DSKEY)pSrcAddr->sin_addr.s_addr));
	avlInsert(pConnectionContainer->hAllConnections, (DSKEY)pSrcAddr->sin_addr.s_addr, connectionObject);
}

void ConnectionContainerRemoveConnection(ConnectionContainerObject_t connectionContainerObject, ConnectionObject_t connectionObject, struct sockaddr_in *pSrcAddr)
{
  ConnectionContainer_t* pConnectionContainer = (ConnectionContainer_t*)connectionContainerObject;
	DBG_ASSERT(pConnectionContainer);
	DBG_ASSERT(connectionObject);
	avlRemoveByKey(pConnectionContainer->hAllConnections, (DSKEY)pSrcAddr->sin_addr.s_addr);
}
