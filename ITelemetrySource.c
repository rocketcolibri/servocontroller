/*
 * TelemetrySource.c
 *
 *  Created on: 23.05.2014
 *      Author: lorenz
 */

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <memory.h>
#include <unistd.h>
#include <fcntl.h>
#include <json-c/json.h>

#include "base/GEN.h"
#include "ITelemetrySource.h"

BOOL ITelemetrySourceIsAvailable(ITelemetrySourceObject_t obj)
{
	ITelemetrySource_t *this = (ITelemetrySource_t*)obj;
	return this->fnIsAvailable(this->obj);
}

json_object * ITelemetrySourceGetJsonObj(ITelemetrySourceObject_t obj)
{
	ITelemetrySource_t *this = (ITelemetrySource_t*)obj;
	return this->fnGetJsonObj(this->obj);
}

void ITelemetrySourceDelete(ITelemetrySourceObject_t obj)
{
	ITelemetrySource_t *this = (ITelemetrySource_t*)obj;
	return this->fnDelete(this->obj);
	free(this);
}
