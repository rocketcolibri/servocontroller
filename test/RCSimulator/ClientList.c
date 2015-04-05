/*
 * ClientList.c
 *
 *  Created on: 13.06.2014
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
#include "json-c/json.h"

#include "base/GEN.h"
#include "base/TRC.h"
#include "base/DBG.h"
#include "base/MON.h"
#include "base/Reactor.h"
#include "base/AD.h"

#include "RCClient.h"
#include "ClientList.h"

/**
 * Data structure of the ConnectionContainer object
 */
typedef struct
{
  AVLTREE hClients; // storage for the RCClient objects
} ClientList_t;

static BOOL ClientListMonCmd(ClientListObject_t obj, char * cmdLine)
{
	ClientList_t* this = (ClientList_t*)obj;
	DBG_ASSERT(this);

	void traversClients(RCClientObject_t client)
	{
		DBG_ASSERT(client);
		MON_WriteInfof("\nrc:%s IP:%s", RCClientGetName(client), RCClientGetIpAddress(client));
	}

	avlWalkAscending(this->hClients, traversClients);
	return TRUE;
}

ClientListObject_t NewClientList()
{
	ClientList_t *this = malloc(sizeof(ClientList_t));
	bzero(this, sizeof(ClientList_t));
	this->hClients = avlNewTree(NULL, sizeof(UINT32), 0);
	MON_AddMonCmd("clientlist", ClientListMonCmd, this);
	return this;
}


ClientListObject_t NewClientListJson(struct json_object *pJsonObject)
{
	ClientList_t *this = (ClientList_t *)NewClientList();
	if(pJsonObject)
	{
		struct json_object *pRcArray = json_object_object_get(pJsonObject, "rocketcolibri" );
		int arrayLen = json_object_array_length(pRcArray);
		int i;
		for (i=0; i < arrayLen; i++)
		{
			RCClientObject_t client = NewRcClientJson(json_object_array_get_idx(pRcArray, i));
			if(client)
				avlInsert(this->hClients,(DSKEY)RCClientGetName(client), client);
		}
	}
	return this;
}

void DeleteClientList(ClientListObject_t obj)
{
	ClientList_t* this = (ClientList_t*)obj;
		DBG_ASSERT(this);
		// TODO:
}

void ClientListAddClient(ClientListObject_t obj, RCClientObject_t rcclient)
{
	ClientList_t* this = (ClientList_t*)obj;
	DBG_ASSERT(this);
	DBG_ASSERT(rcclient);
	DBG_ASSERT(NULL == avlFind(this->hClients, (DSKEY)RCClientGetName(rcclient)));
	avlInsert(this->hClients, (DSKEY)RCClientGetName(rcclient), rcclient);
}

RCClientObject_t ClientListRemoveClient(ClientListObject_t obj, const char *pClientName)
{
	ClientList_t* this = (ClientList_t*)obj;
	DBG_ASSERT(this);
	DBG_ASSERT(pClientName);
	return (RCClientObject_t)avlRemoveByKey(this->hClients, (DSKEY)pClientName);

}

RCClientObject_t ClientListFindClient(ClientListObject_t obj, const char *pClientName)
{
	ClientList_t* this = (ClientList_t*)obj;
	DBG_ASSERT(this);
	DBG_ASSERT(pClientName);
	return (RCClientObject_t)avlFind(this->hClients, (DSKEY)pClientName);
}
