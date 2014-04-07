/*
 * SystemState.c
 *
 *  Created on: 23.03.2014
 *      Author: lorenz
 */

#include <strings.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "base/GEN.h"
#include "base/TRC.h"
#include "base/AD.h"
#include "base/DBG.h"
#include "SystemFsm.h"

#define SYS_NOF_ACTION 2

/**
 * @short Rocket Colibri protocol states, see state diagram
 */
typedef enum
{
  SYS_IDLE, // not active
  SYS_CONTROLLING, //
  SYS_NOF_STATE
} System_State_t;

/**
 * @short List of all events
 */
typedef enum
{
  SYS_EVENT_CONNECTION_TRANS_ACTIV, // start state machine
  SYS_EVENT_CONNECTION_TRANS_PASSIV, //
  SYS_NOF_EVENT
} System_Event_t;

/** connection event action */
typedef struct
{
  SystemFsm_ActionFn_t action[SYS_NOF_ACTION + 1]; // +1 for NULL termination
  System_State_t nextState;
} System_EventAction_t;

typedef struct
{
	void *pActionObj;
	System_EventAction_t SystemFsm[SYS_NOF_STATE][SYS_NOF_EVENT];
	System_State_t state;
} System_t;

SystemFsmObject_t NewSystemFsm(
		void *actionObj,
		SystemFsm_ActionFn_t A1_ActionSetServoPassive)
{
	System_t * this = malloc(sizeof(System_t));
	bzero(this, sizeof(System_t));
	this->state = SYS_IDLE; // initial state of the state machine

	this->pActionObj = actionObj;

	System_EventAction_t SystemFsm[SYS_NOF_STATE][SYS_NOF_EVENT] =
	{
	  /* state: SYS_IDLE */
	{ /* events                              actions0                  actions1       actions2          next state */
	  /* SYS_EVENT_CONNECTION_TRANS_ACTIV*/  {{ NULL,                  NULL,          NULL },           SYS_CONTROLLING },
	  /* SYS_EVENT_CONNECTION_TRANS_PASSIV*/ {{ NULL,                  NULL,          NULL },      		SYS_IDLE },
	},

	/* state: SYS_CONTROLLING */
	{ /* events                              actions0                    actions1       actions2          next state */
	  /* SYS_EVENT_CONNECTION_TRANS_ACTIV*/  {{ NULL,                    NULL,          NULL },           SYS_CONTROLLING },
	  /* SYS_EVENT_CONNECTION_TRANS_PASSIV*/ {{ A1_ActionSetServoPassive,NULL,          NULL },           SYS_IDLE },
	}
	};
	memcpy(&this->SystemFsm , &SystemFsm, sizeof(SystemFsm));
	return (SystemFsmObject_t) this;
}

void DeleteSystemFsm(SystemFsmObject_t obj)
{
	DBG_ASSERT(obj);
	free(obj);
}

/**
 * Execute state machine event
 * @param pArpingInst
 * @param event
 */
static void ExecuteEvent(SystemFsmObject_t obj,
    System_Event_t event)
{
	DBG_ASSERT(event < SYS_NOF_EVENT);
	DBG_ASSERT(obj);
	System_t *this = (System_t *)obj;

	static const char* statesStr[] = { "SYS_IDLE", "SYS_CONTRLLING" };
	static const char* eventsStr[] = { "TRANSITION_ACTIVE", "TRANSITION_PASSIV"};

	System_State_t currentState = this->state;

	// get new state
	System_State_t newState = this->SystemFsm[currentState][event].nextState;


	// execute actions
	UINT32 action = 0;
	while (this->SystemFsm[currentState][event].action[action])
	{
		this->SystemFsm[currentState][event].action[action](this);
		action++;
	}

	if(this->state != newState)
	{
		TRC_Log_Print(TRC_log, "System State machine: event %s initiates state change from %s to %s", eventsStr[event], statesStr[this->state], statesStr[newState]);
		this->state = newState;
	}
}

BOOL SystemFsmIs_SYS_IDLE(SystemFsmObject_t obj)
{
	DBG_ASSERT(obj);
	System_t *this = (System_t *)obj;
	return SYS_IDLE == this->state;
}

BOOL SystemFsmIs_SYS_CONTROLLING(SystemFsmObject_t obj)
{
	DBG_ASSERT(obj);
	System_t *this = (System_t *)obj;
	return SYS_CONTROLLING == this->state;
}

void SystemFsmEventTransitionToActive(SystemFsmObject_t obj)
{
	DBG_ASSERT(obj);
	ExecuteEvent(obj, SYS_EVENT_CONNECTION_TRANS_ACTIV);
}

void SystemFsmEventTransitionToPassive(SystemFsmObject_t obj)
{
	DBG_ASSERT(obj);
	ExecuteEvent(obj, SYS_EVENT_CONNECTION_TRANS_PASSIV);
}
