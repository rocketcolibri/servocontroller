/**
 * ConnectionContainer.h
 *
 *  The connection container holds the information about all active connections and it's states
 *
 *  The key to search for connection is the source address of the received UDP messages.
 */

#ifndef CONNECTIONCONTAINER_H_
#define CONNECTIONCONTAINER_H_

/**
 * Creates an new empty connection conntainer
 */
extern ConnectionContainerObject_t NewConnectionContainer();

/**
 * Deletes a connection container object
 */
extern void DeleteConnectionContainer(ConnectionContainerObject_t connectionContainerObject);

/**
 * Perform a handover of the active connection to another one.
 * @param connectionContainerObject, this
 * @param pNewSrcAddr, address of the connection that becomes active
 */
extern BOOL ConnectionContainerHandover(ConnectionContainerObject_t connectionContainerObject, struct sockaddr_in *pNewSrcAddr);

/**
 * Find a connection in the connection container.
 * @param connectionContainerObject, this
 * @param pSrcAddr, connection to search for
 */
extern ConnectionObject_t ConnectionContainerFindConnection(ConnectionContainerObject_t connectionContainerObject, struct sockaddr_in *pSrcAddr);

/**
 * Add a connection to the connection container.
 * @param connectionContainerObject, this
 * @param pSrcAddr, connection to be added
 */
extern void ConnectionContainerAddConnection(ConnectionContainerObject_t connectionContainerObject, ConnectionObject_t connectionObject, struct sockaddr_in *pSrcAddr);

/**
 * Remove a connection from the connection container.
 * @param connectionContainerObject, this
 * @param pSrcAddr, connection to be removed.
 */
extern void ConnectionContainerRemoveConnection(ConnectionContainerObject_t connectionContainerObject, ConnectionObject_t connectionObject);


/**
 * Add a connection to the connection container.
 * @param connectionContainerObject, this
 * @return AVL tree containing all connections
 */
extern AVLTREE ConnectionContainerGetAllConnections(ConnectionContainerObject_t connectionContainerObject);

extern ConnectionContainerObject_t ConnectionContainerGetActiveConnection(ConnectionContainerObject_t connectionContainerObject);

extern BOOL ConnectionContainerIsActiveConnection(
		ConnectionContainerObject_t connectionContainerObject,
		ConnectionObject_t connection);

// setter
extern void ConnectionContainerSetActiveConnection(ConnectionContainerObject_t connectionContainerObject, ConnectionObject_t newActiveConnection);


// getter
extern SystemFsmObject_t ConnectionContainerGetSystemFsm(ConnectionContainerObject_t connectionContainerObject);

#endif /* CONNECTIONCONTAINER_H_ */
