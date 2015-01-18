/*
 * MessageSinkCdc.h
 *
 *  Created on: 25.10.2013
 *      Author: lorenz
 */

#ifndef MESSAGESINKCDC_H_
#define MESSAGESINKCDC_H_


typedef struct
{
	UINT32 nofChannel;
	UINT32 *channelData;
  UINT32 nofFailsafe;
  UINT32 *failsafeData;
} MessageSinkCdc_t;

/**
 * Create a Cdc Message Sink object
 */
extern MessageSinkCdc_t* NewMessageSinkCdc(json_object * jobj);

/**
 * Getter for the number of channels received in the message
 * @param MessageSindCdc object
 * @return number of channels
 */
extern UINT32 GetNofChannel(MessageSinkCdc_t *pMsgCdc);

/**
 * Getter for the channel vector
 * @param MessageSinkCdc object
 * @return channel vector
 */
extern UINT32* GetChannelVector(MessageSinkCdc_t *pMsgCdc);

/**
 * Getter for the number of failsafe settings received in the message
 * @param MessageSindCdc object
 * @return number of failsafe settings
 */
extern UINT32 GetNofFailsafe(MessageSinkCdc_t *pMsgCdc);

/**
 * Getter for the failsafe vector
 * @param MessageSinkCdc object
 * @return failsafe vector
 */
extern UINT32* GetFailsafeVector(MessageSinkCdc_t *pMsgCdc);

/**
 * Deletes the MessageSindCdc object
 */
extern void DeleteMessageSinkCdc(MessageSinkCdc_t *pMsgCdc );

#endif /* MESSAGESINKCDC_H_ */
