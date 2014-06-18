/*
 * ServoControllerData.c
 *
 *  Created on: 15.06.2014
 *      Author: lorenz
 */

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <memory.h>
#include <unistd.h>
#include <fcntl.h>
#include <json-c/json.h>

#include "base/GEN.h"
#include "base/DBG.h"
#include "base/TRC.h"
#include "base/Reactor.h"
#include "base/AD.h"

#include "ServoControllerData.h"

typedef struct
{
	const char *pIpAddress;
} ServoController_t;

static const char *GetIpAddress(struct json_object *pJsonObject)
{
	char *pTmpIpAddress = NULL;
	json_object *pJsonServoController = json_object_object_get(pJsonObject, "servocontroller");
	if (pJsonServoController)
	{
		pTmpIpAddress = strdup(json_object_get_string(pJsonServoController));
		json_object_put(pJsonServoController);
	}
	return pTmpIpAddress;
}


ServoControllerObject_t NewServoControllerData()
{
	ServoController_t *this = malloc(sizeof(ServoController_t));
	bzero(this, sizeof(ServoController_t));
	return this;
}
ServoControllerObject_t NewServoControllerDataJson(struct json_object *pJsonObject)
{
	DBG_ASSERT(pJsonObject);
	ServoController_t *this = (ServoController_t *)NewServoControllerData()
	this->pIpAddress = GetIpAddress(pJsonObject);
	return this;
}

void DeleteServoControllerData(ServoControllerObject_t obj)
{
	ServoController_t *this =(ServoController_t *)obj;
	DBG_ASSERT(this);
	free(this);
}
