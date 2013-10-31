/*
 * TransmitTelemetryTimerHandler.h
 *
 *  Created on: 14.10.2013
 *      Author: lorenz
 */

#ifndef TRANSMITTELEMETRYTIMERHANDLER_H_
#define TRANSMITTELEMETRYTIMERHANDLER_H_

typedef void * TransmitTelemetryTimerHandlerObject_t;

TransmitTelemetryTimerHandlerObject_t NewTransmitTelemetryTimerHandler(ConnectionContainerObject_t connectionContainerObject);
void DeleteTransmitTelemetryTimerHandler(TransmitTelemetryTimerHandlerObject_t transmitTelemetryHandlerObject);


#endif /* TRANSMITTELEMETRYTIMERHANDLER_H_ */
