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
#include "base/TRC.h"
#include "base/DBG.h"
#include "base/MON.h"
#include "base/Reactor.h"
#include "base/AD.h"

#include "Connection.h"
#include "ConnectionContainer.h"
#include "SystemFsm.h"
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
  ConnectionObject_t activeConnectionObject; // reference to the currently active connection
  AVLTREE hAllConnections; // storage for the connection objects
  SystemFsmObject_t sysSm;
} ConnectionContainer_t;

void SYS_Action_1_SetServosToPassiveMode(ConnectionContainerObject_t this)
{

}

/** monitor command called from in BKGR main init function */
static BOOL ConnectionContainer_MonCmd(ConnectionContainerObject_t obj, char * cmdLine)
{
	ConnectionContainer_t *this = (ConnectionContainer_t*)obj;
	if(this->activeConnectionObject)
		MON_WriteInfof("\nactiveip: %s (%s)", ConnectionGetUserName(this->activeConnectionObject), inet_ntoa(ConnectionGetAddress(this->activeConnectionObject)->sin_addr));
	else
		MON_WriteInfof("\nactiveip: -");

	void monPrintConnectionToArray(ConnectionObject_t connection)
	{
		if(this->activeConnectionObject != connection)
			MON_WriteInfof("\npassive: %s (%s)", ConnectionGetUserName(connection), inet_ntoa(ConnectionGetAddress(connection)->sin_addr));
	}
	avlWalkAscending(this->hAllConnections, monPrintConnectionToArray);
	return TRUE;
}

ConnectionContainerObject_t NewConnectionContainer()
{
	ConnectionContainer_t *this = malloc(sizeof(ConnectionContainer_t));
	bzero(this, sizeof(ConnectionContainer_t));
	this->hAllConnections = avlNewTree(IpCmp, sizeof(UINT32), 0);
	this->sysSm = NewSystemFsm(this, SYS_Action_1_SetServosToPassiveMode);
	MON_AddMonCmd("cc", ConnectionContainer_MonCmd, this );
	return (ConnectionContainerObject_t) this;
}

void DeleteConnectionContainer(ConnectionContainerObject_t connectionContainerObject)
{
  ConnectionContainer_t* this = (ConnectionContainer_t*)connectionContainerObject;
	DBG_ASSERT(this);
	// TODO:
}

BOOL ConnectionContainerHandover(ConnectionContainerObject_t connectionContainerObject, struct sockaddr_in *pNewSrcAddr)
{
  ConnectionContainer_t* this = (ConnectionContainer_t*)connectionContainerObject;
	DBG_ASSERT(this);
	DBG_ASSERT(pNewSrcAddr);
	ConnectionObject_t newActiveConnection = avlFind(this->hAllConnections, (DSKEY)pNewSrcAddr->sin_addr.s_addr);
	if(newActiveConnection)
	{
		this->activeConnectionObject = newActiveConnection;
	}
	return FALSE;
}

ConnectionContainerObject_t ConnectionContainerFindConnection(ConnectionContainerObject_t connectionContainerObject, struct sockaddr_in *pSrcAddr)
{
  ConnectionContainer_t* this = (ConnectionContainer_t*)connectionContainerObject;
	DBG_ASSERT(this);
	DBG_ASSERT(pSrcAddr);
	return (ConnectionContainerObject_t)avlFind(this->hAllConnections, (DSKEY)pSrcAddr->sin_addr.s_addr);
}

void ConnectionContainerAddConnection(ConnectionContainerObject_t connectionContainerObject , ConnectionObject_t connectionObject, struct sockaddr_in *pSrcAddr)
{
  ConnectionContainer_t* this = (ConnectionContainer_t*)connectionContainerObject;
	DBG_ASSERT(this);
	DBG_ASSERT(connectionObject);
	DBG_ASSERT(pSrcAddr);
	DBG_ASSERT(NULL == avlFind(this->hAllConnections, (DSKEY)pSrcAddr->sin_addr.s_addr));
	avlInsert(this->hAllConnections, (DSKEY)pSrcAddr->sin_addr.s_addr, connectionObject);
}

void ConnectionContainerRemoveConnection(ConnectionContainerObject_t connectionContainerObject, ConnectionObject_t connectionObject, struct sockaddr_in *pSrcAddr)
{
  ConnectionContainer_t* this = (ConnectionContainer_t*)connectionContainerObject;
	DBG_ASSERT(this);
	DBG_ASSERT(connectionObject);
	avlRemoveByKey(this->hAllConnections, (DSKEY)pSrcAddr->sin_addr.s_addr);
}

AVLTREE ConnectionContainerGetAllConnections(ConnectionContainerObject_t connectionContainerObject)
{
	ConnectionContainer_t* this = (ConnectionContainer_t*)connectionContainerObject;
	DBG_ASSERT(this);
	return this->hAllConnections;
}

ConnectionObject_t ConnectionContainerGetActiveConnection(ConnectionContainerObject_t connectionContainerObject)
{
	ConnectionContainer_t* this = (ConnectionContainer_t*)connectionContainerObject;
	DBG_ASSERT(this);
	return this->activeConnectionObject;
}

void ConnectionContainerSetActiveConnection(
		ConnectionContainerObject_t connectionContainerObject,
		ConnectionObject_t newActiveConnection)
{
	ConnectionContainer_t* this = (ConnectionContainer_t*)connectionContainerObject;
	DBG_ASSERT(this);
	DBG_ASSERT(newActiveConnection);
	if(this->activeConnectionObject != newActiveConnection)
	{
		TRC_Log_Print(TRC_log, "%s: set new active connection %s ",__PRETTY_FUNCTION__,
				inet_ntoa(ConnectionGetAddress(newActiveConnection)->sin_addr));
		this->activeConnectionObject = newActiveConnection;
	}
}

