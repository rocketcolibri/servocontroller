/*
 * GEN.c
 *
 *  Created on: 13.10.2013
 *      Author: lorenz
 */

#include <time.h>
#include <unistd.h>
#include <sys/syscall.h>
#include "GEN.h"
 /**
 * @return  UINT32 (Real Time Clock value) [milliseconds]
 */
UINT32 GEN_GetRTC(void)
{
  struct timespec clock;
        syscall(__NR_clock_gettime, CLOCK_MONOTONIC, &clock);
        return(clock.tv_sec*1000 + clock.tv_nsec/1000000);
}

