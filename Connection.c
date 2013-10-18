/*
 * Connection.h
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
#include "base/DBG.h"
#include "base/TRC.h"
#include "base/POLL.h"
#include "base/AD.h"

#include "Connection.h"
#include "ConnectionContainer.h"

#define CONNECTION_NOF_ACTION 2

/**
 * @short Master state machine states
 */
typedef enum
{
  CONNECTION_IDLE, // not active
  CONNECTION_IDENTIFIED, // try to reach host
  CONNECTION_UP, // host not reached for the first time
  CONNECTION_DEGRADED_1, // host not reached for the 2nd time
  CONNECTION_DEGRADED_2, // host not reached for the 3rd time
  CONNECTION_DEGRADED_3, // connection to host
  CONNECTION_NOF_STATE
} Connection_State_t;

/**
 * @short Master event type
 */
typedef enum
{
  CONNECTION_EVENT_START, // start state machine
  CONNECTION_EVENT_OK, // mac address received from host
  CONNECTION_EVENT_FAIL, // no arp reply from host
  CONNECTION_EVENT_STOP, // stop state machine
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

void Connection_DummyFn(Connection_t *pArpingInst)
{

}

/**
 * @short Master State Machine
 */
// !!! the first NULL in an event line terminates the action list !!!
static const Connection_EventAction_t connectionStateMachine[CONNECTION_NOF_STATE][CONNECTION_NOF_EVENT] =
{ /* state 0: CONNECTION_STATE_DISABLED */
{ /* events                      actions 0               actions 1                 next state */
  /* CONNECTION_IDLE*/        {{ Connection_DummyFn,     NULL, NULL },             CONNECTION_IDLE },
  /* CONNECTION_IDENTIFIED*/  {{ NULL, NULL, NULL },                               CONNECTION_IDLE },
  /* CONNECTION_UP */         {{ NULL, NULL, NULL },                               CONNECTION_IDLE },
  /* CONNECTION_DEGRADED_1 */ {{ NULL, NULL, NULL },                               CONNECTION_IDLE },
  /* CONNECTION_DEGRADED_2 */ {{ NULL, NULL, NULL },                               CONNECTION_IDLE },
  /* CONNECTION_DEGRADED_3 */ {{ NULL, NULL, NULL },                               CONNECTION_IDLE }
} };

/**
 * Execute state machine event
 * @param pArpingInst
 * @param event
 */
static void Connection_ExecuteEvent(Connection_t *pArpingInst,
    Connection_Event_t event)
{
  static const char* statesStr[] =
  { "DISABLED", "RESOLVING", "FAIL_1", "FAIL_2", "FAIL_3", "RESOLVED" };

  static const char* eventsStr[] =
  { "START", "OK", "FAIL", "STOP" };

  DBG_ASSERT(event < CONNECTION_NOF_EVENT);
  DBG_ASSERT(pArpingInst);

  Connection_State_t currentState = pArpingInst->state;

  // get new state
  Connection_State_t newState =
      connectionStateMachine[currentState][event].nextState;

  TRC_INFO(pArpingInst->hTrc,
      "0x%x\t Event:%s %s -> %s", pArpingInst, eventsStr[event], statesStr[currentState], statesStr[newState]);
  // execute actions
  UINT32 action = 0;
  while (connectionStateMachine[currentState][event].action[action])
  {
    connectionStateMachine[currentState][event].action[action](pArpingInst);
    action++;
  }

  pArpingInst->state = newState;
}

static void ParseCdcCommand(json_object * jobj)
{
  json_object *pChannelArray = json_object_object_get(jobj, "channels");
  int u;
  for (u=0; u<json_object_array_length(pChannelArray); u++)
  {
    json_object *pChannel = json_object_array_get_idx(pChannelArray, u);
    // set servos ere
    // TODO: ServoSetChannel(u, json_object_get_int(pChannel);
    json_object_put(pChannel);
  }
  json_object_put(pChannelArray);
}

static void ParseHelloCommand(json_object * jobj)
{

}

static void ParseHandoverCommand(json_object * jobj)
{

}

void HandleJsonMessage(Connection_t *pConn, const char *pJsonString)
{
  struct json_tokener *tok = json_tokener_new();
  struct json_object * jobj = json_tokener_parse_ex(tok, pJsonString,
      strlen(pJsonString));
  if (!jobj)
  {
    TRC_ERROR(pConn->hTrc,
        "JSON error: %s %s", (char *)json_tokener_error_desc(json_tokener_get_error(tok)), (char *)pJsonString);
    return;
  }
  json_object * version = json_object_object_get(jobj, "v");
  if ((version) && (1 == json_object_get_int(version)))
  {
    json_object *pUserObj = json_object_object_get(jobj, "user");
    if (pUserObj)
    {
      if (NULL == pConn->pUserName)
      {
        pConn->pUserName = strdup(json_object_get_string(pUserObj));
      }
      else
      {
        // user name must remain the same
        DBG_ASSERT_NO_RES(
            0==strcmp(pConn->pUserName, json_object_get_string(pUserObj)));
      }
      json_object_put(pUserObj);
    }

    json_object *seqObj = json_object_object_get(jobj, "seq");
    if (seqObj)
    {
      int sequence = json_object_get_int(seqObj);
      if (pConn->lastSequence < sequence)
      {
        // here comes the interesting part, parsing the command and call the command process function

        // here comes the interesting part, parsing the command and call the command process function
        json_object *command = json_object_object_get(jobj, "cmd");
        if (command)
        {
          if (0 == strcmp(json_object_get_string(command), "cdc"))
          {
            ParseCdcCommand(jobj);
          }
          else if (0 == strcmp(json_object_get_string(command), "hello"))
          {
            ParseHelloCommand(jobj);
          }
          else if (0 == strcmp(json_object_get_string(command), "handover"))
          {
            ParseHandoverCommand(jobj);
          }
          else
          {
            TRC_ERROR(pConn->hTrc,
                "JSON unknown command: %s", json_object_get_string(command));
            return;
          }
          json_object_put(command);
        }
      }
      else
      {
        TRC_ERROR(pConn->hTrc,
            "JSON invalid sequence number last:% current:%", pConn->lastSequence, sequence);
      }

      json_object_put(sequence);
    }
    json_object_put(jobj);
  }
  json_tokener_free(tok);
  }

  static void ConnectionTimeoutHandler(int timerfd20ms, void *pData)
  {
    Connection_t *pConn = (Connection_t *) pData;
    DBG_ASSERT(pConn);
    fprintf(stderr, "C");
    UINT32 expiredTime = TIMERFD_Read(timerfd20ms);
  }

  Connection_t *NewConnection(const ConnectionContainer_t *pContainerConnection,
      const struct sockaddr_in *pSrcAddr)
  {
    DBG_ASSERT(pContainerConnection);
    DBG_ASSERT(pSrcAddr);
    Connection_t *pConn = malloc(sizeof(Connection_t));
    bzero(pConn, sizeof(Connection_t));
    pConn->pConnectionContainer = pContainerConnection;
    pConn->srcAddress = *pSrcAddr;
    pConn->state = CONN_IDLE;
    pConn->hConnectionTimeoutPoll = POLL_AddReadFd(TIMERFD_Create(40 * 1000),
        ConnectionTimeoutHandler, pConn, "ConnectionTimeoutRx");

    return pConn;
  }

