/* SPDX-License-Identifier: MIT */
#include "glue.h"
#include "polled-uart.h"
#include "pl011-console.h"
void bridge_output(const char*);
static void check(int v){if(!v){bridge_output("FAIL polled UART\n");abort();}}
static void sink(void*opaque,unsigned char c){unsigned*p=opaque;*p=(*p<<8)|c;}
int run_uart_tests(void){uc_engine*u=NULL;check(uc_open(UC_ARCH_ARM64,UC_MODE_ARM,&u)==UC_ERR_OK);check(uc_mem_map(u,0x1000,4096,UC_PROT_ALL)==UC_ERR_OK);PolledUart uart;unsigned out=0;uint64_t base=0x235200000ULL;check(polled_uart_map(u,&uart,base,sink,&out)==UC_ERR_OK);
 /* ldr w1,[x0,#0x10]; str w2,[x0,#0x20]; ldr w3,[x0,#0x30]; ldr w4,[x0,#0x24] */
 const uint32_t code[]={0xb9401001,0xb9002002,0xb9403003,0xb9402404};check(uc_mem_write(u,0x1000,code,sizeof(code))==UC_ERR_OK);check(uc_reg_write(u,UC_ARM64_REG_X0,&base)==UC_ERR_OK);uint64_t a='A';check(uc_reg_write(u,UC_ARM64_REG_X2,&a)==UC_ERR_OK);check(polled_uart_receive(&uart,'K'));check(uc_emu_start(u,0x1000,0x1010,0,100)==UC_ERR_OK);
 uint64_t value=0;check(uc_reg_read(u,UC_ARM64_REG_X1,&value)==UC_ERR_OK && value==7);check(out=='A' && uart.transmitted==1);check(uc_reg_read(u,UC_ARM64_REG_X3,&value)==UC_ERR_OK && (value&5)==5);check(uc_reg_read(u,UC_ARM64_REG_X4,&value)==UC_ERR_OK && value=='K');check(uart.count==0 && !uart.bad_access);Pl011Console pl;uint64_t plbase=0x20010000;check(pl011_console_map(u,&pl,plbase,sink,&out)==0);const uint32_t plcode[]={0xb9401801,0xb9000002,0xb9403c03,0xb9404004};check(uc_mem_write(u,0x1000,plcode,sizeof(plcode))==0);check(uc_reg_write(u,UC_ARM64_REG_X0,&plbase)==0);check(uc_emu_start(u,0x1000,0x1010,0,100)==0);check(uc_reg_read(u,UC_ARM64_REG_X1,&value)==0 && value==0x90);check(pl.transmitted==1 && !pl.bad_access);check(uc_reg_read(u,UC_ARM64_REG_X3,&value)==0 && value==0x20);check(uc_reg_read(u,UC_ARM64_REG_X4,&value)==0 && value==0);check(uc_close(u)==UC_ERR_OK);bridge_output("PASS UART: guest MMIO transmit, receive, FIFO/status and pending bits\n");return 1;}
