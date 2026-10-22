#include "latitude_pwm.h"
#include <assert.h>
#include <stdio.h>
int main(){
 LatitudePWMState s={0x80000000,192000,11294,0x80000000,0x80000000};uint32_t d=0;
 assert(latitudePWMValid(s));assert(latitudePWMPlan(s,s,0,&d)&&d==11295);assert(latitudePWMPlan(s,s,65535,&d)&&d==192000);
 uint32_t last=0;for(uint32_t n=0;n<=65535;n++){assert(latitudePWMPlan(s,s,n,&d)&&d>=last&&d<=s.period);last=d;}
 auto n=s;n.period++;assert(!latitudePWMPlan(n,s,40000,&d));n=s;n.control^=1u<<29;assert(!latitudePWMPlan(n,s,40000,&d));
 n=s;n.pipeConf=0;assert(!latitudePWMPlan(n,s,10000,&d));n=s;n.duty=n.period+1;assert(!latitudePWMPlan(n,s,10000,&d));
 n=s;n.control=0;assert(!latitudePWMPlan(n,s,10000,&d));n=s;n.period=0xffffffff;assert(!latitudePWMPlan(n,s,10000,&d));
 assert(!latitudePWMPlan(s,s,65536,&d));assert(latitudePWMLevel(s.period,11294)<65535);
 puts("PASS real PWM plan: 65536 monotonic slider levels; VBT floor; exact maximum; disabled/changed-period/polarity/pipe/invalid-duty rejected. No hardware writes.");
}
