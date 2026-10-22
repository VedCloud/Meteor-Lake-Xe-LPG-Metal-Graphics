#pragma once
#include <stdint.h>
struct LatitudePWMState { uint32_t control, period, duty, pipeConf, planeControl; };
static inline bool latitudePWMValid(const LatitudePWMState &s) {
 return (s.control & 0x80000000u) && s.period>=32 && s.period<=0x00ffffffu && s.duty<=s.period &&
        (s.pipeConf&0x80000000u) && (s.planeControl&0x80000000u);
}
static inline uint32_t latitudePWMMinimum(uint32_t period) { return (uint32_t)(((uint64_t)period*15+254)/255); }
static inline uint32_t latitudePWMDuty(uint32_t period,uint32_t level) {
 const uint32_t minimum=latitudePWMMinimum(period);
 if(level>65535)level=65535;
 return minimum+(uint32_t)(((uint64_t)(period-minimum)*level+32767)/65535);
}
static inline uint32_t latitudePWMLevel(uint32_t period,uint32_t duty) {
 const uint32_t minimum=latitudePWMMinimum(period);
 if(duty<=minimum)return 0;if(duty>=period)return 65535;
 return (uint32_t)(((uint64_t)(duty-minimum)*65535+(period-minimum)/2)/(period-minimum));
}
static inline bool latitudePWMPlan(const LatitudePWMState &now,const LatitudePWMState &initial,uint32_t level,uint32_t *duty) {
 if(!duty || !latitudePWMValid(now) || now.period!=initial.period || now.control!=initial.control || level>65535)return false;
 *duty=latitudePWMDuty(now.period,level);return true;
}
