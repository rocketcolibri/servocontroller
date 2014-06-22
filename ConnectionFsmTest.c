/*
 * ConnectionFsmTest.c
 *
 * Unit test for ConnectionFsm
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

#include "ConnectionFsm.h"

static ConnectionFsmObject_t *out;
BOOL action[5];

static void ResetActionResult()
{
	bzero(action, sizeof(action));
}
static void A1_ActionFunctionCCAddConnection(ConnectionObject_t obj) { action[0]=TRUE; }
static void A2_ActionFunctionCCSetActiveConnection(ConnectionObject_t obj) { action[1]=TRUE; }
static void A3_ActionFunctionCCSetBackToPassivConnection(ConnectionObject_t obj) { action[2]=TRUE; }
static void A4_ActionFunctionCCRemoveConnection(ConnectionObject_t obj) { action[3]=TRUE; }
static void A5_ActionFunctionActionDeleteConnection(ConnectionObject_t obj) { action[4]=TRUE; }


static void test_setup()
{
	ResetActionResult();
	out = NewConnectionFsm(NULL,
			A1_ActionFunctionCCAddConnection,
			A2_ActionFunctionCCSetActiveConnection,
			A3_ActionFunctionCCSetBackToPassivConnection,
			A4_ActionFunctionCCRemoveConnection,
			A5_ActionFunctionActionDeleteConnection);
}

static void test_teardown()
{
	DeleteConnectionFsm(out);
}

MU_TEST(testConnectionFsmAll)
{
	mu_assert(ConnectionFsmIs_CONN_IDENTIFIED(out), "check initial state: Must be in CONN_IDENTIFIED");

	ConnectionFsmEventRecvCdcCmd(out); // event RecvCdc
	mu_assert(action[0], "A1");
	mu_assert(!action[1], "!A2");
	mu_assert(!action[2], "!A3");
	ResetActionResult();
	mu_assert(ConnectionFsmIs_CONN_ACTIVE(out), "check transition to CONN_ACTIVE");

	ConnectionFsmEventRecvCdcCmd(out); // event RecvCdc
	mu_assert(ConnectionFsmIs_CONN_ACTIVE(out), "remain in state CONN_ACTIVE");
	mu_assert(!action[0], "!A1");
	mu_assert(!action[1], "!A2");
	mu_assert(!action[2], "!A3");
	ResetActionResult();

	ConnectionFsmEventRecvHelloCmd(out); // event RecvHello
	mu_assert(!action[0], "!A1");
	mu_assert(action[1], "A2");
	mu_assert(!action[2], "!A3");
	ResetActionResult();
	mu_assert(ConnectionFsmIs_CONN_IDENTIFIED(out), "check transition back be CONN_IDENTIFIED");

	ConnectionFsmEventRecvHelloCmd(out); // event RecvHello
	mu_assert(!action[0], "!A1");
	mu_assert(!action[1], "!A2");
	mu_assert(!action[2], "!A3");
	ResetActionResult();
	mu_assert(ConnectionFsmIs_CONN_IDENTIFIED(out), "remain in state CONN_IDENTIFIED");

	ConnectionFsmEventTimeout(out); // event Timeout
	mu_assert(!action[0], "!A1");
	mu_assert(action[1], "A2");
	mu_assert(action[2], "A3");
}


MU_TEST_SUITE(testConnectionFsm) {
	MU_SUITE_CONFIGURE(&test_setup, &test_teardown);

	MU_RUN_TEST(testConnectionFsmAll);

}

void ConnectionFsmTest()
{
	MU_RUN_SUITE(testConnectionFsm);
	MU_REPORT();
}
