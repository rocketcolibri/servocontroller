/*
 * ServoControllerData.h
 *
 *  Created on: 13.06.2014
 *      Author: lorenz
 */

#ifndef SERVOCONTROLLERDATA_H_
#define SERVOCONTROLLERDATA_H_

#define SERVOCONTROLLER_PORT 30001

extern BOOL ServoControllerDataSetIpJson(struct json_object *pJsonObject);
extern BOOL ServoControllerDataSetIpString(const char *pIpAddress);
extern const char * ServoControllerDataGetIpAddress();
extern struct sockaddr_in* ServoControllerDataGetSockAddr();


#endif /* SERVOCONTROLLERDATA_H_ */
