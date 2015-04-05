/*
 * ServoDriverLinino.c
 *
 *  Created on: 05.04.2015
 *      Author: lorenz
 *
 *
 * Linino driver documenation
 * @see http://wiki.linino.org/doku.php?id=wiki:lininoio_sysfs
 *
 * echo 1 > /sys/class/pwm/pwmchip0/export
 * echo 20000000  > /sys/class/pwm/pwmchip0/D9/period
 * echo 560000  > /sys/class/pwm/pwmchip0/D9/duty_cycle
 * echo 1  > /sys/class/pwm/pwmchip0/D9/enable
 *
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <strings.h>
#include <memory.h>
#include <unistd.h>

#include "base/GEN.h"
#include "base/DBG.h"
#include "base/TRC.h"
#include "base/Reactor.h"
#include "base/AD.h"

#include "ServoDriver.h"


#define  SERVO_PERIOD  20000000
#define  SERVO_MIN_POS  560000
#define  SERVO_MAX_POS  2400000
#define  CHANNEL_MIN_POS  0
#define  CHANNEL_MAX_POS  1000

static const int nofPwmChannels = 6;

static const char *pPwmEnable = "/sys/class/pwm/pwmchip0/export";

static const char *pPwmChannelPeriod[] =
{
		 "/sys/class/pwm/pwmchip0/D3/period",
		 "/sys/class/pwm/pwmchip0/D9/period",
		 "/sys/class/pwm/pwmchip0/D10/period",
		 "/sys/class/pwm/pwmchip0/D11/period",
		 "/sys/class/pwm/pwmchip0/D5/period",
		 "/sys/class/pwm/pwmchip0/D6/period"
};

static const char *pPwmChannelDutyCycle[] =
{
		 "/sys/class/pwm/pwmchip0/D3/duty_cycle",
		 "/sys/class/pwm/pwmchip0/D9/duty_cycle",
		 "/sys/class/pwm/pwmchip0/D10/duty_cycle",
		 "/sys/class/pwm/pwmchip0/D11/duty_cycle",
		 "/sys/class/pwm/pwmchip0/D5/duty_cycle",
		 "/sys/class/pwm/pwmchip0/D6/duty_cycle"
};

static const char *pPwmChannelEnable[] =
{
		 "/sys/class/pwm/pwmchip0/D3/enable",
		 "/sys/class/pwm/pwmchip0/D9/enable",
		 "/sys/class/pwm/pwmchip0/D10/enable",
		 "/sys/class/pwm/pwmchip0/D11/enable",
		 "/sys/class/pwm/pwmchip0/D5/enable",
		 "/sys/class/pwm/pwmchip0/D6/enable"
};

static BOOL isInit = FALSE;

static int EchoStr(char filename[], char value[])
{
    int fd = open(filename, O_WRONLY);
    if (fd == -1)
    {
        printf("\topen: %s\n", strerror(errno));
    }
    int n = strlen(value);
    int n_wrote  = write(fd, value, n);
//    printf("wrote %d/%d of %s to %s\n", n_wrote, strlen(value), value, filename);
    if (n_wrote == -1)
    {
        printf("\twrite: %s\n", strerror(errno));
    }
    close(fd);
    return n_wrote;
}

static int EchoInt(const char filename[], int value)
{
    int fd = open(filename, O_WRONLY);
    if (fd == -1)
    {
        printf("\topen: %s\n", strerror(errno));
    }
    char tmp[512];
    snprintf(tmp, 512,"%d", value);
    int n = strlen(tmp);
    int n_wrote  = write(fd, tmp, n);
//    printf("wrote %d/%d of %s to %s\n", n_wrote, strlen(value), value, filename);
    if (n_wrote == -1)
    {
        printf("\twrite: %s\n", strerror(errno));
    }
    close(fd);
    return n_wrote;
}

/**
 * converts the channel value from the RocketColibri remote control to the value needed by the PWM device
 */
static UINT32 ConvertChannelToServoHw(UINT32 channelPosition)
{
	int pos =  SERVO_MIN_POS + (SERVO_MAX_POS-SERVO_MIN_POS) * channelPosition / (CHANNEL_MAX_POS-CHANNEL_MIN_POS);

	return pos;
}

static UINT32 oldChannels[MAX_SERVOS] = { 0,0,0,0,0,0};
static UINT32 failsafe[MAX_SERVOS] = { 0,0,0,0,0,0};

static void InitServos()
{
	int servo;
	for(servo=0; servo < nofPwmChannels; servo++)
	{
		EchoInt (pPwmEnable, servo );
		EchoInt (pPwmChannelPeriod[servo], 20000000);
		EchoInt (pPwmChannelDutyCycle[servo], SERVO_MIN_POS);
	}
}

static void EnableServo(UINT32 s)
{
	EchoInt( pPwmChannelEnable[s], 1);
}

static void DisableServo(UINT32 s)
{
	EchoInt( pPwmChannelEnable[s], 0);
}

static void SetServo(UINT32 s, UINT32 pos)
{
	if(pos)
	{
		if(0==oldChannels[s])
			EnableServo(s);

		EchoInt (pPwmChannelDutyCycle[s], ConvertChannelToServoHw(pos) );
	}
	else
		DisableServo(s);
}

static void PrintServoStates()
{
	DBG_ASSERT(MAX_SERVOS == 8);
	FILE *f = fopen("/tmp/servostates", "w");
	if(f)
	{
		fprintf(f, "{\"channels\":[%d, %d, %d, %d, %d, %d, %d, %d]}\n",
			oldChannels[0], oldChannels[1], oldChannels[2], oldChannels[3],
			oldChannels[4], oldChannels[5], oldChannels[6], oldChannels[7]);
		fclose(f);
	}
}

void ServoDriverLininoSetServos(UINT32 servoc, UINT32 *servov)
{
	if(!isInit)
	{
		InitServos();
		isInit=TRUE;
	}

	UINT s=0;
	BOOL servoChanged = FALSE;
	static UINT count;
  	UINT update = (count++ % 50);
	for(s=0; s < MAX_SERVOS; s++)
	{
		if(s < servoc )
		{
			// update channel if  position changed or 1s interval if servo is not disabled
			if((oldChannels[s] != servov[s]) || ((update == s) && servov[s]))
			{
				SetServo(s, servov[s]);
				oldChannels[s] = servov[s];
				servoChanged = TRUE;
			}
		}
		else
		{
			if(oldChannels[s] != 0)
			{
				SetServo(s, 0);
				oldChannels[s] = 0;
				servoChanged = TRUE;
			}
		}
	}
	if(servoChanged)
		PrintServoStates();
}

void ServoDriverLininoStoreFailsafePosition(UINT32 servoc, UINT32 *servov)
{
  if(servoc)
    memcpy(failsafe, servov, servoc * sizeof(UINT32));
}

void ServoDriverLininoSetFailsafe()
{
  ServoDriverLininoSetServos(MAX_SERVOS, failsafe);
}
