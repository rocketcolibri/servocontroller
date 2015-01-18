/*
 * MessageSinkCdc.c
 *
 *  Created on: 25.10.2013
 *      Author: lorenz
 */
#include <strings.h>
#include <memory.h>
#include <unistd.h>
#include <fcntl.h>
#include <json-c/json.h>

#include "base/GEN.h"
#include "base/DBG.h"
#include "base/Reactor.h"
#include "base/AD.h"

#include "MessageSinkCdc.h"


static void ParseJsonObj(MessageSinkCdc_t *pMsgCdc, json_object * jobj)
{
  json_object *pChannelArray = json_object_object_get(jobj, "channels");
  if(pChannelArray)
  {
    if((pMsgCdc->nofChannel = json_object_array_length(pChannelArray)))
    {
      pMsgCdc->channelData = (UINT32*)malloc(sizeof(UINT32)*pMsgCdc->nofChannel);
      bzero(pMsgCdc->channelData, sizeof(pMsgCdc->channelData));
      int u;
      for (u=0; u<pMsgCdc->nofChannel; u++)
      {
        json_object *pChannel = json_object_array_get_idx(pChannelArray, u);
        pMsgCdc->channelData[u] = json_object_get_int(pChannel);
        json_object_put(pChannel);
      }
    }
    json_object_put(pChannelArray);
  }
  else
    pMsgCdc->nofChannel = 0;

  json_object *pFailsafeArray = json_object_object_get(jobj, "failsafe");
  if(pFailsafeArray)
  {
    if((pMsgCdc->nofFailsafe = json_object_array_length(pFailsafeArray)))
    {
      pMsgCdc->failsafeData = (UINT32*)malloc(sizeof(UINT32)*pMsgCdc->nofFailsafe);
      bzero(pMsgCdc->failsafeData, sizeof(pMsgCdc->failsafeData));
      int u;
      for (u=0; u<pMsgCdc->nofFailsafe; u++)
      {
        json_object *pFailsafe = json_object_array_get_idx(pFailsafeArray, u);
        pMsgCdc->failsafeData[u] = json_object_get_int(pFailsafe);
        json_object_put(pFailsafe);
      }
    }
    json_object_put(pFailsafeArray);
  }
  else
    pMsgCdc->nofFailsafe = 0;
}

MessageSinkCdc_t* NewMessageSinkCdc(json_object * jobj)
{
	MessageSinkCdc_t*pMsgCdc = malloc(sizeof(MessageSinkCdc_t));
	bzero(pMsgCdc, sizeof(MessageSinkCdc_t));
	ParseJsonObj(pMsgCdc, jobj);
	return pMsgCdc;
}

UINT32 GetNofChannel(MessageSinkCdc_t *pMsgCdc)
{
	return pMsgCdc->nofChannel;
}

UINT32* GetChannelVector(MessageSinkCdc_t *pMsgCdc)
{
	return pMsgCdc->channelData;
}

UINT32 GetNofFailsafe(MessageSinkCdc_t *pMsgCdc)
{
  return pMsgCdc->nofFailsafe;
}

UINT32* GetFailsafeVector(MessageSinkCdc_t *pMsgCdc)
{
  return pMsgCdc->failsafeData;
}

void DeleteMessageSinkCdc(MessageSinkCdc_t *pMsgCdc )
{
	if(pMsgCdc->channelData)
		free(pMsgCdc->channelData);
	free(pMsgCdc);
}
