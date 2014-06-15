/*
 * ServoControllerData.h
 *
 *  Created on: 13.06.2014
 *      Author: lorenz
 */

#ifndef SERVOCONTROLLERDATA_H_
#define SERVOCONTROLLERDATA_H_



typedef void *ServoControllerObject_t;


extern ServoControllerObject_t NewServoControllerData(struct json_object *pJsonObject);

extern void DeleteServoControllerData(ServoControllerObject_t obj);


#endif /* SERVOCONTROLLERDATA_H_ */
