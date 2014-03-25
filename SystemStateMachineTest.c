/*
 * ConnectionStateMachineTest.c
 *
 * Unit test for ConnectionStateMachine
 *
 *  Created on: 23.03.2014
 *      Author: lorenz
 */

#include <minunit.h>


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

#include "SystemStateMachine.h"

static SystemStateMachineObject_t *out;
BOOL action[1];

static void ResetActionResult()
{
	bzero(action, sizeof(action));
}
static void A1_SetServoToPassiv(void* obj) { action[0]=TRUE; }

static void test_setup()
{
	ResetActionResult();
	out = NewSystemStateMachine(NULL, A1_SetServoToPassiv);
}

static void test_teardown()
{
	DeleteSystemStateMachine(out);
}

MU_TEST(testSystemStateMachineAll)
{
	mu_assert(SystemStateMachineIs_SYS_IDLE(out), "check initial state: Must be in SYS_IDLE");

	SystemStateMachineEventTransitionToActive(out); // event TransitionToActive
	mu_assert(!action[0], "!A1");
	ResetActionResult();

	SystemStateMachineEventTransitionToPassive(out);
	mu_assert(action[0], "A1");
	ResetActionResult();
	mu_assert(SystemStateMachineIs_SYS_IDLE(out), "check initial state: Must be in SYS_IDLE");

	SystemStateMachineEventTransitionToActive(out);
	mu_assert(!action[0], "!A1");
	mu_assert(SystemStateMachineIs_SYS_CONTROLLING(out), "check transition to SYS_CONTROLLING");

	SystemStateMachineEventTransitionToActive(out);
	mu_assert(!action[0], "!A1");
	mu_assert(SystemStateMachineIs_SYS_CONTROLLING(out), "check transition to SYS_CONTROLLING");

	SystemStateMachineEventTransitionToPassive(out);
	mu_assert(action[0], "A1");
	mu_assert(SystemStateMachineIs_SYS_IDLE(out), "check transition back to SYS_IDLE");
}


MU_TEST_SUITE(testSystemStateMachine) {
	MU_SUITE_CONFIGURE(&test_setup, &test_teardown);

	MU_RUN_TEST(testSystemStateMachineAll);

}

void SystemStateMachineTest()
{
	MU_RUN_SUITE(testSystemStateMachine);
	MU_REPORT();
}
