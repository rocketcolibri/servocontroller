/*
 * RCClientFactoryFromCfgFile.c
 *
 *  Created on: 13.06.2014
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
#include "base/Reactor.h"
#include "base/AD.h"
#include "base/TRC.h"

#include "RCClient.h"
#include "ClientList.h"
#include "ProcedureList.h"
#include "ServoControllerData.h"

static char *ReadCfgFile(const char *pConfigFileName, UINT32 *pSize)
{
	FILE *pCfgFile = fopen(pConfigFileName, "r");

	// TODO
	char *pBuffer = malloc(100000);
	 size_t s = fread(pBuffer,  1, 100000, pCfgFile);
	 *pSize = s;
	fclose(pCfgFile);
	return pBuffer;
}

void RCClientFactoryFromCfgFile(const char *pConfigFileName, ClientListObject_t *pClientList, ProcedureListObject_t *pProcedureList, ServoControllerObject_t *pServoControllerData)
{
	UINT32 size=0;
	char *pBuffer = ReadCfgFile(pConfigFileName, &size);
	if(pBuffer)
	{
		  struct json_tokener *tok = json_tokener_new();
		  struct json_object * jobj = json_tokener_parse_ex(tok, pBuffer, size);
		  if (jobj)
		  {
			  *pClientList = NewClientList(jobj);
			  *pServoControllerData = NewServoControllerData(jobj);
			  *pProcedureList = NewProcedureList(jobj);
		  }
		  else
		  {
		    fprintf(stderr, "JSON error: %s %s", (char *)json_tokener_error_desc(json_tokener_get_error(tok)), (char *)pBuffer);
		    exit(1);
		  }
	}
	free(pBuffer);
}
