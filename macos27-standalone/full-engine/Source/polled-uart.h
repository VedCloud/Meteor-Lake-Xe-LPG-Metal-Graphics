/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef DELL27_POLLED_UART_H
#define DELL27_POLLED_UART_H
#include <unicorn/unicorn.h>
typedef void (*UartSink)(void*,unsigned char);
typedef struct {uint32_t reg[16];unsigned char rx[16];unsigned head,count;uint64_t transmitted;UartSink sink;void*opaque;unsigned bad_access;} PolledUart;
uc_err polled_uart_map(uc_engine*,PolledUart*,uint64_t,UartSink,void*);
int polled_uart_receive(PolledUart*,unsigned char);
#endif
