/*
 * ServoDriverRPi.h
 *
 *  Created on: 27.10.2013
 *      Author: lorenz
 */

#ifndef SERVODRIVERMOCK_H_
#define SERVODRIVERMOCK_H_


extern void ServoDriverMockSetServos(UINT32 servoc, UINT32 *servov);
extern void ServoDriverMockStoreFailsafePosition(UINT32 servoc, UINT32 *servov);
extern void ServoDriverMockSetFailsafe();

#endif /* SERVODRIVERMOCK_H_ */
