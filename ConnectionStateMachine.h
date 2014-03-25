/*
 * ConnectionStateMachine.h
 *
 * @see state diagram docs/drawings/Connection_States.uxf
 *
 *  Created on: 23.03.2014
 *      Author: lorenz
 */

#ifndef CONNECTIONSTATEMACHINE_H_
#define CONNECTIONSTATEMACHINE_H_

#include "Connection.h"

/** system state machine object */
typedef  void* ConnectionStateMachineObject_t;

/** state machine action function signature */
typedef void (*ConnectionStateMachine_ActionFn_t)(ConnectionStateMachineObject_t obj);


/** create a system state machine object */
extern ConnectionStateMachineObject_t NewConnectionStateMachine(
		ConnectionObject_t connection,
		ConnectionStateMachine_ActionFn_t A1_ActionFunctionSendToActive,
		ConnectionStateMachine_ActionFn_t A2_ActionFunctionSendToPassive,
		ConnectionStateMachine_ActionFn_t A3_ActionDeleteConnection);

/** delete a system state object */
extern void DeleteConnectionStateMachine(ConnectionStateMachineObject_t systemObj);

extern BOOL ConnectionStateMachineIs_CONN_IDENTIFIED(ConnectionStateMachineObject_t obj);

extern BOOL ConnectionStateMachineIs_CONN_ACTIVE(ConnectionStateMachineObject_t obj);

extern void ConnectionStateMachineEventRecvHelloCmd(ConnectionStateMachineObject_t obj);

extern void ConnectionStateMachineEventRecvCdcCmd(ConnectionStateMachineObject_t obj);

extern void ConnectionStateMachineEventTimeout(ConnectionStateMachineObject_t obj);

/** unit tests */
extern void ConnectionStateMachineTest();

#endif /* CONNECTIONSTATEMACHINE_H_ */
