/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef DELL27_PL011_CONSOLE_H
#define DELL27_PL011_CONSOLE_H
#include "polled-uart.h"
typedef struct {uint32_t reg[20];uint64_t transmitted;unsigned bad_access;UartSink sink;void*opaque;} Pl011Console;
uc_err pl011_console_map(uc_engine*,Pl011Console*,uint64_t,UartSink,void*);
#endif
