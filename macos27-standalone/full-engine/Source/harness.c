/* SPDX-License-Identifier: MIT */
#include "glue.h"
#include <unicorn/unicorn.h>
#include "../Standalone/probe_payload.h"
#include "../Standalone/privileged_payload.h"
void bridge_init(EFI_SYSTEM_TABLE*);
int bridge_selftest(void);
int run_sparse_ram_tests(void);
int run_uart_tests(void);
int run_bdif_tests(void);
int run_qarma_tests(void);
int run_config_tests(void);
int run_rtc_tests(void);
int run_pl031_tests(void);
int run_aes_tests(void);
int run_exception_tests(void);
int run_gic_tests(void);
int run_timer_msr_tests(void);
int run_timer_preemption_tests(void);
int run_msi_tests(void);
int run_sgi_tests(void);
int run_pci_tests(void);
int run_virtio_root_tests(void);
int run_gpio_tests(void);
int run_iosfc_tests(void);
int run_pstate_tests(void);
int run_sprr_tests(void);
int run_cow_tests(void);
int run_pac_tests(EFI_SYSTEM_TABLE*);
int run_pac_load_tests(void);
EFI_STATUS run_vmapple_entry(EFI_HANDLE,EFI_SYSTEM_TABLE*);
EFI_STATUS run_reference_entry(EFI_HANDLE,EFI_SYSTEM_TABLE*);
void bridge_output(const char*);
void bridge_finish_output(void);
void bridge_address(const char*,const void*);
static void check(uc_err err,const char*what){if(err!=UC_ERR_OK){bridge_output("FAIL ");bridge_output(what);bridge_output(": ");bridge_output(uc_strerror(err));bridge_output("\n");abort();}}
static int run_probe(int privileged){
 uc_engine*uc=NULL;uc_err err=uc_open(UC_ARCH_ARM64,UC_MODE_ARM,&uc);check(err,"uc_open");bridge_output("Opened ARM64 engine\n");
 check(uc_mem_map(uc,0,16384,UC_PROT_READ|UC_PROT_EXEC),"map code");
 check(uc_mem_map(uc,0x41000000,4096,UC_PROT_READ|UC_PROT_WRITE),"map data");
 const uint8_t*code=privileged?priv_code:probe_code;size_t length=privileged?sizeof(priv_code):sizeof(probe_code);
 uint8_t fresh[80];check(uc_mem_read(uc,0x41000000,fresh,sizeof(fresh)),"fresh RAM read");for(size_t i=0;i<sizeof(fresh);i++)if(fresh[i]){bridge_output("FAIL fresh guest RAM is not zero\n");abort();}
 check(uc_mem_write(uc,0,code,length),"write code");
 const uint8_t*data=privileged?priv_data:probe_data;size_t size=privileged?sizeof(priv_data):sizeof(probe_data);
 check(uc_mem_write(uc,0x41000000,data,size),"write data");
 if(privileged){uc_arm64_cp_reg scr={.crn=1,.crm=1,.op0=3,.op1=6,.op2=0,.val=0x501};check(uc_reg_write(uc,UC_ARM64_REG_CP_REG,&scr),"set SCR_EL3 nonsecure");uint32_t pstate=0x3c9;check(uc_reg_write(uc,UC_ARM64_REG_PSTATE,&pstate),"set EL2");}
 err=uc_emu_start(uc,0,privileged?0x4c:0xb0,0,10000);if(err){uint64_t pc=0;uc_reg_read(uc,UC_ARM64_REG_PC,&pc);bridge_address("Guest fault PC",(void*)(UINTN)pc);}check(err,"execute ARM");
 uint8_t result[80];check(uc_mem_read(uc,0x41000000,result,sizeof(result)),"read result");
 const uint8_t*expected=privileged?priv_expected:probe_expected;int different=memcmp(result,expected,sizeof(result));
 check(uc_close(uc),"uc_close");return different==0;
}
int run_compositor_policy_tests(void);
int run_service_observer_tests(void);
EFI_STATUS EFIAPI efi_main(EFI_HANDLE image,EFI_SYSTEM_TABLE*t){(void)image;bridge_init(t);
 EFI_STATUS watchdog=t->BootServices->SetWatchdogTimer(0,0,0,NULL);
 if(EFI_ERROR(watchdog)){bridge_output("FAIL disable firmware watchdog\n");return watchdog;}
 bridge_output("Firmware watchdog disabled for long-running ARM64 runtime\n");
 bridge_output("Dell27 Unicorn firmware engine integration test\n");bridge_address("EFI entry",efi_main);if(!bridge_selftest()){bridge_output("FAIL firmware services self-test\n");return EFI_COMPROMISED_DATA;}bridge_output("PASS firmware services self-test\n");
 #ifdef TEST_ASSERT_GUARD
 volatile unsigned guard=0;assert(guard);bridge_output("FAIL assertion did not halt\n");return EFI_COMPROMISED_DATA;
 #endif
 if(!run_probe(0)){bridge_output("FAIL scalar result comparison\n");return EFI_COMPROMISED_DATA;}
 bridge_output("PASS full Unicorn scalar JIT execution: 80 QEMU-matched bytes\n");
 if(!run_probe(1)){bridge_output("FAIL EL2 result comparison\n");return EFI_COMPROMISED_DATA;}
 bridge_output("PASS full Unicorn selected EL2 JIT execution: 80 QEMU-matched bytes\n");
 if(!run_compositor_policy_tests())return EFI_COMPROMISED_DATA;
 if(!run_service_observer_tests())return EFI_COMPROMISED_DATA;
 if(!run_pstate_tests())return EFI_COMPROMISED_DATA;
 if(!run_iosfc_tests())return EFI_COMPROMISED_DATA;
 if(!run_gpio_tests())return EFI_COMPROMISED_DATA;
 if(!run_virtio_root_tests())return EFI_COMPROMISED_DATA;
 if(!run_pci_tests())return EFI_COMPROMISED_DATA;
 if(!run_sgi_tests())return EFI_COMPROMISED_DATA;
 if(!run_msi_tests())return EFI_COMPROMISED_DATA;
 if(!run_gic_tests())return EFI_COMPROMISED_DATA;
 if(!run_timer_msr_tests())return EFI_COMPROMISED_DATA;
 if(!run_timer_preemption_tests())return EFI_COMPROMISED_DATA;
 if(!run_exception_tests())return EFI_COMPROMISED_DATA;
 if(!run_uart_tests())return EFI_COMPROMISED_DATA;
 if(!run_bdif_tests())return EFI_COMPROMISED_DATA;
 if(!run_pl031_tests())return EFI_COMPROMISED_DATA;
 if(!run_rtc_tests())return EFI_COMPROMISED_DATA;
 if(!run_config_tests())return EFI_COMPROMISED_DATA;
 if(!run_aes_tests())return EFI_COMPROMISED_DATA;
 if(!run_sparse_ram_tests())return EFI_COMPROMISED_DATA;
 if(!run_pac_tests(t))return EFI_COMPROMISED_DATA;
 if(!run_pac_load_tests())return EFI_COMPROMISED_DATA;
 if(!run_qarma_tests())return EFI_COMPROMISED_DATA;
 if(!run_sprr_tests())return EFI_COMPROMISED_DATA;
 if(!run_cow_tests())return EFI_COMPROMISED_DATA;
 if(EFI_ERROR(run_reference_entry(image,t)))return EFI_LOAD_ERROR;
#ifdef TEST_VMAPPLE_ENTRY
 if(EFI_ERROR(run_vmapple_entry(image,t)))return EFI_LOAD_ERROR;
#endif
 bridge_output("DONE engine integration tests. No macOS kernel boot claimed.\n");
 bridge_finish_output();
 UINTN index;t->BootServices->WaitForEvent(1,&t->ConIn->WaitForKey,&index);return EFI_SUCCESS;
}
