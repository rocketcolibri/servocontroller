/*
 * ConnectionContainer.h
 *
 *  Created on: 15.10.2013
 *      Author: lorenz
 */

#ifndef CONNECTIONCONTAINER_H_
#define CONNECTIONCONTAINER_H_


extern ConnectionContainerObject_t NewConnectionContainer();
extern void DeleteConnectionContainer(ConnectionContainerObject_t connectionContainerObject);
extern BOOL ConnectionContainerHandover(ConnectionContainerObject_t connectionContainerObject, struct sockaddr_in *pNewSrcAddr);
extern ConnectionObject_t ConnectionContainerFindConnection(ConnectionContainerObject_t connectionContainerObject, struct sockaddr_in *pSrcAddr);
extern void ConnectionContainerAddConnection(ConnectionContainerObject_t connectionContainerObject, ConnectionObject_t connectionObject, struct sockaddr_in *pSrcAddr);
extern void ConnectionContainerRemoveConnection(ConnectionContainerObject_t connectionContainerObject, ConnectionObject_t connectionObject, struct sockaddr_in *pSrcAddr);

#endif /* CONNECTIONCONTAINER_H_ */
