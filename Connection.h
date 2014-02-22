/**
 *  Connection.c
 *
 * Implements the RocketColibri protocol on the receiver side.
 */

#ifndef CONNECTION_H_
#define CONNECTION_H_

typedef void *ConnectionObject_t;
typedef void *ConnectionContainerObject_t;

/**
 * handles the reception of a message
 *
 * @param connectionObject
 * @param pJsonStiring, JSON formated message
 */
extern void HandleJsonMessage( ConnectionObject_t connectionObject, const char *pJsonString);

/**
   * Creates a new connection object
   *
   * @param connectionContainerObject, connection container.
   * @param pSrcAddr source address of the new connection
   * @param socketFd socket
   * @param hTrcSocket to trace debug messages, all connections of the socket uses the same trace output
   */
extern ConnectionObject_t NewConnection(const ConnectionContainerObject_t containerConnection,
		const struct sockaddr_in *pSrcAddr, const int socketFd, UINT8 hTrcSocket);

/**
 * Delete a connection object
 */

// getter
extern struct sockaddr_in* ConnectionGetAddress(ConnectionObject_t connectionObject);
extern int ConnectionGetSocket(ConnectionObject_t connectionObject);
extern char* ConnectionGetUserName(ConnectionObject_t connectionObject);

#endif /* CONNECTION_H_ */

extern void DeleteConnection(ConnectionObject_t *pConn);
