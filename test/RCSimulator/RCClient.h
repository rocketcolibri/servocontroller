/*
 * RCClient.h
 *
 *  Created on: 12.06.2014
 *      Author: lorenz
 */

#ifndef RCCLIENT_H_
#define RCCLIENT_H_


typedef void *RCClientObject_t;

extern RCClientObject_t NewRcClient(const char *pName, const char *pIpAddress);

extern void DeleteRCClient(RCClientObject_t obj);

extern void RCClientSendHello(RCClientObject_t obj);

extern void RCClientSendCdc(RCClientObject_t obj);

extern void RCClientSendExpectedActive(RCClientObject_t obj, const char *pActiveClientName);

extern void RCClientSendExpectedPassive(RCClientObject_t obj, const char *pPassiveClientName[]);

#endif /* RCCLIENT_H_ */
