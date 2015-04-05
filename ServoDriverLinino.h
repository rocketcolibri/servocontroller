/*
 * ServoDriverLinino.h
 *
 *  Created on: 04.04.2014
 *      Author: lorenz
 */

#ifndef SERVODRIVERLININO_H_
#define SERVODRIVERLININO_H_

extern void ServoDriverLininoSetServos(UINT32 servoc, UINT32 *servov);
extern void ServoDriverLininoStoreFailsafePosition(UINT32 servoc, UINT32 *servov);
extern void ServoDriverLininoSetFailsafe();

#endif /* SERVODRIVERLININO_H_ */
