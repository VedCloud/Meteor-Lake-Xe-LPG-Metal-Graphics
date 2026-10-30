/* SPDX-License-Identifier: MIT */
#ifndef DELL27_PL031_H
#define DELL27_PL031_H
#include "vmapple-rtc.h"
typedef struct {uc_engine*uc;RtcClock clock;void*opaque;uint64_t anchor,due;uint32_t count,match,load,mask,status;unsigned invalid;uint64_t reads;} Pl031Rtc;
uc_err pl031_rtc_map(uc_engine*,Pl031Rtc*,RtcClock,void*);
void pl031_rtc_poll(Pl031Rtc*);
#endif
