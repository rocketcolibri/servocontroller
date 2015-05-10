/*
 * ConnectionFsmTest.c
 *
 * Unit test for ConnectionFsm
 *
 *  Created on: 23.03.2014
 *      Author: lorenz
 */

#include "base/minunit.h"


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
static void A6_ActionFunctionSetFailsafe(ConnectionObject_t obj) { action[5]=TRUE; }

static void test_setup()
{
	ResetActionResult();
	out = NewConnectionFsm(NULL,
			A1_ActionFunctionCCAddConnection,
			A2_ActionFunctionCCSetActiveConnection,
			A3_ActionFunctionCCSetBackToPassivConnection,
			A4_ActionFunctionCCRemoveConnection,
			A5_ActionFunctionActionDeleteConnection,
			A6_ActionFunctionSetFailsafe);
}

static void test_teardown()
{
	DeleteConnectionFsm(out);
}

MU_TEST(testConnectionFsm_State_Idle)
{
	mu_assert(ConnectionFsmIs_CONN_IDLE(out), "check initial state: Must be in CONN_IDLE");
	ConnectionFsmEventRecvCdcCmd(out); // event RecvCdc
	mu_assert(!action[0], "!A1");
	mu_assert(!action[1], "!A2");
	mu_assert(!action[2], "!A3");
	mu_assert(!action[3], "!A4");
	mu_assert(action[4], "A5");
	mu_assert(!action[5], "!A6");
	ResetActionResult();
	ConnectionFsmEventTimeout(out); // event Timeout
	mu_assert(ConnectionFsmIs_CONN_IDLE(out), "check CONN_IDLE");
	mu_assert(!action[0], "!A1");
	mu_assert(!action[1], "!A2");
	mu_assert(!action[2], "!A3");
	mu_assert(!action[3], "!A4");
	mu_assert(action[4], "A5");
	mu_assert(!action[5], "!A6");
	ResetActionResult();

	ConnectionFsmEventRecvHelloCmd(out); // event RecvHello
	mu_assert(ConnectionFsmIs_CONN_IDENTIFIED(out), "check CONN_IDENTIFIED");
	mu_assert(action[0], "A1");
	mu_assert(!action[1], "!A2");
	mu_assert(!action[2], "!A3");
	mu_assert(!action[3], "!A4");
	mu_assert(!action[4], "!A5");
	mu_assert(!action[5], "!A6");
	ResetActionResult();
}

MU_TEST(testConnectionFsm_State_Identified)
{

	ConnectionFsmEventRecvHelloCmd(out); // event RecvHello
	mu_assert(ConnectionFsmIs_CONN_IDENTIFIED(out), "check CONN_IDENTIFIED");
	ResetActionResult();

	ConnectionFsmEventRecvHelloCmd(out); // event RecvHello
	mu_assert(ConnectionFsmIs_CONN_IDENTIFIED(out), "check transition to CONN_IDENTIFIED");
	mu_assert(!action[0], "!A1");
	mu_assert(!action[1], "!A2");
	mu_assert(!action[2], "!A3");
	mu_assert(!action[3], "!A4");
	mu_assert(!action[4], "!A5");
	mu_assert(!action[5], "!A6");
	ResetActionResult();


	ConnectionFsmEventRecvCdcCmd(out); // event RecvCdc
	mu_assert(ConnectionFsmIs_CONN_ACTIVE(out), "remain in state CONN_ACTIVE");
	mu_assert(!action[0], "!A1");
	mu_assert(action[1], "A2");
	mu_assert(!action[2], "!A3");
	mu_assert(!action[3], "!A4");
	mu_assert(!action[4], "!A5");
	mu_assert(!action[5], "!A6");
	ResetActionResult();
}

MU_TEST(testConnectionFsm_State_Identified_Event_Timeout)
{
	ConnectionFsmEventRecvHelloCmd(out); // event RecvHello
	mu_assert(ConnectionFsmIs_CONN_IDENTIFIED(out), "check CONN_IDENTIFIED");
	ResetActionResult();

	ConnectionFsmEventTimeout(out); // event Timeout
	mu_assert(ConnectionFsmIs_CONN_IDENTIFIED(out), "check transition to CONN_IDENTIFIED");
	mu_assert(!action[0], "!A1");
	mu_assert(!action[1], "!A2");
	mu_assert(!action[2], "!A3");
	mu_assert(action[3], "A4");
	mu_assert(action[4], "A5");
	mu_assert(!action[5], "!A6");
	ResetActionResult();
}


