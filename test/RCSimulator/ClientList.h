/*
 * ClienList.h
 *
 *  Created on: 13.06.2014
 *      Author: lorenz
 */

#ifndef CLIENLIST_H_
#define CLIENLIST_H_

typedef void* ClientListObject_t;


extern ClientListObject_t NewClientList(struct json_object *pJsonObject);

extern void DeleteClientList(ClientListObject_t obj);

extern void ClientListAddClient(ClientListObject_t obj, RCClientObject_t rcclient);

extern void ClientListRemoveClient(ClientListObject_t obj, const char *pClientName);

extern RCClientObject_t  ClientListFindClient(ClientListObject_t obj, const char *pClientName);



#endif /* CLIENLIST_H_ */
