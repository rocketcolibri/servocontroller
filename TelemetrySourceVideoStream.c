/**
 * this is the command to find out if a camera is connected
 * /opt/vc/bin/vcgencmd get_camera
 * supported=0 detected=0
 */

#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "base/GEN.h"
#include "base/AD.h"
#include "base/MON.h"
#include "base/DBG.h"
#include "base/Reactor.h"
#include <json-c/json.h>

#include "CommandLineArguments.h"
#include "ITelemetrySource.h"

typedef void *TelemetrySourceVideoStreamObject_t;
typedef struct
{
	BOOL isAvailable;
} TelemetrySourceVideoStream_t;

static BOOL TelemetrySourceVideoStreamIsAvailable(TelemetrySourceVideoStreamObject_t obj)
{
	DBG_ASSERT(obj);
	TelemetrySourceVideoStream_t *this = (TelemetrySourceVideoStream_t*)obj;
	if(0==strcmp("simulate", CommandLineArguments_getServoDriver(args)))
		this->isAvailable = 0 == system("echo 'supported=1 detected=1' | egrep supported=1.+detected=1");
	else
		this->isAvailable = 0 == system("/opt/vc/bin/vcgencmd get_camera | egrep supported=1.+detected=1");
	return this->isAvailable;
}

static json_object *  TelemetrySourceVideoStreamGetJsonObj(TelemetrySourceVideoStreamObject_t obj)
{
	DBG_ASSERT(obj);
	TelemetrySourceVideoStream_t *this = (TelemetrySourceVideoStream_t*)obj;
	DBG_ASSERT(this->isAvailable);
	json_object * jobj = json_object_new_object();
	json_object_object_add(jobj, "type", json_object_new_string("video"));
	json_object_object_add(jobj, "description", json_object_new_string("ServoController Video Stream"));
	json_object_object_add(jobj, "value", json_object_new_string("rtsp://192.168.200.1:8554/unicast"));
	return jobj;
}

static void DeleteTelemetrySourceVideoStream(TelemetrySourceVideoStreamObject_t obj)
{
	DBG_ASSERT(obj);
	free(obj);
}

ITelemetrySourceObject_t NewTelemetrySourceVideoStream()
{
	ITelemetrySource_t *Ithis = malloc(sizeof(ITelemetrySource_t));
	bzero(Ithis, sizeof(ITelemetrySource_t));
	Ithis->obj = malloc(sizeof(TelemetrySourceVideoStream_t));
	bzero(Ithis->obj, sizeof(TelemetrySourceVideoStream_t));
	Ithis->fnIsAvailable = TelemetrySourceVideoStreamIsAvailable;
	Ithis->fnGetJsonObj = TelemetrySourceVideoStreamGetJsonObj;
	Ithis->fnDelete = DeleteTelemetrySourceVideoStream;
	return Ithis;
}

