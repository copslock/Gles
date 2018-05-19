#pragma once
#ifdef _MSC_VER
#ifdef __cplusplus
extern "C" {
#endif

#define CLOCK_MONOTONIC                 1

	int clock_gettime(int X, struct timeval *tv);
#ifdef __cplusplus
}
#endif

#endif
