#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/ioctl.h>
#include <string.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <linux/joystick.h>

#include "base/GEN.h"
#include "base/AD.h"
#include "base/MON.h"
#include "base/DBG.h"
#include "base/POLL.h"
#include "base/TRC.h"

#define JOY_DEVICE "/dev/input/js0"

UINT8 hTrc=0;
int senderSocket=0;
struct sockaddr_in addr;

/** joystick stuct */
typedef struct
{
	SINT16 *pAxixv;
	UINT axisc;
} JoystickData_t;


static int GetSendSocket(const char *pIpAddress)
{
	int s;
	addr.sin_family = AF_INET;

	if (0 == inet_aton(pIpAddress, &addr.sin_addr))
	{
		perror("inet_aton");
		exit(1);
	}
	addr.sin_port = htons(30001);
	s = socket(PF_INET, SOCK_DGRAM, 0);
	if (s == -1)
	{
		perror("socket");
		exit(1);
	}
	return s;
}

/**
 * @short converts the unsigned short range -32768..32767 to the servo range (0..1000)
 */
static SINT32 Convert(SINT16 axis)
{
	SINT32 c = (axis + 32768) / 0xFFFF;
	return c;
}

/**
 * 20ms timer handler, sends a CDC message to the client
 */
static void TransmitCdcMessageTimer(int socketfd, void *pData)
{
	static UINT32 sequence;
	JoystickData_t *pJoystick = (JoystickData_t*)pData;
	UINT32 expiredTime = TIMERFD_Read(socketfd);

	int axis =0;
	char msg[1024];

	msg[0]='\000';
	for(axis=0; axis < pJoystick->axisc; axis++)
	{
		sprintf(&msg[strlen(msg)], "%d", Convert(pJoystick->pAxixv[axis]));
		if(axis< pJoystick->axisc-1)
			sprintf(&msg[strlen(msg)], ",");
	}

	char json[1024];
	sprintf(json, "{ \"v\" : 1, \"cmd\":\"cdc\",\"sequence\":%d,\"user\":\"%s\",\"channels\":[ %s]}",
			sequence++,
			"Lorenz",
			msg);


	TRC_INFO(hTrc, "%d\t%s", expiredTime, json);
  sendto(senderSocket, json, strlen(json), 0, (struct sockaddr *)&addr, sizeof(addr));
}

/**
 * @short opens the joystick device, read the properties and returns the file descriptor
 */
static int GetJoysticFd(const char *pJoystickDevice, JoystickData_t *pJoystick)
{
	int fd;
	UINT8 nofAxis = 0;

	if ((fd = open(pJoystickDevice, O_RDONLY)) == -1)
	{
		fprintf(stderr, "Couldn't open joystick\n");
		exit(-1);
	}

	ioctl(fd, JSIOCGAXES, &nofAxis);

	pJoystick->pAxixv = malloc(sizeof(SINT16)*nofAxis);
	bzero(pJoystick->pAxixv, sizeof(pJoystick->pAxixv));
	pJoystick->axisc = nofAxis;

	if (nofAxis < 2) {
		fprintf(stderr, "The joystick needs axis\n");
		exit(-2);
	}

	fcntl(fd, F_SETFL, O_NONBLOCK);
	return fd;
}

/**
 * @short handles the joystick change event
 *
 * and reads the new axis positon from the file descriptor
 */
static void JoystickChangeHandler(int joystickfd, void *pData)
{
	JoystickData_t *pJoystick = (JoystickData_t*)pData;
	struct js_event event;
	read(joystickfd, &event, sizeof(event));
	if (event.type & JS_EVENT_AXIS)
	{
		DBG_ASSERT(event.number < pJoystick->axisc);
		pJoystick->pAxixv[event.number] = event.value;
	}
}

int main()
{
	DBG_Init();
	POLL_Init();
	MON_Init();
	TRC_Init();
	MON_AddMonCmd("poll", POLL_MonCmd, 0 );
	MON_AddMonCmd("trc",TRC_ExecMonCmd, 0);

	hTrc = TRC_AddTraceGroup("joystick");
	senderSocket = GetSendSocket("192.168.200.1");

	JoystickData_t joystick;
  int joystickfd = GetJoysticFd(JOY_DEVICE, &joystick);

	fprintf(stderr, "\nnof axis:%d",joystick.axisc);

	// creates 20ms timer and add to the poll list
	void *hPollJoystickChange = POLL_AddReadFd(joystickfd, JoystickChangeHandler, &joystick, "Joystick change");
	// creates the joystick device and add to the poll list
	void *hPollTransmitCdcMsgTimer = POLL_AddReadFd(TIMERFD_Create(20*1000), TransmitCdcMessageTimer, &joystick, "TransmitCdcTimer");;

	// start poll loop
	POLL_Dispatch();

	POLL_RemoveFdAndClose(hPollTransmitCdcMsgTimer);
	POLL_RemoveFdAndClose(hPollJoystickChange);
	return 0;
}
