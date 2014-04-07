/*
 * SystemState.h
 *
 * @see state diagram docs/drawings/System_States.uxf
 *
 *  Created on: 23.03.2014
 *      Author: lorenz
 */

#ifndef SystemFsm_H_
#define SystemFsm_H_

/** system state machine object */
typedef  void* SystemFsmObject_t;

/** state machine action function signaure */
typedef void (*SystemFsm_ActionFn_t)(void* obj);


/** create a system state machine object */
extern SystemFsmObject_t NewSystemFsm(void *actionObj, SystemFsm_ActionFn_t A1_ActionSetServoPassive);

/** delete a system state object */
extern void DeleteSystemFsm(SystemFsmObject_t systemObj);

/**
 * Check system state machine for state SYS_IDLE
 *
 * @return TRUE if state is SYS_IDLE
 */
extern BOOL SystemFsmIs_SYS_IDLE(SystemFsmObject_t systemObj);

/**
 * Check system state machine for state SYS_CONTROLLING
 *
 * @return TRUE if state is SYS_CONTROLLING
 */
extern BOOL SystemFsmIs_SYS_CONTROLLING(SystemFsmObject_t systemObj);

/**
 * Execute Event Transition to active.
 *
 * This must be called when a passive connection becomes active
 */
extern void SystemFsmEventTransitionToActive(SystemFsmObject_t systemObj);

/**
 * Execute Event Transition to passive.
 *
 * This must be called when the active connection becomes inactive
 */
extern void SystemFsmEventTransitionToPassive(SystemFsmObject_t systemObj);

/** unit tests for SystemFsm */
extern void SystemFsmTest();

#endif /* SystemFsm_H_ */
