/*
 * RCClientFactoryFromCfgFile.h
 *
 *  Created on: 13.06.2014
 *      Author: lorenz
 */

#ifndef RCCLIENTFACTORY_H_
#define RCCLIENTFACTORY_H_


extern void RCClientFactoryFromCfgFile(ClientListObject_t *pClientList, ProcedureListObject_t *pProcedureList, ServoControllerObject_t *pServoController, const char *pConfigFileName);

extern void RCClientFactoryInteractive(ClientListObject_t *pClientList, ProcedureListObject_t *pProcedureList, ServoControllerObject_t *pServoControllerData);

#endif /* RCCLIENTFACTORY_H_ */
