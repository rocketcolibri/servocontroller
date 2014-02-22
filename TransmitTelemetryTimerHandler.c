/**
 * TransmitTelemetryTimerHandler.c
 *
 *  Created on: 14.10.2013
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
#include "base/AD.h"
#include "base/DBG.h"
#include "base/Reactor.h"

#include "Connection.h"
#include "ConnectionContainer.h"
#include "TransmitTelemetryTimerHandler.h"
#define TRANSMIT_TELEMETRY_CMD_PORT 30002


/** data structure of a TransmitTelemetryTimerHanlder object */
typedef struct
{
	int sequenceNumber;
	UINT8 hTrc;
	ConnectionContainerObject_t connectionContainerObject;
	void *hTransmitTelemetryTimerPoll;
} transmitTelemetryTimerHandler_t;

static struct json_object* GetJsonUserAndIp(const char *pUser, struct sockaddr_in* ipAddress)
{
	json_object * jobj = json_object_new_object();
	json_object_object_add(jobj, "user", json_object_new_string( pUser));
	json_object_object_add(jobj, "ip", json_object_new_string( inet_ntoa (ipAddress->sin_addr)));
	return jobj;
}

static char * GetJsonTrasmitTelemetryMsg(int sequenceNumber, AVLTREE connections, ConnectionObject_t activeConnection)
{
	 json_object * jobj = json_object_new_object();
	 json_object_object_add(jobj, "v", json_object_new_int(1));
	 json_object_object_add(jobj, "cmd", json_object_new_string("tdc"));
	 json_object_object_add(jobj, "sequence", json_object_new_int( sequenceNumber));
	 if(activeConnection)
	 {
		 json_object_object_add(jobj, "activeip",
			 GetJsonUserAndIp( ConnectionGetUserName(activeConnection),
					 	 	   ConnectionGetAddress(activeConnection)));
	 }
	  json_object *jarray = json_object_new_array();
	  void addConnectionToArray(ConnectionObject_t connection)
	  {
		  if(activeConnection != connection)
		  {
			  json_object_array_add(jarray,
							  GetJsonUserAndIp( ConnectionGetUserName(connection),
							  					ConnectionGetAddress(connection)));
		  }
	  }
	  avlWalkAscending(connections, addConnectionToArray);
	  json_object_object_add(jobj,"availip", jarray);
	  char *pJsonStr = strdup(json_object_to_json_string(jobj));
	  json_object_put(jobj);
	  return pJsonStr;
}

static void SendToAll(transmitTelemetryTimerHandler_t *this, AVLTREE connections, const char *pJsonMsg)
{
  TRC_INFO(this->hTrc, "%s", pJsonMsg);
  void addConnectionToArray(ConnectionObject_t connection)
  {
	  struct sockaddr_in sendSock;
	  memcpy(&sendSock, ConnectionGetAddress(connection), sizeof(sendSock));
	  sendSock.sin_family = AF_INET;
	  sendSock.sin_port = htons(TRANSMIT_TELEMETRY_CMD_PORT);
	  sendSock.sin_addr.s_addr = htonl(INADDR_ANY );

	  sendto(ConnectionGetSocket(connection), pJsonMsg, strlen(pJsonMsg), 0,
			  (struct sockaddr*)ConnectionGetAddress(connection) , sizeof(struct sockaddr_in));

  }
  avlWalkAscending(ConnectionContainerGetAllConnections(this->connectionContainerObject), addConnectionToArray);
}

/**
 * @Override POLL_CallbackFunction_t
 * @param socketfd
 * @param connectionContainerObject this
 */
static void TransmitTelemetryTimerHandler(int socketfd, TransmitTelemetryTimerHandlerObject_t transmitTelemetryTimerHandlerObject)
{
	transmitTelemetryTimerHandler_t *pTransmitTelemetryTimer =(transmitTelemetryTimerHandler_t *)transmitTelemetryTimerHandlerObject;
	DBG_ASSERT(pTransmitTelemetryTimer);

	char *pJsonMsg = GetJsonTrasmitTelemetryMsg(
			pTransmitTelemetryTimer->sequenceNumber++,
			ConnectionContainerGetAllConnections(pTransmitTelemetryTimer->connectionContainerObject),
			ConnectionContainerGetActiveConnection(pTransmitTelemetryTimer->connectionContainerObject));

	SendToAll(pTransmitTelemetryTimer, ConnectionContainerGetAllConnections(pTransmitTelemetryTimer->connectionContainerObject), pJsonMsg);

	free(pJsonMsg);
	TIMERFD_Read(socketfd);
}

TransmitTelemetryTimerHandlerObject_t NewTransmitTelemetryTimerHandler(ConnectionContainerObject_t connectionContainerObject)
{
	transmitTelemetryTimerHandler_t *pTransmitTelemetryTimerHandler = malloc(sizeof(transmitTelemetryTimerHandler_t));
	bzero(pTransmitTelemetryTimerHandler, sizeof(pTransmitTelemetryTimerHandler));
	pTransmitTelemetryTimerHandler->connectionContainerObject = connectionContainerObject;
	pTransmitTelemetryTimerHandler->hTrc = TRC_AddTraceGroup("TelementryCmd");
	pTransmitTelemetryTimerHandler->hTransmitTelemetryTimerPoll
		= Reactor_AddReadFd(TIMERFD_Create(1000*1000), TransmitTelemetryTimerHandler, pTransmitTelemetryTimerHandler, "TransmitTelemetryHanlder");

	return (TransmitTelemetryTimerHandlerObject_t)pTransmitTelemetryTimerHandler;
}

void DeleteTransmitTelemetryTimerHandler(TransmitTelemetryTimerHandlerObject_t deleteTransmitTelemetryHandlerObject)
{
	transmitTelemetryTimerHandler_t *this = (transmitTelemetryTimerHandler_t *)deleteTransmitTelemetryHandlerObject;
	Reactor_RemoveFdAndClose(this->hTransmitTelemetryTimerPoll);
	free(this);
}
