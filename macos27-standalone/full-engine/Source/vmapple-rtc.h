/* SPDX-License-Identifier: MIT */
#ifndef DELL27_RTC_H
#define DELL27_RTC_H
#include <unicorn/unicorn.h>
#include <stdint.h>
typedef uint64_t (*RtcClock)(void*);
typedef struct {uc_engine*uc;RtcClock clock;void*opaque;uint64_t counter,anchor,control,mask,status,reads;unsigned invalid;} VmappleRtc;
uc_err vmapple_rtc_map(uc_engine*,VmappleRtc*,RtcClock,void*);
uc_err vmapple_rtc_notify(VmappleRtc*,unsigned);
#endif
