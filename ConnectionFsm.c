/*
 * ConnectionFsm.c
 *
 *  Created on: 23.03.2014
 *      Author: lorenz
 */

/*
 * SystemState.c
 *
 *  Created on: 23.03.2014
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

#include "ConnectionFsm.h"

#define SYS_NOF_ACTION 2


/**
 * @short Rocket Colibri protocol states, see state diagram
 */
typedef enum
{
  CONN_IDENTIFIED, // not active
  CONN_ACTIVE, //
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
} ConnectionFsm_t;



ConnectionFsmObject_t NewConnectionFsm(
		ConnectionObject_t connection,
		ConnectionFsm_ActionFn_t A1_ActionFunctionSendToActive,
		ConnectionFsm_ActionFn_t A2_ActionFunctionSendToPassive,
		ConnectionFsm_ActionFn_t A3_ActionDeleteConnection)
{
	ConnectionFsm_t * this = malloc(sizeof(ConnectionFsm_t));
	bzero(this, sizeof(ConnectionFsm_t));
	this->state = CONN_IDENTIFIED; // initial is CONN_IDENTIFIED because the state machine starts as soon as a hello command has been received!
	this->connection = connection;

    Connection_EventAction_t ConnectionFsm[CONN_NOF_STATE][CONN_NOF_EVENT] =
	{
	  /* state: CONN_IDENTIFIED */
	{ /* events                              actions0                        actions1                  actions2     next state */
	  /* CONN_EVENT_RECV_HELLO_CMD*/      {{ NULL,                           NULL,                     NULL },      CONN_IDENTIFIED },
	  /* CONN_EVENT_RECV_CDC_CMD*/        {{ A1_ActionFunctionSendToActive,  NULL,                     NULL },      CONN_ACTIVE },
	  /* CONN_EVENT_TIMEOUT*/             {{ A2_ActionFunctionSendToPassive, A3_ActionDeleteConnection,NULL },      CONN_IDENTIFIED },
	},

	/* state: CONN_ACTIVE */
	{ /* events                              actions0                        actions1                  actions2     next state */
	  /* CONN_EVENT_RECV_HELLO_CMD*/      {{ A2_ActionFunctionSendToPassive, NULL,                     NULL },      CONN_IDENTIFIED },
	  /* CONN_EVENT_RECV_CDC_CMD*/        {{ NULL,                           NULL,                     NULL },      CONN_ACTIVE },
	  /* CONN_EVENT_TIMEOUT*/             {{ A2_ActionFunctionSendToPassive, A3_ActionDeleteConnection,NULL },     	CONN_IDENTIFIED },
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

	static const char* statesStr[] = { "CONN_IDENTIFIED", "CONN_ACTIVE" };
	static const char* eventsStr[] = { "RECV_HELLO_CMD", "RECV_CDC_CMD", "TIMEOUT"};

	Connection_State_t currentState = this->state;

	// get new state
	Connection_State_t newState = this->ConnectionFsm[currentState][event].nextState;

	// execute actions
	UINT32 action = 0;
	while (this->ConnectionFsm[currentState][event].action[action])
	{
		this->ConnectionFsm[currentState][event].action[action](this->connection);
		action++;
	}

	if(this->state != newState)
	{
		TRC_Log_Print(TRC_log, "System State machine: event %s initiates state change from %s to %s", eventsStr[event], statesStr[this->state], statesStr[newState]);
		this->state = newState;
	}
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
