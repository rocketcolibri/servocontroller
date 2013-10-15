/*
 * ConnectionContainer.h
 *
 *  Created on: 15.10.2013
 *      Author: lorenz
 */

#ifndef CONNECTIONCONTAINER_H_
#define CONNECTIONCONTAINER_H_

typedef struct Connection Connection_t;

typedef struct ConnectionContainer
{
	UINT8 dummy;
	Connection_t *pActiveConnection;
	AVLTREE hAllConnections;
} ConnectionContainer_t;

extern ConnectionContainer_t *NewConnectionContainer();
extern void DeleteConnectionContainer(ConnectionContainer_t *pConnectionContainer);
extern BOOL ConnectionContainerHandover(ConnectionContainer_t *pConnectionContainer, struct sockaddr_in *pNewSrcAddr);
extern Connection_t *ConnectionContainerFindConnection(ConnectionContainer_t *pConnectionContainer, struct sockaddr_in *pSrcAddr);
extern void ConnectionContainerAddConnection(ConnectionContainer_t *pConnectionContainer, Connection_t *pConnection, struct sockaddr_in *pSrcAddr);
extern void ConnectionContainerRemoveConnection(ConnectionContainer_t *pConnectionContainer, Connection_t *pConnection);

#endif /* CONNECTIONCONTAINER_H_ */
