/*
 * ConnectionFsm.c
 *
 *  Created on: 23.03.2014
 *      Author: lorenz
 */

/*
		ConnectionFsm_ActionFn_t A2_CCSetActiveConnection,
 * SystemState.c
 *
 *  Created on: 23.03.2014
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
#include <json-c/json.h>


#include "base/GEN.h"
#include "base/TRC.h"
#include "base/AD.h"
#include "base/DBG.h"

#include "ConnectionFsm.h"

#define SYS_NOF_ACTION 3


/**
 * @short Rocket Colibri protocol states, see state diagram
 */
typedef enum
{
  CONN_IDLE, // not active
  CONN_IDENTIFIED,
  CONN_ACTIVE, //
  CONN_FAILSAFE,
  CONN_NOF_STATE
} Connection_State_t;

/**
 * @short List of all events
 */
typedef enum
{
  CONN_EVENT_HELLO_CMD_RECV, // start state machine
  CONN_EVENT_CDC_CMD_RECV, //
  CONN_EVENT_TIMEOUT,
  CONN_EVENT_PROTO_IVALID_CMD,
  CONN_NOF_EVENT
} Connection_Event_t;

/** connection event action */
typedef struct
{
	ConnectionFsm_ActionFn_t action[SYS_NOF_ACTION + 1]; // +1 for NULL termination
	Connection_State_t nextState;
} Connection_EventAction_t;

typedef struct
{
	Connection_State_t state;
	ConnectionObject_t connection;
	Connection_EventAction_t ConnectionFsm[CONN_NOF_STATE][CONN_NOF_EVENT];
	ConnectionFsm_ActionFn_t terminateAfterThisAction;
} ConnectionFsm_t;



ConnectionFsmObject_t NewConnectionFsm(
		ConnectionObject_t connection,
		ConnectionFsm_ActionFn_t A1_CCAddConnection,
		ConnectionFsm_ActionFn_t A2_CCSetActiveConnection,
		ConnectionFsm_ActionFn_t A3_CCSetBackToPassivConnection,
		ConnectionFsm_ActionFn_t A4_CCRemoveConnection,
		ConnectionFsm_ActionFn_t A5_ActionDeleteConnection,
		ConnectionFsm_ActionFn_t A6_SetServoToFailsafe)
{
	ConnectionFsm_t * this = malloc(sizeof(ConnectionFsm_t));
	bzero(this, sizeof(ConnectionFsm_t));
	this->state = CONN_IDLE;
	this->connection = connection;

	// after this action has been called the FSM object is deleted!
	this->terminateAfterThisAction = A5_ActionDeleteConnection;

    Connection_EventAction_t ConnectionFsm[CONN_NOF_STATE][CONN_NOF_EVENT] =
	{
	  /* state: CONN_IDLE */
	{ /* events                              actions0                        actions1                  actions2     		next state */
	  /* CONN_EVENT_RECV_HELLO_CMD*/      {{ A1_CCAddConnection, 			       NULL,                     NULL,					NULL },      CONN_IDENTIFIED },
	  /* CONN_EVENT_RECV_CDC_CMD*/        {{ A5_ActionDeleteConnection,      NULL,                     NULL,					NULL },      CONN_IDLE },
	  /* CONN_EVENT_TIMEOUT*/             {{ A5_ActionDeleteConnection,      NULL,					           NULL,					NULL },      CONN_IDLE },
	  /* CONN_EVENT_PROTO_IVALID_CMD*/    {{ A5_ActionDeleteConnection,      NULL,					           NULL,					NULL },      CONN_IDLE },
	},

	  /* state: CONN_IDENTIFIED */
	{ /* events                              actions0                       actions1                  actions2     							next state */
	  /* CONN_EVENT_RECV_HELLO_CMD*/      {{ NULL,                          NULL,                     NULL,						NULL },      CONN_IDENTIFIED },
	  /* CONN_EVENT_RECV_CDC_CMD*/        {{ A2_CCSetActiveConnection,  	NULL,                     NULL,						NULL },      CONN_ACTIVE },
	  /* CONN_EVENT_TIMEOUT*/             {{ A4_CCRemoveConnection, 		A5_ActionDeleteConnection,NULL,						NULL },      CONN_IDLE },
	  /* CONN_EVENT_PROTO_IVALID_CMD*/    {{ A4_CCRemoveConnection,   		A5_ActionDeleteConnection,NULL, 					NULL },      CONN_IDLE },
	},

	/* state: CONN_ACTIVE */
	{ /* events                              actions0                        actions1                  actions2     							next state */
	  /* CONN_EVENT_RECV_HELLO_CMD*/      {{ A3_CCSetBackToPassivConnection, NULL,                     NULL,					 NULL },      CONN_IDENTIFIED },
	  /* CONN_EVENT_RECV_CDC_CMD*/        {{ NULL,                           NULL,                     NULL,					 NULL },      CONN_ACTIVE },
	  /* CONN_EVENT_TIMEOUT*/             {{ A6_SetServoToFailsafe,          NULL,                     NULL,					 NULL },      CONN_FAILSAFE },
	  /* CONN_EVENT_PROTO_IVALID_CMD*/    {{ A4_CCRemoveConnection,      	 A5_ActionDeleteConnection, NULL,					NULL },       CONN_IDLE },
	},
	/* state: CONN_FAILSAFE */
	{ /* events                              actions0                        actions1                  actions2     							next state */
	  /* CONN_EVENT_RECV_HELLO_CMD*/      {{ A3_CCSetBackToPassivConnection, NULL,                     NULL,					 NULL },      CONN_IDENTIFIED },
	  /* CONN_EVENT_RECV_CDC_CMD*/        {{ NULL,                           NULL,                     NULL,					 NULL },      CONN_ACTIVE },
	  /* CONN_EVENT_TIMEOUT*/             {{ NULL,                           NULL,                     NULL,					 NULL },      CONN_FAILSAFE },
	  /* CONN_EVENT_PROTO_IVALID_CMD*/    {{ A4_CCRemoveConnection,      	 A5_ActionDeleteConnection, NULL,					 NULL },      CONN_IDLE },
	}
	};
    memcpy(&this->ConnectionFsm , &ConnectionFsm, sizeof(ConnectionFsm));
	return (ConnectionFsmObject_t) this;
}

