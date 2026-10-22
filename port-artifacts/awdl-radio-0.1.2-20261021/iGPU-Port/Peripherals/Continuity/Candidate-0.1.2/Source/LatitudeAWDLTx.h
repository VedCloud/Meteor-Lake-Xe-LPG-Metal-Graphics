#pragma once
#include <stdint.h>
// Gate-confined bookkeeping. A firmware success is not proof that a peer
// received a broadcast action frame. Reclaim and reset are distinct events.
struct LatitudeAWDLTxTag { uint64_t cookie=0; bool response=false; };
struct LatitudeAWDLTxLedger {
 uint64_t next=0, submitted=0, responses=0, succeeded=0, failed=0;
 uint64_t reclaimed=0, aborted=0, missingResponse=0, lastCookie=0;
 uint32_t lastStatus=0;
 uint64_t issue(){if(!++next)++next;return next;}
 void response(LatitudeAWDLTxTag& t,uint32_t status,bool success){
  if(!t.cookie||t.response)return;
  t.response=true;++responses;lastCookie=t.cookie;lastStatus=status;
  if(success)++succeeded;else ++failed;
 }
 void finish(LatitudeAWDLTxTag& t,bool reset){
  if(!t.cookie)return;
  if(reset)++aborted;else {++reclaimed;if(!t.response)++missingResponse;}
  t={};
 }
};
