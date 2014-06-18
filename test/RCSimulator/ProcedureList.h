/*
 * ProcedureList.h
 *
 *  Created on: 13.06.2014
 *      Author: lorenz
 */

#ifndef PROCEDURELIST_H_
#define PROCEDURELIST_H_


typedef void* ProcedureListObject_t;


extern ProcedureListObject_t NewProcedureListJson(struct json_object *pJsonObject);
extern ProcedureListObject_t NewProcedureList();

extern void DeleteProcedureList(ProcedureListObject_t obj);

#endif /* PROCEDURELIST_H_ */
