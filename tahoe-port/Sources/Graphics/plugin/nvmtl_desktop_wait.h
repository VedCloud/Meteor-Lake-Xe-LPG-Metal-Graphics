#pragma once
#include <pthread.h>
#include <errno.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
static inline int nvmtl_desktop_mutex_enter(pthread_mutex_t *mutex,long budget_ms)
{
    const uint64_t start=clock_gettime_nsec_np(CLOCK_MONOTONIC_RAW);
    const uint64_t budget=(uint64_t)(budget_ms>0?budget_ms:1)*1000000ull;
    for(;;){
        const int result=pthread_mutex_trylock(mutex);
        if(result!=EBUSY)return result;
        if(clock_gettime_nsec_np(CLOCK_MONOTONIC_RAW)-start>=budget)return ETIMEDOUT;
        usleep(1000);
    }
}