MU_TEST(testConnectionFsm_State_Active_Event_RecvCdc)
{
	ConnectionFsmEventRecvHelloCmd(out); // event RecvHello
	ConnectionFsmEventRecvCdcCmd(out); // event RecvCdc
	mu_assert(ConnectionFsmIs_CONN_ACTIVE(out), "check CONN_ACTIV");
	ResetActionResult();

	ConnectionFsmEventTimeout(out); // event Timeout
	mu_assert(!action[0], "!A1");
	mu_assert(!action[1], "!A2");
	mu_assert(!action[2], "!A3");
	mu_assert(!action[3], "!A4");
	mu_assert(!action[4], "!A5");
	mu_assert(action[5], "A6");
	ResetActionResult();
}

MU_TEST(testConnectionFsm_State_Active_Event_RecvHello)
{
	ConnectionFsmEventRecvHelloCmd(out); // event RecvHello
	ConnectionFsmEventRecvCdcCmd(out); // event RecvCdc
	mu_assert(ConnectionFsmIs_CONN_ACTIVE(out), "check CONN_ACTIV");
	ResetActionResult();

	ConnectionFsmEventRecvHelloCmd(out); // event RecvHello
	mu_assert(ConnectionFsmIs_CONN_IDENTIFIED(out), "check CONN_ACTIV");
	mu_assert(!action[0], "!A1");
	mu_assert(!action[1], "!A2");
	mu_assert(action[2], "A3");
	mu_assert(!action[3], "!A4");
	mu_assert(!action[4], "!A5");
	mu_assert(action[5], "A6");
	ResetActionResult();
}

MU_TEST(testConnectionFsm_State_Active_Event_RecvTimeout)
{
	ConnectionFsmEventRecvHelloCmd(out); // event RecvHello
	ConnectionFsmEventRecvCdcCmd(out); // event RecvCdc
	mu_assert(ConnectionFsmIs_CONN_ACTIVE(out), "check CONN_ACTIV");
	ResetActionResult();
	ConnectionFsmEventTimeout(out); // event Timeout
	mu_assert(!action[0], "!A1");
	mu_assert(!action[1], "!A2");
	mu_assert(!action[2], "!A3");
	mu_assert(!action[3], "!A4");
	mu_assert(!action[4], "!A5");
	mu_assert(action[5], "A6");
	ResetActionResult();
}

MU_TEST(testConnectionFsm_State_Filesafe_Recv_Hello)
{
	ConnectionFsmEventRecvHelloCmd(out); // event RecvHello
	ConnectionFsmEventRecvCdcCmd(out); // event RecvCdc
	ConnectionFsmEventTimeout(out); // event Timeout
	mu_assert(ConnectionFsmIs_CONN_FAILSAFE(out), "check CONN_FAILSAFE");
	ResetActionResult();
	ConnectionFsmEventRecvCdcCmd(out); // event RecvCdc
	mu_assert(!action[0], "!A1");
	mu_assert(!action[1], "!A2");
	mu_assert(!action[2], "!A3");
	mu_assert(!action[3], "!A4");
	mu_assert(!action[4], "!A5");
	mu_assert(action[5], "!A6");
	ResetActionResult();
}

MU_TEST_SUITE(testConnectionFsm) {
	MU_SUITE_CONFIGURE(&test_setup, &test_teardown);

	MU_RUN_TEST(testConnectionFsm_State_Idle);

	MU_RUN_TEST(testConnectionFsm_State_Identified);
	MU_RUN_TEST(testConnectionFsm_State_Identified_Event_Timeout);

	MU_RUN_TEST(testConnectionFsm_State_Active_Event_RecvCdc);
	MU_RUN_TEST(testConnectionFsm_State_Active_Event_RecvHello);
	MU_RUN_TEST(testConnectionFsm_State_Active_Event_RecvTimeout);
	MU_RUN_TEST(testConnectionFsm_State_Filesafe_Recv_Hello);


}

void ConnectionFsmTest()
{
	MU_RUN_SUITE(testConnectionFsm);
	MU_REPORT();
}
