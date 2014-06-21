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
#include "base/MON.h"
#include "base/DBG.h"
#include "base/Reactor.h"
#include "base/AD.h"
#include "base/TRC.h"

#include "RCClient.h"
#include "ClientList.h"
#include "ProcedureList.h"
#include "ServoControllerData.h"

// batch-file factory
// interactive factory
static const char *pCmdClient = "client";
static const char *pCmdClientAdd = "add";
static const char *pCmdClientShow = "show";
static const char *pCmdClientRemove = "remove";
static const char *pCmdClientDisconnect = "disconnect";
static const char *pCmdClientSendHello = "send-hello";
static const char *pCmdClientSendCdc = "send-cdc";
static BOOL Client_MonCmd(ClientListObject_t clientlist, char * cmdLine)
{
	DBG_ASSERT(clientlist);
	UINT8 argc;
	char** argv;
	MON_SplitArgs(cmdLine, &argc, &argv);

	if ((argc == 4) && (0 == strcmp(argv[1], pCmdClientAdd)))
	{
		ClientListAddClient(clientlist, NewRcClient(argv[2], argv[3]));
	}
	else if ((argc == 3) && (0 == strcmp(argv[1], pCmdClientRemove)))
	{
		RCClientObject_t client = ClientListRemoveClient(clientlist, argv[2]);
		if(client)
			DeleteRCClient(client);
		else
			MON_WriteInfo("client not found");
	}
	else if ((argc == 3) && (0 == strcmp(argv[1], pCmdClientDisconnect)))
	{
		RCClientObject_t  client = ClientListFindClient(clientlist, argv[2]);
		if(client)
			RCClientDisconnect(client);
		else
			MON_WriteInfo("client not found");
	}
	else if ((argc == 2) && (0 == strcmp(argv[1], pCmdClientShow)))
	{
		// TODO
	}
	else if ((argc == 3) && (0 == strcmp(argv[1], pCmdClientSendHello)))
	{
		RCClientObject_t  client = ClientListFindClient(clientlist, argv[2]);
		if(client)
			RCClientSendHello(client);
		else
			MON_WriteInfo("client not found");
	}
	else if ((argc == 3) && (0 == strcmp(argv[1], pCmdClientSendCdc)))
	{
		RCClientObject_t  client = ClientListFindClient(clientlist, argv[2]);
		if(client)
			RCClientSendCdc(client);
		else
			MON_WriteInfo("client not found");
	}
	else
	{
		MON_WriteInfof("Usage:");
		MON_WriteInfof("\n%s\t%s <client-name> <client-ip>", pCmdClient, pCmdClientAdd);
		MON_WriteInfof("\n%s\t%s", pCmdClient, pCmdClientShow);
		MON_WriteInfof("\n%s\t%s <client-name>", pCmdClient, pCmdClientRemove);
		MON_WriteInfof("\n%s\t%s <client-name>", pCmdClient, pCmdClientSendHello);
		MON_WriteInfof("\n%s\t%s <client-name>", pCmdClient, pCmdClientSendCdc);
		MON_WriteInfof("\n%s\t%s <client-name>", pCmdClient, pCmdClientDisconnect);
	}
	return TRUE;
}

static const char *pCmdServoController = "set-server-ip";
static BOOL ServoController_MonCmd(ClientListObject_t clientlist, char * cmdLine)
{
	UINT8 argc;
	char** argv;
	MON_SplitArgs(cmdLine, &argc, &argv);

	if (argc == 2)
	{
		ServoControllerDataSetIpString(argv[1]);
	}
	else
	{
		MON_WriteInfof("Usage:");
		MON_WriteInfof("\n%s\t%s <servocontroller-ip>", pCmdServoController);
	}
	return TRUE;
}

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
		DBG_MAKE_ENTRY_FMT(TRUE, "Error:%s File:%s", strerror(errno), pConfigFileName);
	}
	*pSize = numbytes;
	return buffer;
}

void RCClientFactoryFromCfgFile(ClientListObject_t *pClientList, ProcedureListObject_t *pProcedureList, const char *pConfigFileName)
{
	UINT32 size=0;
	char *pBuffer = ReadCfgFile(pConfigFileName, &size);
	if(pBuffer)
	{
		  struct json_tokener *tok = json_tokener_new();
		  struct json_object * jobj = json_tokener_parse_ex(tok, pBuffer, size);
		  if (jobj)
		  {
			  ServoControllerDataSetIpJson(jobj);
			  *pClientList = NewClientListJson(jobj);
			  *pProcedureList = NewProcedureListJson(jobj);
			  MON_AddMonCmd(pCmdClient, Client_MonCmd, *pClientList );
			  MON_AddMonCmd(pCmdServoController, ServoController_MonCmd, NULL);
		  }
		  else
		  {
		    fprintf(stderr, "JSON error: %s %s", (char *)json_tokener_error_desc(json_tokener_get_error(tok)), (char *)pBuffer);
		    exit(1);
		  }
	}
	free(pBuffer);
}

void RCClientFactoryInteractive(ClientListObject_t *pClientList, ProcedureListObject_t *pProcedureList)
{
	*pClientList = NewClientList(NULL);
	*pProcedureList = NULL;

	MON_AddMonCmd(pCmdClient, Client_MonCmd, *pClientList );
	MON_AddMonCmd(pCmdServoController, ServoController_MonCmd, NULL);
}
