#pragma once
#include <stdint.h>
// Per-device event gate; never changes the shared PS/2 controller or USB input.
// Caller serializes on the same workloop as normal event delivery.
namespace LatitudeTablet {
inline bool suppressionLease(uint64_t now,bool enabled,bool qualified,bool tablet,uint64_t stamp){
 return enabled&&qualified&&tablet&&stamp&&stamp<=now&&now-stamp<=1500000000ULL;
}
class KeyboardGate {
 bool blocked=false,held[256]={},delivered[256]={},quarantined[256]={};
public:
 template<class Emit>void transition(bool value,Emit emit){
  if(value==blocked)return;blocked=value;
  if(blocked)for(unsigned k=0;k<256;k++){quarantined[k]=held[k];if(delivered[k]){delivered[k]=false;emit(k,false);}}
 }
 // A key held during folding must be physically released before it can produce
 // another press after unfolding; stale repeats cannot resurrect modifiers.
 bool accept(unsigned key,bool down){
  if(key>=256)return !blocked;
  if(down){held[key]=true;if(blocked){quarantined[key]=true;return false;}if(quarantined[key])return false;delivered[key]=true;return true;}
  held[key]=false;bool suppress=blocked||quarantined[key];quarantined[key]=false;delivered[key]=false;return !suppress;
 }
 bool isBlocked()const{return blocked;}
};
class PointerGate {
 bool blocked=false;uint32_t ignored=0,delivered=0;
public:
 template<class Release>void transition(bool value,Release release){
  if(value==blocked)return;blocked=value;
  if(blocked){ignored|=delivered;if(delivered)release();delivered=0;}
 }
 bool pointer(uint32_t raw,uint32_t &output){
  if(blocked){ignored=raw;return false;}ignored&=raw;output=raw&~ignored;delivered=output;return true;
 }
 bool scroll()const{return !blocked;}
 bool isBlocked()const{return blocked;}
};
}
