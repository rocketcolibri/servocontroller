/*
 * ITelemetrySource.h
 *
 *  Created on: 23.05.2014
 *      Author: lorenz
 */

#ifndef ITELEMETRYSOURCE_H_
#define ITELEMETRYSOURCE_H_

typedef void *ITelemetrySourceObject_t;

typedef struct
{
	BOOL (*fnIsAvailable)(void *obj);
	json_object *(* fnGetJsonObj)(void *obj);
	void (* fnDelete)(void *obj);
	void *obj;
} ITelemetrySource_t;

extern BOOL ITelemetrySourceIsAvailable(ITelemetrySourceObject_t obj);

extern json_object * ITelemetrySourceGetJsonObj(ITelemetrySourceObject_t obj);

extern void ITelemetrySourceDelete(ITelemetrySourceObject_t obj);

#endif /* ITELEMETRYSOURCE_H_ */
