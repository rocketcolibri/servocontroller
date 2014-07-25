/*
 * ServoDriverRPi.h
 *
 *  Created on: 27.10.2013
 *      Author: lorenz
 */

#ifndef SERVODRIVERRPI_H_
#define SERVODRIVERRPI_H_


extern void ServoDriverRPiSetServos(UINT32 servoc, UINT32 *servov);
extern void ServoDriverRPiStoreFailsafePosition(UINT32 servoc, UINT32 *servov);
extern void ServoDriverRPiSetFailsafe();

#endif /* SERVODRIVERRPI_H_ */
