/*
 * ProcedureList.c
 *
 *  Created on: 15.06.2014
 *      Author: lorenz
 */

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
#include "base/Reactor.h"
#include "base/AD.h"
#include "base/TRC.h"

#include "ProcedureList.h"

typedef struct
{


} ProcedureList_t;

ProcedureListObject_t NewProcedureList(struct json_object *pJsonObject)
{
	ProcedureList_t *this = malloc(sizeof(ProcedureList_t));
	bzero(this, sizeof(ProcedureList_t));
	return this;
}

void DeleteProcedureList(ProcedureListObject_t obj)
{
	ProcedureList_t *this = (ProcedureList_t *)obj;
	DBG_ASSERT(this);
	free(this);
}
