#pragma once
// Hardware-independent decisions only. No input dispatch or framebuffer writes.
// Axes are calibrated physical millimetres/second^2, timestamps monotonic ms.
namespace LatitudeTablet {
using Time = unsigned long long;
enum class Mode { Unknown, Laptop, Tablet };
enum class Pose { Landscape, PortraitLeft, PortraitRight, Inverted };
struct Sample { int x,y,z; Time timestamp; bool valid; };
struct Decision { Mode mode; Pose pose; bool suppressBuiltInIntent; bool orientationValid; };
// Future session UI supplies role/editability only, never the field value.
// Only a keyboard panel owned by our helper may be shown/hidden by this intent.
struct Focus {bool known,editable; Time timestamp;};
inline bool onScreenKeyboardIntent(const Decision &d,Time now,const Focus &focus,bool sessionUnlocked) {
 return sessionUnlocked && d.mode==Mode::Tablet && d.suppressBuiltInIntent && focus.known && focus.editable && focus.timestamp<=now && now-focus.timestamp<=1500;
}
class Policy {
 Mode mode=Mode::Unknown;
 Pose pose=Pose::Landscape, pendingPose=Pose::Landscape;
 Time last=0, modeAt=0, tabletSince=0, poseSince=0, previousSample=0;
 bool started=false, modeSeen=false, tabletPending=false, posePending=false;
 static int abs(int x) {return x<0?-x:x;}
 static bool fresh(Time now,Time stamp,Time age) {return stamp<=now && now-stamp<=age;}
 void unknown() {mode=Mode::Unknown;modeSeen=false;tabletPending=false;posePending=false;}
public:
 void reset() {*this=Policy();}
 // knownMode: -1=no update, 0=laptop, 1=tablet. qualification belongs to
 // the verified transport, NEVER inferred from zero/missing ACPI properties.
 Decision update(Time now,int knownMode,bool qualified,const Sample &s,bool rotationLock=false) {
  if(started && now<last) {reset();return {Mode::Unknown,pose,false,false};}
  started=true;last=now;
  if(!qualified) unknown();
  else if(knownMode==0) {mode=Mode::Laptop;modeSeen=true;modeAt=now;tabletPending=false;posePending=false;pose=Pose::Landscape;}
  else if(knownMode==1) {
   if(!modeSeen || !fresh(now,modeAt,1500)) {mode=Mode::Unknown;tabletPending=false;}
   modeSeen=true;modeAt=now;
   if(mode!=Mode::Tablet) {
    if(!tabletPending) {tabletPending=true;tabletSince=now;}
    else if(now-tabletSince>=500) {mode=Mode::Tablet;tabletPending=false;}
   }
  }
  else if(knownMode!=-1) unknown();
  if(modeSeen && !fresh(now,modeAt,1500)) unknown();
  bool valid=s.valid && fresh(now,s.timestamp,1000) && absSafe(s.x) && absSafe(s.y) && absSafe(s.z);
  if(valid) {long long norm=1LL*s.x*s.x+1LL*s.y*s.y+1LL*s.z*s.z;valid=norm>=49000000 && norm<=144000000;}
  if(mode!=Mode::Tablet || !valid || rotationLock) {posePending=false;return {mode,pose,mode==Mode::Tablet,false};}
  int x=abs(s.x),y=abs(s.y);Pose next=pose;bool resolved=false;
  // Flat/diagonal/ambiguous poses hold the last accepted position.
  if(x>=4000 && 100LL*x>=135LL*y) {next=s.x>0?Pose::PortraitLeft:Pose::PortraitRight;resolved=true;}
  else if(y>=4000 && 100LL*y>=135LL*x) {next=s.y<0?Pose::Landscape:Pose::Inverted;resolved=true;}
  if(!resolved) {posePending=false;return {mode,pose,true,false};}
  if(next==pose) {posePending=false;return {mode,pose,true,true};}
  // Duplicate/stale samples cannot accumulate a stable orientation interval.
  if(!posePending || next!=pendingPose || !fresh(now,previousSample,1000)) {
   pendingPose=next;poseSince=now;posePending=true;previousSample=s.timestamp;
  } else if(s.timestamp>previousSample) {
   previousSample=s.timestamp;
   if(now-poseSince>=500) {pose=next;posePending=false;}
  }
  return {mode,pose,true,true};
 }
private:
 static bool absSafe(int x) {return x>=-20000 && x<=20000;}
};
}
