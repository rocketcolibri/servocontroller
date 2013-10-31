/**
 * ControlCommandRxSocket.h
 *
 *  Created on: 14.10.2013
 *      Author: lorenz
 */

#ifndef CONTROLCOMMANDRXSOCKET_H_
#define CONTROLCOMMANDRXSOCKET_H_

typedef void *ControlCommandRxSocketObject_t;

extern ControlCommandRxSocketObject_t NewControlCommandRxSocket(ConnectionContainerObject_t connectionContainerObject);
extern void DeleteControlCommandRxSocket(ControlCommandRxSocketObject_t controlCommandRxSocketObject);

#endif /* CONTROLCOMMANDRXSOCKET_H_ */
