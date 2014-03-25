/*
 * SystemState.h
 *
 * @see state diagram docs/drawings/System_States.uxf
 *
 *  Created on: 23.03.2014
 *      Author: lorenz
 */

#ifndef SYSTEMSTATEMACHINE_H_
#define SYSTEMSTATEMACHINE_H_

/** system state machine object */
typedef  void* SystemStateMachineObject_t;

/** state machine action function signaure */
typedef void (*SystemStateMachine_ActionFn_t)(void* obj);


/** create a system state machine object */
extern SystemStateMachineObject_t NewSystemStateMachine(void *actionObj, SystemStateMachine_ActionFn_t A1_ActionSetServoPassive);

/** delete a system state object */
extern void DeleteSystemStateMachine(SystemStateMachineObject_t systemObj);

/**
 * Check system state machine for state SYS_IDLE
 *
 * @return TRUE if state is SYS_IDLE
 */
extern BOOL SystemStateMachineIs_SYS_IDLE(SystemStateMachineObject_t systemObj);

/**
 * Check system state machine for state SYS_CONTROLLING
 *
 * @return TRUE if state is SYS_CONTROLLING
 */
extern BOOL SystemStateMachineIs_SYS_CONTROLLING(SystemStateMachineObject_t systemObj);

/**
 * Execute Event Transition to active.
 *
 * This must be called when a passive connection becomes active
 */
extern void SystemStateMachineEventTransitionToActive(SystemStateMachineObject_t systemObj);

/**
 * Execute Event Transition to passive.
 *
 * This must be called when the active connection becomes inactive
 */
extern void SystemStateMachineEventTransitionToPassive(SystemStateMachineObject_t systemObj);

/** unit tests for SystemStateMachine */
extern void SystemStateMachineTest();

#endif /* SYSTEMSTATEMACHINE_H_ */
