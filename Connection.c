/*
 * Connection.c
 *
 * Implements the RocketColibri protocol on the receiver side.
 *
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
#include "base/TRC.h"
#include "base/Reactor.h"
#include "base/AD.h"

#include "SystemFsm.h"
#include "Connection.h"
#include "ConnectionContainer.h"
#include "ConnectionFsm.h"
#include "ServoDriver.h"

#include "MessageSinkCdc.h"

#define MAX_TIMEOUT_TIME 3

typedef struct ConnectionContainer ConnectionContainer_t;

typedef struct Connection
{
  struct sockaddr_in srcAddress;
  int socketFd;
  char *pUserName;
  UINT32 lastSequence;
  UINT32 timeout;
  void *hConnectionTimeoutPoll;
  ConnectionContainerObject_t connectionContainer;
  ConnectionFsmObject_t connectionFsm;
  UINT8 hTrc; // trace handle
} Connection_t;



#define CONNECTION_NOF_ACTION 2


void HandleJsonMessage(ConnectionObject_t connectionObject, const char *pJsonString)
{
  DBG_ASSERT(connectionObject);
  Connection_t *this = (Connection_t*) connectionObject;

  struct json_tokener *tok = json_tokener_new();
  struct json_object * jobj = json_tokener_parse_ex(tok, pJsonString, strlen(pJsonString));
  if (!jobj)
  {
    TRC_ERROR(this->hTrc,
        "JSON error: %s %s", (char *)json_tokener_error_desc(json_tokener_get_error(tok)), (char *)pJsonString);
    return;
  }
  json_object * version = json_object_object_get(jobj, "v");
  if ((version) && (1 == json_object_get_int(version)))
  {
    json_object *pUserObj = json_object_object_get(jobj, "user");
    if (pUserObj)
    {
      if (NULL == this->pUserName)
      {
        this->pUserName = strdup(json_object_get_string(pUserObj));
      }
      else
      {
        // user name must remain the same
        DBG_ASSERT_NO_RES(
            0==strcmp(this->pUserName, json_object_get_string(pUserObj)));
      }
      json_object_put(pUserObj);
    }

    json_object *seqObj = json_object_object_get(jobj, "sequence");
    if (seqObj)
    {
      int sequence = json_object_get_int(seqObj);
      if (this->lastSequence < sequence)
      {
        // here comes the interesting part, parsing the command and call the command process function

        // here comes the interesting part, parsing the command and call the command process function
        json_object *command = json_object_object_get(jobj, "cmd");
        if (command)
        {
          if (0 == strcmp(json_object_get_string(command), "cdc"))
          {

        	this->lastSequence = sequence;
          	TRC_INFO(this->hTrc, "cdc: user:%s sequence:%d", this->pUserName, this->lastSequence);

            if(SystemFsmIs_SYS_IDLE(ConnectionContainerGetSystemFsm(this->connectionContainer)))
            {
            	ConnectionFsmEventRecvCdcCmd(this->connectionFsm);
            }
            else if (SystemFsmIs_SYS_CONTROLLING(ConnectionContainerGetSystemFsm(this->connectionContainer)))
			{
              	ServoDriver_t *pServoDriver = ServoDriverGetInstance();
                MessageSinkCdc_t *pMsgCdc = NewMessageSinkCdc(jobj);
                pServoDriver->SetServos(GetNofChannel(pMsgCdc), GetChannelVector(pMsgCdc));
                DeleteMessageSinkCdc(pMsgCdc);
                this->timeout = 0; // reset timeout counter
			}
            else
            {
            	DBG_MAKE_ENTRY(FALSE);
            }
          }
          else if (0 == strcmp(json_object_get_string(command), "hello"))
          {
        	  ConnectionFsmEventRecvHelloCmd(this->connectionFsm);
        	  this->timeout = 0; // reset timeout counter
          }
          else
          {
            TRC_ERROR(this->hTrc,
                "JSON unknown command: %s", json_object_get_string(command));
            return;
          }
          json_object_put(command);
        }
      }
      else
      {
        TRC_ERROR(this->hTrc,
            "JSON invalid sequence number last:%d current:%d", this->lastSequence, sequence);
      }
      json_object_put(seqObj);
    }
    else
    {
    	DBG_MAKE_ENTRY(FALSE);
    }
    json_object_put(jobj);
  }
  else
  {
   	DBG_MAKE_ENTRY(FALSE);
  }
  json_tokener_free(tok);
}

  static void ConnectionTimeoutHandler(int timerfd, void *obj)
  {
    Connection_t *this = (Connection_t *) obj;
    DBG_ASSERT(this);
    TIMERFD_Read(timerfd);
    if (this->timeout > MAX_TIMEOUT_TIME)
    {
    	ConnectionFsmEventTimeout(this->connectionFsm);
    	TRC_ERROR(this->hTrc, "timeout expired %s", this->pUserName);
    }
    else
    {
    	TRC_ERROR(this->hTrc, "this:0x%x timeout:%d",this, this->timeout);
    	this->timeout++;
    }
  }

static void A1_ActionFunctionSendToActive(ConnectionFsmObject_t obj)
{
	Connection_t *pConnection = (Connection_t *)ConnectionFsmGetConnection(obj);
	ConnectionContainerSetActiveConnection(pConnection->connectionContainer, pConnection);
	SystemFsmEventTransitionToActive(ConnectionContainerGetSystemFsm(pConnection->connectionContainer));
}

static void A2_ActionFunctionSendToPassive(ConnectionFsmObject_t obj)
{
	Connection_t *pConnection = (Connection_t *)ConnectionFsmGetConnection(obj);
	ConnectionContainerSetActiveConnection(pConnection->connectionContainer, NULL);
	SystemFsmEventTransitionToPassive(ConnectionContainerGetSystemFsm(pConnection->connectionContainer));
}

static void A3_ActionDeleteConnection(ConnectionFsmObject_t obj)
{
	Connection_t *pConnection = (Connection_t *)ConnectionFsmGetConnection(obj);
	ConnectionContainerRemoveConnection(pConnection->connectionContainer, pConnection);
	DeleteConnection(pConnection);
}

  ConnectionObject_t NewConnection(const ConnectionContainerObject_t containerConnectionObject,
      const struct sockaddr_in *pSrcAddr, const int socketFd,  UINT8 hTrcSocket)
  {
    DBG_ASSERT(containerConnectionObject);
    DBG_ASSERT(pSrcAddr);
    Connection_t *this = malloc(sizeof(Connection_t));
    bzero(this, sizeof(Connection_t));
    this->connectionContainer = containerConnectionObject;
    this->srcAddress = *pSrcAddr;
    this->socketFd = socketFd;
    this->hTrc = hTrcSocket; // inherits the trace handle from the socket
    this->hConnectionTimeoutPoll = Reactor_AddReadFd(TIMERFD_Create(1000 * 1000),
        ConnectionTimeoutHandler, this, "ConnectionTimeoutRx");

    this->connectionFsm = NewConnectionFsm(
		this,
    	A1_ActionFunctionSendToActive,
    	A2_ActionFunctionSendToPassive,
    	A3_ActionDeleteConnection);

    return (ConnectionObject_t)this;
  }

 void DeleteConnection(ConnectionObject_t *pConn)
 {
	 Connection_t *this = (Connection_t *) pConn;
	 Reactor_RemoveFd(this->hConnectionTimeoutPoll);
	 DeleteConnectionFsm(this->connectionFsm);
	 free(pConn);
 }
  // getter
  struct sockaddr_in* ConnectionGetAddress(ConnectionObject_t connectionObject)
  {
	  return &((Connection_t*) connectionObject)->srcAddress;
  }

  int ConnectionGetSocket(ConnectionObject_t connectionObject)
  {
	  return ((Connection_t*) connectionObject)->socketFd;
  }

  char* ConnectionGetUserName(ConnectionObject_t connectionObject)
  {
  	  return ((Connection_t*) connectionObject)->pUserName;
  }

