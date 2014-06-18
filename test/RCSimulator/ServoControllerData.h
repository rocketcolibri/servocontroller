/*
 * ServoControllerData.h
 *
 *  Created on: 13.06.2014
 *      Author: lorenz
 */

#ifndef SERVOCONTROLLERDATA_H_
#define SERVOCONTROLLERDATA_H_



typedef void *ServoControllerObject_t;

extern ServoControllerObject_t NewServoControllerDataString(const char *pIpAddress);
extern ServoControllerObject_t NewServoControllerDataJson(struct json_object *pJsonObject);
extern ServoControllerObject_t NewServoControllerData();

extern void DeleteServoControllerData(ServoControllerObject_t obj);


#endif /* SERVOCONTROLLERDATA_H_ */
