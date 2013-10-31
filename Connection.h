/*
 * Connection.h
 *
 *  Created on: 15.10.2013
 *      Author: lorenz
 */

#ifndef CONNECTION_H_
#define CONNECTION_H_

typedef void *ConnectionObject_t;
typedef void *ConnectionContainerObject_t;

extern void HandleJsonMessage( ConnectionObject_t connectionObject, const char *pJsonString);

extern ConnectionObject_t NewConnection(const ConnectionContainerObject_t containerConnection, const struct sockaddr_in *pSrcAddr, UINT8 hTrcSocket);

extern void DeleteConnection(ConnectionObject_t *pConn);

#endif /* CONNECTION_H_ */
