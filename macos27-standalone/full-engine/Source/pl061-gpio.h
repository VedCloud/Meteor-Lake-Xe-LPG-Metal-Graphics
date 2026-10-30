/* SPDX-License-Identifier: MIT */
#ifndef DELL27_PL061_H
#define DELL27_PL061_H
#include <unicorn/unicorn.h>
#include <stdint.h>
typedef struct {uc_engine*uc;uint8_t input,output,dir,sense,both,event,mask,edge,alternate;uint64_t accesses;unsigned invalid;} Pl061Gpio;
uc_err pl061_gpio_map(uc_engine*,Pl061Gpio*);
uc_err pl061_gpio_input(Pl061Gpio*,unsigned,unsigned);
#endif
