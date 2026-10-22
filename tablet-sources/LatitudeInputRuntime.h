#pragma once
#include "LatitudeInputSuppression.h"
#include <IOKit/IOService.h>
#include <IOKit/IOTimerEventSource.h>
#include <libkern/c++/OSBoolean.h>
#include <libkern/c++/OSNumber.h>
#include <libkern/c++/OSString.h>
#include <kern/clock.h>
// Separate from generic policy so real kernel callers use retained properties.
static inline bool latitudeSuppressionRequested(){
 auto match=IOService::serviceMatching("LatitudeTabletState");if(!match)return false;
 auto iterator=IOService::getMatchingServices(match);match->release();if(!iterator)return false;
 unsigned count=0;bool desired=false;UInt64 ticks=0,now=0;clock_get_uptime(&ticks);absolutetime_to_nanoseconds(ticks,&now);
 while(auto service=OSDynamicCast(IOService,iterator->getNextObject())){
  count++;auto dict=service->dictionaryWithProperties();if(!dict)continue;
  auto readerVersion=OSDynamicCast(OSString,dict->getObject("LatitudeTabletStateVersion"));
  auto allow=OSDynamicCast(OSBoolean,dict->getObject("LatitudeTabletInputSuppressionEnabled"));
  auto qualified=OSDynamicCast(OSBoolean,dict->getObject("LatitudeTabletModeQualified"));
  auto tablet=OSDynamicCast(OSBoolean,dict->getObject("LatitudeTabletModeCurrent"));
  auto stamp=OSDynamicCast(OSNumber,dict->getObject("LatitudeTabletModeHeartbeatUptimeNs"));
  desired=readerVersion&&readerVersion->isEqualTo("0.2.1")&&LatitudeTablet::suppressionLease(now,allow&&allow->isTrue(),qualified&&qualified->isTrue(),tablet&&tablet->isTrue(),stamp?stamp->unsigned64BitValue():0);
  dict->release();
 }
 iterator->release();return count==1&&desired;
}