void DeleteConnectionFsm(ConnectionFsmObject_t obj)
{
	DBG_ASSERT(obj);
	free(obj);
}

/**
 * Execute state machine event
 * @param pArpingInst
 * @param event
 */
static void ExecuteEvent(ConnectionFsmObject_t obj,
    Connection_Event_t event)
{
	DBG_ASSERT(event < CONN_NOF_EVENT);
	DBG_ASSERT(obj);
	ConnectionFsm_t *this = (ConnectionFsm_t *)obj;

	static const char* statesStr[CONN_NOF_STATE] = {"CONN_IDLE", "CONN_IDENTIFIED", "CONN_ACTIVE", "CONN_FAILSAFE"};
	static const char* eventsStr[CONN_NOF_EVENT] = { "EVENT_HELLO_CMD_RECV", "EVENT_CDC_CMD_RECV", "CONN_EVENT_TIMEOUT", "CONN_EVENT_PROTO_IVALID_CMD" };
	Connection_State_t currentState = this->state;

	// get new state
	Connection_State_t newState = this->ConnectionFsm[currentState][event].nextState;

	// execute actions
	UINT32 action = 0;
	while (this->ConnectionFsm[currentState][event].action[action])
	{
		if(this->terminateAfterThisAction == this->ConnectionFsm[currentState][event].action[action])
		{
			this->ConnectionFsm[currentState][event].action[action](this);
			return;
		}
		else
			this->ConnectionFsm[currentState][event].action[action](this);

		action++;
	}

	if(this->state != newState)
	{
		DBG_LOG_ENTRY_FMT("Connection: event %s: state %s to %s", eventsStr[event], statesStr[this->state], statesStr[newState]);
		this->state = newState;
	}
}

BOOL ConnectionFsmIs_CONN_IDLE(ConnectionFsmObject_t obj)
{
	DBG_ASSERT(obj);
	ConnectionFsm_t *this = (ConnectionFsm_t *)obj;
	return CONN_IDLE == this->state;
}


BOOL ConnectionFsmIs_CONN_IDENTIFIED(ConnectionFsmObject_t obj)
{
	DBG_ASSERT(obj);
	ConnectionFsm_t *this = (ConnectionFsm_t *)obj;
	return CONN_IDENTIFIED == this->state;
}

BOOL ConnectionFsmIs_CONN_ACTIVE(ConnectionFsmObject_t obj)
{
	DBG_ASSERT(obj);
	ConnectionFsm_t *this = (ConnectionFsm_t *)obj;
	return CONN_ACTIVE == this->state;
}

BOOL ConnectionFsmIs_CONN_FAILSAFE(ConnectionFsmObject_t obj)
{
	DBG_ASSERT(obj);
	ConnectionFsm_t *this = (ConnectionFsm_t *)obj;
	return CONN_FAILSAFE == this->state;
}

void ConnectionFsmEventRecvHelloCmd(ConnectionFsmObject_t obj)
{
	DBG_ASSERT(obj);
	ExecuteEvent(obj, CONN_EVENT_HELLO_CMD_RECV);
}

void ConnectionFsmEventRecvCdcCmd(ConnectionFsmObject_t obj)
{
	DBG_ASSERT(obj);
	ExecuteEvent(obj, CONN_EVENT_CDC_CMD_RECV);
}

void ConnectionFsmEventTimeout(ConnectionFsmObject_t obj)
{
	DBG_ASSERT(obj);
	ExecuteEvent(obj, CONN_EVENT_TIMEOUT);
}

void ConnectionFsmEventInvalidCmd(ConnectionFsmObject_t obj)
{
	DBG_ASSERT(obj);
	ExecuteEvent(obj, CONN_EVENT_PROTO_IVALID_CMD);
}

ConnectionObject_t ConnectionFsmGetConnection(ConnectionFsmObject_t obj)
{
	ConnectionFsm_t *this = (ConnectionFsm_t *)obj;
	return this->connection;
}
