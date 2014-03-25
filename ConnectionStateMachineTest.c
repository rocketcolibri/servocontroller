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

#include "ConnectionStateMachine.h"

static ConnectionStateMachineObject_t *out;
BOOL action[3];

static void ResetActionResult()
{
	bzero(action, sizeof(action));
}
static void A1_ActionFunctionSendToActive(ConnectionObject_t obj) { action[0]=TRUE; }
static void A2_ActionFunctionSendToPassive(ConnectionObject_t obj) { action[1]=TRUE; }
static void A3_ActionDeleteConnection(ConnectionObject_t obj) { action[2]=TRUE; }

static void test_setup()
{
	ResetActionResult();
	out = NewConnectionStateMachine(NULL, A1_ActionFunctionSendToActive, A2_ActionFunctionSendToPassive, A3_ActionDeleteConnection);
}

static void test_teardown()
{
	DeleteConnectionStateMachine(out);
}

MU_TEST(testConnectionStateMachineAll)
{
	mu_assert(ConnectionStateMachineIs_CONN_IDENTIFIED(out), "check initial state: Must be in CONN_IDENTIFIED");

	ConnectionStateMachineEventRecvCdcCmd(out); // event RecvCdc
	mu_assert(action[0], "A1");
	mu_assert(!action[1], "!A2");
	mu_assert(!action[2], "!A3");
	ResetActionResult();
	mu_assert(ConnectionStateMachineIs_CONN_ACTIVE(out), "check transition to CONN_ACTIVE");

	ConnectionStateMachineEventRecvCdcCmd(out); // event RecvCdc
	mu_assert(ConnectionStateMachineIs_CONN_ACTIVE(out), "remain in state CONN_ACTIVE");
	mu_assert(!action[0], "!A1");
	mu_assert(!action[1], "!A2");
	mu_assert(!action[2], "!A3");
	ResetActionResult();

	ConnectionStateMachineEventRecvHelloCmd(out); // event RecvHello
	mu_assert(!action[0], "!A1");
	mu_assert(action[1], "A2");
	mu_assert(!action[2], "!A3");
	ResetActionResult();
	mu_assert(ConnectionStateMachineIs_CONN_IDENTIFIED(out), "check transition back be CONN_IDENTIFIED");

	ConnectionStateMachineEventRecvHelloCmd(out); // event RecvHello
	mu_assert(!action[0], "!A1");
	mu_assert(!action[1], "!A2");
	mu_assert(!action[2], "!A3");
	ResetActionResult();
	mu_assert(ConnectionStateMachineIs_CONN_IDENTIFIED(out), "remain in state CONN_IDENTIFIED");

	ConnectionStateMachineEventTimeout(out); // event Timeout
	mu_assert(!action[0], "!A1");
	mu_assert(action[1], "A2");
	mu_assert(action[2], "A3");
}


MU_TEST_SUITE(testConnectionStateMachine) {
	MU_SUITE_CONFIGURE(&test_setup, &test_teardown);

	MU_RUN_TEST(testConnectionStateMachineAll);

}

void ConnectionStateMachineTest()
{
	MU_RUN_SUITE(testConnectionStateMachine);
	MU_REPORT();
}
