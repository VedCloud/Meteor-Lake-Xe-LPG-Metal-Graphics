// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include <stdint.h>
#include <stddef.h>
// Intel pinctrl-meteorlake.c: INTC1083, community5, GPP_D [229..253], gpio_base384.
constexpr unsigned LAT_GPIO=387, LAT_PAD=232, LAT_COMM_BASE=204, LAT_GPP_BASE=229;
constexpr uint32_t LAT_BIT=1u<<(LAT_PAD-LAT_GPP_BASE);
constexpr uint32_t LAT_IS=0x204,LAT_IE=0x214,LAT_HOST=0x144,LAT_LOCK=0x118,LAT_OWN=0xc0;
static inline bool latitudePadOffset(uint32_t padbar,size_t bytes,uint32_t *out){
 uint64_t at=(uint64_t)padbar+(LAT_PAD-LAT_COMM_BASE)*16;
 if(padbar<0x220 || (padbar&15) || at+16>bytes || bytes<0x220)return false;
 *out=(uint32_t)at;return true;
}
static inline bool latitudePin(int pin){return pin==LAT_GPIO;}
// GPIO mode, RX enabled, TX disabled, active-low level GPI. Preserve unrelated bits.
static inline bool latitudePadPlan(uint32_t old,uint32_t owner,uint32_t locked,uint32_t *next){
 if(old==0xffffffff || ((owner>>12)&15)!=0)return false;
 const uint32_t mask=(3u<<25)|(1u<<23)|(15u<<17)|(15u<<10)|(1u<<9)|(1u<<8);
 *next=(old&~mask)|(1u<<23)|(1u<<8);
 return !(locked&LAT_BIT) || (((old^*next)&~2u)==0);
}
