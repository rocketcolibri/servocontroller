/*
 * RCClient.c
 *
 *  Created on: 12.06.2014
 *      Author: lorenz
 */
#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <memory.h>
#include <unistd.h>
#include <fcntl.h>
#include <json-c/json.h>

#include "base/GEN.h"
#include "base/AD.h"
#include "base/MON.h"
#include "base/DBG.h"
#include "base/Reactor.h"
#include <json-c/json.h>

#define BUFLEN 512
#define NPACK 10
#define CONTROL_CMD_PORT 30001

typedef struct
{
	const char *pName;
	const char *pIpAddress;
	void *hSendTimer;
	void *hReactorReceiveMsg;
	int sequenceNumber;
	int fdSocket;
} RCClient_t;

/**
 * Creates a ControlCmd socket and binds to the port.
 */
static int OpenClientSocket(const char *pSrcIpAddress)
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


static char * GetJsonTrasmitHelloMsg(RCClient_t *this)
{
	 json_object * jobj = json_object_new_object();
	 json_object_object_add(jobj, "v", json_object_new_int(1));
	 json_object_object_add(jobj, "cmd", json_object_new_string("hello"));
	 json_object_object_add(jobj, "sequence", json_object_new_int( this->sequenceNumber++));
	 json_object_object_add(jobj, "user", json_object_new_string( this->pName));
	 char *pJsonStr = strdup(json_object_to_json_string(jobj));
	 json_object_put(jobj);
	 return pJsonStr;
}

static void HandleReactorSocketReceiveMsg(int socketfd, RCClientObject_t obj)
{

}

RCClientObject_t NewRcClient(const char *pName, const char *pSrcIpAddress)
{
	RCClient_t *this = malloc(sizeof(RCClient_t));
	bzero(this, sizeof(RCClient_t));
	this->pIpAddress = strdup(pSrcIpAddress);
	this->pName = strdup(pName);
	this->fdSocket = OpenClientSocket(pSrcIpAddress);
	this->hReactorReceiveMsg = Reactor_AddReadFd(this->fdSocket, HandleReactorSocketReceiveMsg, this, "ReceiveTelemetry");
	return this;
}

void DeleteRCClient(RCClientObject_t obj)
{
	RCClientDisconnect(obj);
	RCClient_t *this = (RCClient_t *)obj;
	Reactor_RemoveFdAndClose(this->hReactorReceiveMsg);
	Reactor_RemoveFdAndClose(this->hSendTimer);
	free(this->pIpAddress);
	free(this->pName);
	free(this);
}

/**
 * @Override Reactor_CallbackFunction_t
 * @param socketfd
 * @param RCClientObject_t obj
 */
static void HandleReactorTimerFdSendHello(int socketfd, RCClientObject_t obj)
{
	RCClient_t *this = (RCClient_t *)obj;
	DBG_ASSERT(this);
	char *pJsonMsg = GetJsonTrasmitHelloMsg(this);
	SendToServoController(this);
	free(pJsonMsg);
	TIMERFD_Read(socketfd);
}

void RCClientSendHello(RCClientObject_t obj)
{
	RCClient_t *this = (RCClient_t *)obj;
	if(this->hSendTimer)
	{
		Reactor_RemoveFdAndClose(this->hSendTimer);
		this->hSendTimer = NULL;
	}
	this->hSendTimer = Reactor_AddReadFd(TIMERFD_Create(1000*1000), HandleReactorTimerFdSendHello, this, "TransmitHello");
}

static char * GetJsonTrasmitCdcMsg(RCClient_t *this)
{
	 json_object * jobj = json_object_new_object();
	 json_object_object_add(jobj, "v", json_object_new_int(1));
	 json_object_object_add(jobj, "cmd", json_object_new_string("cdc"));
	 json_object_object_add(jobj, "sequence", json_object_new_int( this->sequenceNumber++));
	 json_object_object_add(jobj, "user", json_object_new_string( this->pName));
	 json_object* jarray = json_object_new_array();
	 int i;
	 for(i=0; i<8; i++)
		 json_object_array_add(jarray, json_object_new_int( 0));
	 json_object_object_add(jobj, "channels", jarray);
	 char *pJsonStr = strdup(json_object_to_json_string(jobj));
	 json_object_put(jobj);
	 return pJsonStr;
}

/**
 * @Override Reactor_CallbackFunction_t
 * @param socketfd
 * @param RCClientObject_t obj
 */
static void HandleReactorTimerFdSendCdc(int socketfd, RCClientObject_t obj)
{
	RCClient_t *this = (RCClient_t *)obj;
	DBG_ASSERT(this);
	char *pJsonMsg = GetJsonTrasmitCdcMsg(this);
	SendToServoController(this);
	free(pJsonMsg);
	TIMERFD_Read(socketfd);
}

void RCClientSendCdc(RCClientObject_t obj)
{
	RCClient_t *this = (RCClient_t *)obj;

	if(this->hSendTimer)
	{
		Reactor_RemoveFdAndClose(this->hSendTimer);
		this->hSendTimer = NULL;
	}

	this->hSendTimer
			= Reactor_AddReadFd(TIMERFD_Create(1000*1000), HandleReactorTimerFdSendCdc, this, "TransmitCdc");
}

void RCClientDisconnect(RCClientObject_t obj)
{
	RCClient_t *this = (RCClient_t *)obj;
	if(this->hSendTimer)
	{
		Reactor_RemoveFdAndClose(this->hSendTimer);
		this->hSendTimer = NULL;
	}
}

void RCClientSendExpectedActive(RCClientObject_t obj, const char *pActiveClientName)
{

}

void RCClientSendExpectedPassive(RCClientObject_t obj, const char *pPassiveClientName[])
{

}



