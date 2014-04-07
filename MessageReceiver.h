/**
 * MessageReceiver.h
 *
 * The socket class recieves Control, Hello and Handover commands from the Rocket Colibri transmitter.
 */

#ifndef MessageReceiver_H_
#define MessageReceiver_H_

typedef void *MessageReceiverObject_t;

/**
 * Creates a new Socket
 * @param connectionContainerObject where the socket is stored
 */
extern MessageReceiverObject_t NewMessageReceiver(ConnectionContainerObject_t connectionContainerObject);

/**
 * Deletes a socket object
 * @param this
 */
extern void DeleteMessageReceiver(MessageReceiverObject_t MessageReceiverObject);

#endif /* MessageReceiver_H_ */
