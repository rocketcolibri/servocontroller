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
#include <json-c/json.h>

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



ClientListObject_t NewClientList(struct json_object *pJsonObject)
{
	ClientList_t *this = malloc(sizeof(ClientList_t));
	bzero(this, sizeof(ClientList_t));
	this->hClients = avlNewTree(NULL, sizeof(UINT32), 0);

	struct json_object *pRcArray = json_object_object_get(pJsonObject, "rocketcolibri" );
	int arrayLen = json_object_array_length(pRcArray);
	int i;
	for (i=0; i < arrayLen; i++)
	{
		RCClientObject_t client = NewRcClient_FromJson(json_object_array_get_idx(pRcArray, i));
		if(client)
			avlInsert(this->hClients,(DSKEY)GetName(client), client);
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
	DBG_ASSERT(NULL == avlFind(this->hClients, (DSKEY)GetName(rcclient)));
	avlInsert(this->hClients, (DSKEY)GetName(rcclient), rcclient);
}

void ClientListRemoveClient(ClientListObject_t obj, const char *pClientName)
{
	ClientList_t* this = (ClientList_t*)obj;
	DBG_ASSERT(this);
	DBG_ASSERT(pClientName);
	avlRemoveByKey(this->hClients, (DSKEY)pClientName);

}

RCClientObject_t ClientListFindClient(ClientListObject_t obj, const char *pClientName)
{
	ClientList_t* this = (ClientList_t*)obj;
	DBG_ASSERT(this);
	DBG_ASSERT(pClientName);
	return (RCClientObject_t)avlFind(this->hClients, (DSKEY)pClientName);
}
