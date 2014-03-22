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

/** data structure of a TransmitTelemetryTimerHanlder object */
typedef struct
{
	int sequenceNumber;
	UINT8 hTrc;
	ConnectionContainerObject_t connectionContainerObject;
	void *hTransmitTelemetryTimerPoll;
} TransmitTelemetryTimerHandler_t;

static struct json_object* GetJsonUserAndIp(const char *pUser, struct sockaddr_in* ipAddress)
{
	json_object * jobj = json_object_new_object();
	json_object_object_add(jobj, "user", json_object_new_string( pUser ? pUser : ""));
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
	  json_object_object_add(jobj,"passiveip", jarray);
	  char *pJsonStr = strdup(json_object_to_json_string(jobj));
	  json_object_put(jobj);
	  return pJsonStr;
}

static void SendToAll(TransmitTelemetryTimerHandler_t *this, AVLTREE connections, const char *pJsonMsg)
{
  TRC_INFO(this->hTrc, "%s", pJsonMsg);
  void addConnectionToArray(ConnectionObject_t connection)
  {
	  sendto(ConnectionGetSocket(connection), pJsonMsg, strlen(pJsonMsg), 0,
			  (struct sockaddr*)ConnectionGetAddress(connection) , sizeof(struct sockaddr_in));
  }
  avlWalkAscending(ConnectionContainerGetAllConnections(this->connectionContainerObject), addConnectionToArray);
}

/**
 * @Override Reactor_CallbackFunction_t
 * @param socketfd
 * @param connectionContainerObject this
 */
static void TransmitTelemetryTimerHandler(int socketfd, TransmitTelemetryTimerHandlerObject_t transmitTelemetryTimerHandlerObject)
{
	TransmitTelemetryTimerHandler_t *this =(TransmitTelemetryTimerHandler_t *)transmitTelemetryTimerHandlerObject;
	DBG_ASSERT(this);

	char *pJsonMsg = GetJsonTrasmitTelemetryMsg(
			this->sequenceNumber++,
			ConnectionContainerGetAllConnections(this->connectionContainerObject),
			ConnectionContainerGetActiveConnection(this->connectionContainerObject));

	SendToAll(this, ConnectionContainerGetAllConnections(this->connectionContainerObject), pJsonMsg);

	free(pJsonMsg);
	TIMERFD_Read(socketfd);
}

TransmitTelemetryTimerHandlerObject_t NewTransmitTelemetryTimerHandler(ConnectionContainerObject_t connectionContainerObject)
{
	TransmitTelemetryTimerHandler_t *this = malloc(sizeof(TransmitTelemetryTimerHandler_t));
	bzero(this, sizeof(this));
	this->connectionContainerObject = connectionContainerObject;
	this->hTrc = TRC_AddTraceGroup("TelementryCmd");
	this->hTransmitTelemetryTimerPoll
		= Reactor_AddReadFd(TIMERFD_Create(1000*1000), TransmitTelemetryTimerHandler, this, "TransmitTelemetryHanlder");
	return (TransmitTelemetryTimerHandlerObject_t)this;
}

void DeleteTransmitTelemetryTimerHandler(TransmitTelemetryTimerHandlerObject_t deleteTransmitTelemetryHandlerObject)
{
	TransmitTelemetryTimerHandler_t *this = (TransmitTelemetryTimerHandler_t *)deleteTransmitTelemetryHandlerObject;
	Reactor_RemoveFdAndClose(this->hTransmitTelemetryTimerPoll);
	free(this);
}
