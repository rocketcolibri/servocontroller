/**
 * ControlCommandRxSocket.h
 *
 * The socket class recieves Control, Hello and Handover commands from the Rocket Colibri transmitter.
 */

#ifndef CONTROLCOMMANDRXSOCKET_H_
#define CONTROLCOMMANDRXSOCKET_H_

typedef void *ControlCommandRxSocketObject_t;

/**
 * Creates a new Socket
 * @param connectionContainerObject where the socket is stored
 */
extern ControlCommandRxSocketObject_t NewControlCommandRxSocket(ConnectionContainerObject_t connectionContainerObject);

/**
 * Deletes a socket object
 * @param this
 */
extern void DeleteControlCommandRxSocket(ControlCommandRxSocketObject_t controlCommandRxSocketObject);

#endif /* CONTROLCOMMANDRXSOCKET_H_ */
