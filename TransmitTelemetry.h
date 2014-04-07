/*
 * TransmitTelemetry.h
 *
 *  Created on: 14.10.2013
 *      Author: lorenz
 */

#ifndef TRANSMITTELEMETRY_H_
#define TRANSMITTELEMETRY_H_

typedef void * TransmitTelemetryObject_t;

TransmitTelemetryObject_t NewTransmitTelemetry(ConnectionContainerObject_t connectionContainerObject);
void DeleteTransmitTelemetry(TransmitTelemetryObject_t transmitTelemetryHandlerObject);


#endif /* TRANSMITTELEMETRY_H_ */
