//
//  osxtime.h
//  JAUItest
//
//  Created by LiHong on 15/12/10.
//  Copyright © 2015年 LiHong. All rights reserved.
//

#ifndef osxtime_h
#define osxtime_h

#ifdef __APPLE__


#include <sys/time.h>
#include <sys/resource.h>
#include <mach/mach.h>
#include <mach/clock.h>
#include <mach/mach_time.h>
#include <errno.h>
#include <unistd.h>
#include <sched.h>

/*typedef enum {
    CLOCK_REALTIME,
    CLOCK_MONOTONIC,
    CLOCK_PROCESS_CPUTIME_ID,
    CLOCK_THREAD_CPUTIME_ID
} clockid_t;
*/
static mach_timebase_info_data_t __clock_gettime_inf;

#ifdef __cplusplus
extern "C" {
#endif
    
int clock_gettime(clockid_t clk_id, struct timespec *tp);

#ifdef __cplusplus
}
#endif

#endif /* osxtime_h */

#endif
