#include "LatitudeTabletPolicy.h"
#include <cstdio>
#include <cassert>
using namespace LatitudeTablet;
int main() {
 Decision tablet={Mode::Tablet,Pose::Landscape,true,true},laptop={Mode::Laptop,Pose::Landscape,false,true};
 assert(onScreenKeyboardIntent(tablet,1000,{true,true,999},true));
 assert(!onScreenKeyboardIntent(tablet,1000,{true,false,999},true));
 assert(!onScreenKeyboardIntent(tablet,1000,{false,true,999},true));
 assert(!onScreenKeyboardIntent(tablet,3000,{true,true,999},true));
 assert(!onScreenKeyboardIntent(tablet,1000,{true,true,1001},true));
 assert(!onScreenKeyboardIntent(tablet,1000,{true,true,999},false));
 assert(!onScreenKeyboardIntent(laptop,1000,{true,true,999},true));
 Policy p;unsigned long long now,stamp;int mode,qualified,valid,x,y,z,locked;
 while(scanf("%llu %d %d %d %d %d %d %llu %d",&now,&mode,&qualified,&valid,&x,&y,&z,&stamp,&locked)==9) {
  auto d=p.update(now,mode,qualified,{x,y,z,stamp,valid!=0},locked!=0);
  printf("%d %d %d %d\n",int(d.mode),int(d.pose),d.suppressBuiltInIntent,d.orientationValid);
 }
}
