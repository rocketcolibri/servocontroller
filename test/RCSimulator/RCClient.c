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
#include <errno.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <memory.h>
#include <unistd.h>
#include <fcntl.h>

#include "json-c/json.h"

#include "base/GEN.h"
#include "base/AD.h"
#include "base/MON.h"
#include "base/DBG.h"
#include "base/Reactor.h"

#include "RCClient.h"
#include "ServoControllerData.h"

#define BUFLEN 512
#define NPACK 10

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
	int s;
	if((s = socket(AF_INET , SOCK_DGRAM , IPPROTO_UDP ) ) == -1)
	{
		DBG_MAKE_ENTRY(TRUE);
	}
	// bind to pSrcIpAddress
	/* bind to an arbitrary return address */
	/* because this is the client side, we don't care about the address */
	/* since no application will initiate communication here - it will */
	/* just send responses */
	/* INADDR_ANY is the IP address and 0 is the socket */
	/* htonl converts a long integer (e.g. address) to a network representation */
	/* htons converts a short integer (e.g. port) to a network representation */
	struct sockaddr_in myaddr;
	bzero((char *)&myaddr, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	if (0== inet_aton(pSrcIpAddress, &myaddr.sin_addr))
	{
		DBG_MAKE_ENTRY(FALSE);

	}
	const int trueValue = 1;
	setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &trueValue, sizeof(trueValue));
	myaddr.sin_port = htons(SERVOCONTROLLER_PORT);
	if (bind(s, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
		DBG_MAKE_ENTRY_FMT(FALSE, "bind failed (%s)", strerror(errno));
	}
	return s;
}

void SendMsgToServoController(RCClient_t *this, const char*pJsonMsg)
{
	if (-1 == sendto(this->fdSocket, pJsonMsg, strlen(pJsonMsg), 0,(const struct sockaddr *)ServoControllerDataGetSockAddr(), sizeof(struct sockaddr_in)))
	{
		DBG_MAKE_ENTRY_MSG(FALSE, strerror(errno));
	}
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

RCClientObject_t NewRcClientJson(struct json_object* pJsonObj)
{
	return NewRcClient(
			json_object_get_string( json_object_object_get(pJsonObj,"name")),
			json_object_get_string( json_object_object_get(pJsonObj,"ipaddress")));
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
	free((char*)this->pIpAddress);
	free((char*)this->pName);
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
	SendMsgToServoController(this, pJsonMsg);
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
	this->hSendTimer = Reactor_AddReadFd(TIMERFD_Create(100*1000), HandleReactorTimerFdSendHello, this, "TransmitHello");
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
	SendMsgToServoController(this, pJsonMsg);
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
	this->hSendTimer = Reactor_AddReadFd(TIMERFD_Create(20*1000), HandleReactorTimerFdSendCdc, this, "TransmitCdc");
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

const char * RCClientGetName(RCClientObject_t obj)
{
	RCClient_t *this = (RCClient_t *)obj;
	return this->pName;
}

const char *RCClientGetIpAddress(RCClientObject_t obj)
{
	RCClient_t *this = (RCClient_t *)obj;
	return this->pIpAddress;
}
