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

#include "json-c/json.h"

#include "base/GEN.h"
#include "base/MON.h"
#include "base/DBG.h"
#include "base/TRC.h"
#include "base/Reactor.h"
#include "base/AD.h"

#include "ServoControllerData.h"


typedef struct
{
	const char *pIpAddress;
	struct sockaddr_in si_other;
} ServoController_t;

static ServoController_t *_this = NULL;

static ServoController_t* NewServoControllerData();
static ServoController_t*  ServoControllerDataGetInstance();

static BOOL ServoController_MonCmd(void *pData, char * cmdLine)
{
	ServoController_t *this = (ServoController_t *) ServoControllerDataGetInstance();

	if (this->pIpAddress)
	{
		MON_WriteInfof("\nServoController IP:%s", this->pIpAddress);
	}
	else
	{
		MON_WriteInfo("\nno IP address assinged");
	}
	return TRUE;
}


static ServoController_t* NewServoControllerData()
{
	ServoController_t *_this = malloc(sizeof(ServoController_t));
	bzero(_this, sizeof(ServoController_t));
	MON_AddMonCmd("server", ServoController_MonCmd, _this);
	return _this;
}

static const char *ParseIpAddress(struct json_object *pJsonObject)
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

static void SetServerSocket(ServoController_t *this)
{
	bzero( ( char* ) & this->si_other , sizeof ( this->si_other) );
	this->si_other.sin_family = AF_INET ;
	this->si_other.sin_port = htons (SERVOCONTROLLER_PORT);
	if(inet_aton ( this->pIpAddress , &this->si_other.sin_addr) == 0 )
	{
		DBG_MAKE_ENTRY_MSG(FALSE, "inet_aton() failed" );
	}
}

/**
 * the ServoController is a singleton
 */
static ServoController_t*  ServoControllerDataGetInstance()
{
	if(NULL == _this)
		_this = NewServoControllerData();
	return _this;
}


BOOL ServoControllerDataSetIpJson(struct json_object *pJsonObject)
{
	ServoController_t *this = (ServoController_t *) ServoControllerDataGetInstance();
	if(this->pIpAddress)
		free((char*)this->pIpAddress);
	this->pIpAddress = ParseIpAddress(pJsonObject);
	SetServerSocket(this);
	return TRUE;
}

BOOL ServoControllerDataSetIpString(const char *pIpAddress)
{
	DBG_ASSERT(pIpAddress);
	ServoController_t *this = (ServoController_t *) ServoControllerDataGetInstance();
	if(this->pIpAddress)
		free((char*)this->pIpAddress);
	this->pIpAddress = strdup(pIpAddress);
	SetServerSocket(this);
	return TRUE;
}


const char * ServoControllerDataGetIpAddress()
{
	ServoController_t *this = (ServoController_t *) ServoControllerDataGetInstance();
	return this->pIpAddress;
}

struct sockaddr_in* ServoControllerDataGetSockAddr()
{
	ServoController_t *this = (ServoController_t *) ServoControllerDataGetInstance();
	return &this->si_other;
}

