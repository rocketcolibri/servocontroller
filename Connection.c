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

#include "Connection.h"
#include "ConnectionContainer.h"

#include "ServoDriver.h"

#include "MessageSinkCdc.h"

#define MAX_TIMEOUT_TIME 3

typedef enum { CONN_IDLE, CONN_IDENTIFIED, CONN_UP, CONN_DEGRADED_1, CONN_DEGRADED_2, CONN_DEGRADED_3 } connectionState_t;

typedef struct ConnectionContainer ConnectionContainer_t;

typedef struct Connection
{
  struct sockaddr_in srcAddress;
  int socketFd;
  char *pUserName;
  connectionState_t state;
  UINT32 lastSequence;
  UINT32 timeout;
  void *hConnectionTimeoutPoll;
  ConnectionContainerObject_t connectionContainer;
  UINT8 hTrc; // trace handle
} Connection_t;



#define CONNECTION_NOF_ACTION 2



/**
 * @short Rocket Colibri protocol states, see state diagram
 */
typedef enum
{
  CONNECTION_IDLE, // not active
  CONNECTION_UP, //
  CONNECTION_DEGRADED_1, //
  CONNECTION_IDENTIFIED, //
  CONNECTION_DEGRADED_2, //
  CONNECTION_DEGRADED_3, //
  CONNECTION_NOF_STATE
} Connection_State_t;

/**
 * @short List of all events
 */
typedef enum
{
  CONNECTION_EVENT_START, // start state machine
  CONNECTION_EVENT_OK, //
  CONNECTION_EVENT_FAIL, //
  CONNECTION_EVENT_STOP, //
  CONNECTION_NOF_EVENT
} Connection_Event_t;

/** state machine action function signaure */
typedef void (*Connection_ActionFn_t)(Connection_t *pArpingInst);

/** connection event action */
typedef struct
{
  Connection_ActionFn_t action[CONNECTION_NOF_ACTION + 1]; // +1 for NULL termination
  Connection_State_t nextState;
} Connection_EventAction_t;

void HandleJsonMessage(ConnectionObject_t connectionObject, const char *pJsonString)
{
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

          	ServoDriver_t *pServoDriver = ServoDriverGetInstance();
            MessageSinkCdc_t *pMsgCdc = NewMessageSinkCdc(jobj);
            pServoDriver->SetServos(GetNofChannel(pMsgCdc), GetChannelVector(pMsgCdc));
            DeleteMessageSinkCdc(pMsgCdc);
          }
          else if (0 == strcmp(json_object_get_string(command), "hello"))
          {
            // TODO
          }
          else if (0 == strcmp(json_object_get_string(command), "handover"))
          {
            // TODO
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

  static void ConnectionTimeoutHandler(int timerfd, void *pData)
  {
    Connection_t *this = (Connection_t *) pData;
    DBG_ASSERT(this);
    TIMERFD_Read(timerfd);
    if (this->timeout > MAX_TIMEOUT_TIME)
    {
    	 TRC_ERROR(this->hTrc, "timeout expired %s", this->pUserName);
    	ConnectionContainerRemoveConnection(this->connectionContainer, pData);
    	DeleteConnection(this);
    }
    else
    	this->timeout++;
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
    this->state = CONN_IDLE;
    this->hTrc = hTrcSocket; // inherits the trace handle from the socket
    this->hConnectionTimeoutPoll = Reactor_AddReadFd(TIMERFD_Create(1000 * 1000),
        ConnectionTimeoutHandler, this, "ConnectionTimeoutRx");

    return (ConnectionObject_t)this;
  }

 void DeleteConnection(ConnectionObject_t *pConn)
 {
	 Connection_t *this = (Connection_t *) pConn;
	 Reactor_RemoveFd(this->hConnectionTimeoutPoll);
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

