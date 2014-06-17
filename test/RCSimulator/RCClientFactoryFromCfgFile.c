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
#include <errno.h>
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
	char *buffer=NULL;
	UINT32 numbytes=0;
	FILE *infile = fopen(pConfigFileName, "r");
	if(infile)
	{
		fseek(infile, 0L, SEEK_END); //Get the number of bytes
		numbytes = ftell(infile);
		fseek(infile, 0L, SEEK_SET); // reset the file position indicator to the beginning of the file
		buffer = (char*)calloc(numbytes, sizeof(char));
		DBG_ASSERT(buffer);
		fread(buffer, sizeof(char), numbytes, infile);
		fclose(infile);
	}
	else
	{
		DBG_MAKE_ENTRY_MSG(TRUE, strerror(errno));
	}
	*pSize = numbytes;
	return buffer;
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
