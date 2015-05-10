/*
 * ConnectionFsm.h
 *
 * @see state diagram docs/drawings/Connection_States.uxf
 *
 *  Created on: 23.03.2014
 *      Author: lorenz
 */

#ifndef ConnectionFsm_H_
#define ConnectionFsm_H_

#include "Connection.h"

/** system state machine object */
typedef  void* ConnectionFsmObject_t;

/** state machine action function signature */
typedef void (*ConnectionFsm_ActionFn_t)(ConnectionFsmObject_t obj);


/** create a system state machine object */
extern ConnectionFsmObject_t NewConnectionFsm(
		ConnectionObject_t connection,
		ConnectionFsm_ActionFn_t A1_CCAddConnection,
		ConnectionFsm_ActionFn_t A2_CCSetActiveConnection,
		ConnectionFsm_ActionFn_t A3_CCSetBackToPassivConnection,
		ConnectionFsm_ActionFn_t A4_CCRemoveConnection,
		ConnectionFsm_ActionFn_t A5_ActionDeleteConnection,
		ConnectionFsm_ActionFn_t A6_SetServoToFailsafe);

/** delete a system state object */
extern void DeleteConnectionFsm(ConnectionFsmObject_t systemObj);

extern BOOL ConnectionFsmIs_CONN_IDLE(ConnectionFsmObject_t obj);

extern BOOL ConnectionFsmIs_CONN_IDENTIFIED(ConnectionFsmObject_t obj);

extern BOOL ConnectionFsmIs_CONN_ACTIVE(ConnectionFsmObject_t obj);

extern void ConnectionFsmEventRecvHelloCmd(ConnectionFsmObject_t obj);

extern void ConnectionFsmEventRecvCdcCmd(ConnectionFsmObject_t obj);

extern void ConnectionFsmEventTimeout(ConnectionFsmObject_t obj);

extern void ConnectionFsmEventInvalidCmd(ConnectionFsmObject_t obj);

extern ConnectionObject_t ConnectionFsmGetConnection(ConnectionFsmObject_t obj);

/** unit tests */
extern void ConnectionFsmTest();

#endif /* ConnectionFsm_H_ */
