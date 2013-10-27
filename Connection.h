/*
 * Connection.h
 *
 *  Created on: 15.10.2013
 *      Author: lorenz
 */

#ifndef CONNECTION_H_
#define CONNECTION_H_

typedef enum { CONN_IDLE, CONN_IDENTIFIED, CONN_UP, CONN_DEGRADED_1, CONN_DEGRADED_2, CONN_DEGRADED_3 } connectionState_t;

typedef struct ConnectionContainer ConnectionContainer_t;

typedef struct Connection
{
	struct sockaddr_in srcAddress;
	char *pUserName;
	connectionState_t state;
	UINT32 lastSequence;
	void *hConnectionTimeoutPoll;
	const ConnectionContainer_t *pConnectionContainer;
	UINT8 hTrc; // trace handle
} Connection_t;


extern void HandleJsonMessage( Connection_t *pConn, const char *pJsonString);

extern Connection_t *NewConnection(const ConnectionContainer_t *pContainerConnection, const struct sockaddr_in *pSrcAddr, UINT8 hTrcSocket);

extern void DeleteConnection(Connection_t *pConn);

#endif /* CONNECTION_H_ */
