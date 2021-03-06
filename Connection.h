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
		const struct sockaddr *pSrcAddr, int salen, const int socketFd, UINT8 hTrcSocket);

/**
 * Delete a connection object
 */

// getter
extern struct sockaddr* ConnectionGetAddress(ConnectionObject_t connectionObject);
extern int ConnectionGetAddressLen(ConnectionObject_t connectionObject);
extern const char* ConnectionGetAddressName(ConnectionObject_t connectionObject);
extern int ConnectionGetSocket(ConnectionObject_t connectionObject);
extern char* ConnectionGetUserName(ConnectionObject_t connectionObject);
extern void DeleteConnection(ConnectionObject_t *pConn);

#endif /* CONNECTION_H_ */
