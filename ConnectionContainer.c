/*
 * ConnectionContainer.c
 *
 *  Created on: 15.10.2013
 *      Author: lorenz
 */
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <memory.h>
#include <unistd.h>
#include <fcntl.h>

#include "json-c/json.h"

#include "base/GEN.h"
#include "base/TRC.h"
#include "base/DBG.h"
#include "base/MON.h"
#include "base/Reactor.h"
#include "base/AD.h"

#include "SystemFsm.h"
#include "Connection.h"
#include "ConnectionContainer.h"


#define SOCK_ADDR_IN6_PTR(sa)	((struct sockaddr_in6 *)(sa))
#define SOCK_ADDR_IN6_ADDR(sa)	SOCK_ADDR_IN6_PTR(sa)->sin6_addr
#define SOCK_ADDR_IN_PTR(sa)	((struct sockaddr_in *)(sa))
#define SOCK_ADDR_IN_ADDR(sa)	SOCK_ADDR_IN_PTR(sa)->sin_addr

/**
 * this functions compares 2 ip addresses. IPV4 and IPV6 is supported
 */
static int sock_addr_cmp_addr(const struct sockaddr * sa, const struct sockaddr * sb)
{
  if (sa->sa_family != sb->sa_family)
    return (sa->sa_family - sb->sa_family);

  /*
   * With IPv6 address structures, assume a non-hostile implementation that
   * stores the address as a contiguous sequence of bits. Any holes in the
   * sequence would invalidate the use of memcmp().
   */
  if (sa->sa_family == AF_INET)
  {
    return (SOCK_ADDR_IN_ADDR(sa).s_addr - SOCK_ADDR_IN_ADDR(sb).s_addr);
  }
  else if (sa->sa_family == AF_INET6)
  {
     return (memcmp((char *) &(SOCK_ADDR_IN6_ADDR(sa)), (char *) &(SOCK_ADDR_IN6_ADDR(sb)), sizeof(SOCK_ADDR_IN6_ADDR(sa))));
  }
  else
  {
    DBG_MAKE_ENTRY_FMT(TRUE, "sock_addr_cmp_addr: unsupported address family %d", sa->sa_family);
  }
  return 0;
}

/** compare function for the AVLTREE, used IP addresses as the key (4 bytes) */
static int IpCmp(DSKEY ip1, DSKEY ip2)
{
	return(sock_addr_cmp_addr((const struct sockaddr *) ip1, (const struct sockaddr *) ip2));
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

	MON_WriteInfof("\nSystem State: ", SystemFsmIs_SYS_CONTROLLING(this->sysSm) ? "ACTIVE" : "IDLE");

	if(this->activeConnectionObject)
		MON_WriteInfof("\nactiveip: %s (%s)", ConnectionGetUserName(this->activeConnectionObject), ConnectionGetAddressName(this->activeConnectionObject));
	else
		MON_WriteInfof("\nactiveip: -");

	void monPrintConnectionToArray(ConnectionObject_t connection)
	{
		if(this->activeConnectionObject != connection)
			MON_WriteInfof("\npassive: %s (%s)", ConnectionGetUserName(connection), ConnectionGetAddressName(connection));
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

BOOL ConnectionContainerHandover(ConnectionContainerObject_t connectionContainerObject, struct sockaddr *pNewSrcAddr)
{
  ConnectionContainer_t* this = (ConnectionContainer_t*)connectionContainerObject;
	DBG_ASSERT(this);
	DBG_ASSERT(pNewSrcAddr);
	ConnectionObject_t newActiveConnection = avlFind(this->hAllConnections, (DSKEY)pNewSrcAddr);
	if(newActiveConnection)
	{
		this->activeConnectionObject = newActiveConnection;
	}
	return FALSE;
}

ConnectionContainerObject_t ConnectionContainerFindConnection(ConnectionContainerObject_t connectionContainerObject, struct sockaddr *pSrcAddr)
{
  ConnectionContainer_t* this = (ConnectionContainer_t*)connectionContainerObject;
	DBG_ASSERT(this);
	DBG_ASSERT(pSrcAddr);
	return (ConnectionContainerObject_t)avlFind(this->hAllConnections, (DSKEY)pSrcAddr);
}

void ConnectionContainerAddConnection(ConnectionContainerObject_t connectionContainerObject , ConnectionObject_t connectionObject, struct sockaddr *pSrcAddr)
{
  ConnectionContainer_t* this = (ConnectionContainer_t*)connectionContainerObject;
	DBG_ASSERT(this);
	DBG_ASSERT(connectionObject);
	DBG_ASSERT(pSrcAddr);
	DBG_ASSERT(NULL == avlFind(this->hAllConnections, (DSKEY)pSrcAddr));
	avlInsert(this->hAllConnections, (DSKEY)pSrcAddr, connectionObject);
}

void ConnectionContainerRemoveConnection(ConnectionContainerObject_t connectionContainerObject, ConnectionObject_t connectionObject)
{
	ConnectionContainer_t* this = (ConnectionContainer_t*)connectionContainerObject;
	DBG_ASSERT(this);
	DBG_ASSERT(connectionObject);
	if(this->activeConnectionObject == connectionObject)
	{
		this->activeConnectionObject = NULL;
	}
	struct sockaddr*  key = ConnectionGetAddress(connectionObject);
	if(NULL ==avlRemoveByKey(this->hAllConnections, (DSKEY)key))
	{
		DBG_MAKE_ENTRY_MSG(FALSE,ConnectionGetUserName(connectionObject) );
	}
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

BOOL ConnectionContainerIsActiveConnection(
		ConnectionContainerObject_t connectionContainerObject,
		ConnectionObject_t connection)
{
	ConnectionContainer_t* this = (ConnectionContainer_t*)connectionContainerObject;
	DBG_ASSERT(this);
	return (this->activeConnectionObject == connection);
}


void ConnectionContainerSetActiveConnection(
		ConnectionContainerObject_t connectionContainerObject,
		ConnectionObject_t newActiveConnection)
{
	ConnectionContainer_t* this = (ConnectionContainer_t*)connectionContainerObject;
	DBG_ASSERT(this);
	if(this->activeConnectionObject != newActiveConnection)
	{
		if(newActiveConnection)
		{
			DBG_LOG_ENTRY_FMT( "set new active connection user %s with IP %s ",
				ConnectionGetUserName(newActiveConnection),
				ConnectionGetAddressName(newActiveConnection));
		}
		else
		{
			DBG_LOG_ENTRY_FMT( "disconnect user %s with IP %s ",
				ConnectionGetUserName(this->activeConnectionObject),
				ConnectionGetAddressName(this->activeConnectionObject));
		}
		this->activeConnectionObject = newActiveConnection;
	}
}

SystemFsmObject_t ConnectionContainerGetSystemFsm(ConnectionContainerObject_t connectionContainerObject)
{
	ConnectionContainer_t* this = (ConnectionContainer_t*)connectionContainerObject;
	return this->sysSm;
}
