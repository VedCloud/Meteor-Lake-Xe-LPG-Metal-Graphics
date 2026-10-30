/* SPDX-License-Identifier: MIT — bounded reference replay, not a Darwin board */
#include "glue.h"
#include <unicorn/unicorn.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/SimpleFileSystem.h>
#include "../Standalone/boot_input.h"
#ifdef TEST_BOOT_CONTEXT_PROFILE
#include "BootContextExperiment/vmapple_reference_boot.h"
#elif defined(TEST_HFS_PROFILE)
#include "HFSExperiment/vmapple_reference_boot.h"
#elif defined(TEST_STORAGE_PROFILE)
#include "StorageExperiment/vmapple_reference_boot.h"
#elif defined(TEST_FRAMEBUFFER_PROFILE)
#include "FramebufferExperiment/vmapple_reference_boot.h"
#elif defined(TEST_INSTALLER_PROFILE)
#include "InstallerExperiment/vmapple_reference_boot.h"
#else
#include "vmapple_reference_boot.h"
#endif
#include "sparse-ram.h"
#include "polled-uart.h"
#include "pl011-console.h"
#include "vmapple-gic.h"
#include "vmapple-bdif.h"
#include "vmapple-aes.h"
#include "vmapple-config.h"
#include "vmapple-rtc.h"
#include "pl031-rtc.h"
#include "pci-ecam.h"
#include "virtio-root.h"
#include "pl061-gpio.h"
#include "iosfc.h"
static IosfcDevice iosfc_device;
void gop_framebuffer_poll(void);
void gop_framebuffer_close(void);
uc_err bind_gop_framebuffer(uc_engine*,EFI_HANDLE,EFI_SYSTEM_TABLE*,uint64_t);
static Pl061Gpio gpio_device;
static PciEcam pci_device;
static Pl031Rtc pl031_device;
static VmappleRtc rtc_device;
int bridge_rtc_initialize(void);
uint64_t bridge_rtc_clock(void*);
static VmappleConfigDma config_dma;
static VmappleAes aes_device;
#include "utility-file.h"
static VmappleBdif bdif_device;
static VirtioRoot aux_device;
#ifdef TEST_STORAGE_PROFILE
static VirtioRoot root_device;
#endif
uc_err vmapple_config_map(uc_engine*,uint64_t,unsigned,const char*);
void vmapple_host_cpu_brand(char[49]);
static VmappleGic gic_device;
void bridge_output(const char*);
static Pl011Console console_uart;
static char console_line[512];static unsigned console_used;
static void console_flush(void){if(console_used){console_line[console_used]=0;bridge_output(console_line);console_used=0;}}
static void console_byte(void*opaque,unsigned char c){(void)opaque;if(!c)return;console_line[console_used++]=c;if(c==10||console_used==sizeof(console_line)-1)console_flush();}
static SparseRam guest_ram;
void bridge_output(const char*);void bridge_address(const char*,const void*);
static void require(uc_err err,const char*where){if(err){bridge_output("FAIL reference replay ");bridge_output(where);bridge_output(uc_strerror(err));bridge_output("\n");abort();}}
static int guest_read(uc_engine*u,uint64_t va,void*out,unsigned length){
 unsigned char*dst=out;while(length){unsigned chunk=4096-(va&4095);if(chunk>length)chunk=length;unsigned long long pa=0;
  if(uc_arm64_monitor_translate(u,va,0,&pa)||uc_mem_read(u,pa,dst,chunk))return 0;va+=chunk;dst+=chunk;length-=chunk;}return 1;
}
#ifdef TEST_SOFTWARE_COMPOSITOR
#include "no-metal-compat.h"
#include "service-entry-observer.h"
static D27ServiceObserver service_observer;
#endif

#ifdef FAST_KERNEL_TRACE
#define VM_COUNT_LABEL "scheduled_slots"
#else
#define VM_COUNT_LABEL "visited"
#endif
static int has_nonzero(const void*data,unsigned length){
 const unsigned char*p=data;uint64_t bits=0;unsigned i=0;
 for(;i+8<=length;i+=8){uint64_t word;memcpy(&word,p+i,8);bits|=word;}
 for(;i<length;i++)bits|=p[i];return bits!=0;
}
static void guest_string(uc_engine*u,uint64_t pointer,char*out,unsigned length){
 if((pointer>>56)!=255||!guest_read(u,pointer,out,length)){out[0]=0;return;}
 for(unsigned i=0;i<length;i++){unsigned char c=out[i];if(!c)return;if((c<32&&c!=9&&c!=10&&c!=13)||c>126){out[0]=0;return;}}out[length-1]=0;
}
#ifdef TEST_FRAMEBUFFER_PROFILE
static unsigned fb_probe_seen,fb_aperture_seen;
static void framebuffer_trace(uc_engine*u,uint64_t pc,uint32_t size,void*opaque){(void)u;(void)size;(void)opaque;if(pc==0xfffffe0009386c58ULL&&!fb_probe_seen++){bridge_output("Native IOBootFramebuffer probe entered\n");}if(pc==0xfffffe0009387198ULL&&!fb_aperture_seen++){bridge_output("Native IOBootFramebuffer aperture requested\n");}}
#endif
#ifdef TEST_STORAGE_PROFILE
static void storage_trace(uc_engine*u,uint64_t pc,uint32_t size,void*unused){
 (void)size;(void)unused;uint64_t x0=0,x19=0,x20=0;uc_reg_read(u,UC_ARM64_REG_X0,&x0);uc_reg_read(u,UC_ARM64_REG_X19,&x19);uc_reg_read(u,UC_ARM64_REG_X20,&x20);
 char line[240];snprintf(line,sizeof(line),"Native storage checkpoint pc=0x%llx x0=0x%llx x19=0x%llx x20=0x%llx status=%u bar=0x%llx\n",(unsigned long long)pc,(unsigned long long)x0,(unsigned long long)x19,(unsigned long long)x20,root_device.status,(unsigned long long)root_device.bar);bridge_output(line);
 if(pc==0xfffffe0009386d1cULL){uint64_t sp=0,video[8]={0};uc_reg_read(u,UC_ARM64_REG_SP,&sp);if(guest_read(u,sp,video,sizeof(video))){for(unsigned i=0;i<8;i++){snprintf(line,sizeof(line),"Native console info word%u=0x%llx\n",i,(unsigned long long)video[i]);bridge_output(line);}}}
 if(pc==0xfffffe0008a53718ULL){uint64_t data[8]={0};if(guest_read(u,x19+0xf0,data,sizeof(data))){for(unsigned i=0;i<8;i++){snprintf(line,sizeof(line),"Native VirtIO member +0x%x = 0x%llx\n",0xf0+8*i,(unsigned long long)data[i]);bridge_output(line);}}}
}
#endif
static uint64_t visits,last_pc,bad_address;static unsigned kernel_panic;static uc_mem_type bad_type;
static void trace(uc_engine*uc,uint64_t pc,uint32_t size,void*data){(void)uc;(void)size;(void)data;
#ifndef FAST_KERNEL_TRACE
 visits++;
#endif
 last_pc=pc;
 if(pc==0xfffffe000a8194ecULL || pc==0xfffffe000a8195a8ULL){
  uint64_t pointer=0;uc_reg_read(uc,pc==0xfffffe000a8194ecULL?UC_ARM64_REG_X0:UC_ARM64_REG_X3,&pointer);
  char text[257]={0};guest_string(uc,pointer,text,256);
  bridge_output("VMAPPLE kernel panic format: ");bridge_output(text);bridge_output("\n");
  uint64_t sp=0;uc_reg_read(uc,UC_ARM64_REG_SP,&sp);uint64_t stack_args[4]={0};guest_read(uc,sp,stack_args,sizeof(stack_args));
  for(unsigned i=0;i<4;i++){uint64_t value=stack_args[i];char line[100];snprintf(line,sizeof(line),"panic stack%u=0x%llx ",i,(unsigned long long)value);bridge_output(line);char str[257]={0};guest_string(uc,value,str,256);bridge_output(str);bridge_output("\n");}
  for(unsigned i=1;i<=4;i++){uint64_t value=0;uc_reg_read(uc,UC_ARM64_REG_X0+i,&value);char line[100];snprintf(line,sizeof(line),"panic x%u=0x%llx ",i,(unsigned long long)value);bridge_output(line);char str[257]={0};guest_string(uc,value,str,256);bridge_output(str);bridge_output("\n");}
  uc_arm64_cp_reg saved={.op0=3,.op1=0,.crn=4,.crm=0,.op2=1};uc_reg_read(uc,UC_ARM64_REG_CP_REG,&saved);uint64_t elr=saved.val;
  saved.crn=5;saved.crm=2;saved.op2=0;uc_reg_read(uc,UC_ARM64_REG_CP_REG,&saved);uint32_t fault_instruction=0;guest_read(uc,elr,&fault_instruction,4);
  char detail[180];snprintf(detail,sizeof(detail),"Guest exception at panic: ELR_EL1=0x%llx ESR_EL1=0x%llx instruction=0x%08x\n",(unsigned long long)elr,(unsigned long long)saved.val,fault_instruction);bridge_output(detail);
  kernel_panic=1;uc_emu_stop(uc);
 }
 if(visits%50000000==0){uint64_t x9=0,x13=0,x14=0,x1=0;uc_reg_read(uc,UC_ARM64_REG_X9,&x9);uc_reg_read(uc,UC_ARM64_REG_X13,&x13);uc_reg_read(uc,UC_ARM64_REG_X14,&x14);uc_reg_read(uc,UC_ARM64_REG_X1,&x1);char s[220];snprintf(s,sizeof(s),"VMAPPLE checkpoint: visited=%llu pc=%llx x9=%llx x13=%llx x14=%llx x1=%llx\n",(unsigned long long)visits,(unsigned long long)pc,(unsigned long long)x9,(unsigned long long)x13,(unsigned long long)x14,(unsigned long long)x1);bridge_output(s);}}
#ifdef FAST_KERNEL_TRACE
/* A block hook schedules a block before execution. The summed slots are an
 * upper-bound diagnostic estimate, NOT retired instructions or the default
 * before-instruction visit count. Avoid both per-instruction tracing and
 * Unicorn's internal count hook during this exploratory long boot. */
/* Recovery service initialization exceeded the original exploratory bound.
 * Keep a finite, explicit bound; this does not change the guest clock or RAM. */
#ifdef TEST_BOOT_CONTEXT_PROFILE
#define VMAPPLE_BOOT_SLOT_BUDGET 200000000000ULL
#else
#define VMAPPLE_BOOT_SLOT_BUDGET 10000000000ULL
#endif
static uint64_t next_block_checkpoint=50000000;
static void block_trace(uc_engine*u,uint64_t pc,uint32_t size,void*opaque){
 (void)opaque;last_pc=pc;
 observe_service_entry(u,pc,&service_observer);
#ifdef TEST_SOFTWARE_COMPOSITOR
 if(diagnostic_no_metal_cleanup(u,pc))return;
#endif
 if(!size||(size&3)){bridge_output("FAIL invalid ARM64 translation-block size\n");uc_emu_stop(u);return;}
 visits+=size/4;
 static uint64_t poll_after; if(visits>=poll_after){require(uc_arm64_monitor_poll(u,pc),"pre-TB device event poll");pl031_rtc_poll(&pl031_device);
#ifdef TEST_FRAMEBUFFER_PROFILE
 gop_framebuffer_poll();
#endif
 poll_after=visits+65536;}
 if(visits>=next_block_checkpoint){char text[170];snprintf(text,sizeof(text),"VMAPPLE block checkpoint: scheduled_slots=%llu pc=0x%llx\n",(unsigned long long)visits,(unsigned long long)pc);bridge_output(text);next_block_checkpoint=visits+50000000;}
 if(visits>=VMAPPLE_BOOT_SLOT_BUDGET)uc_emu_stop(u);
}
#endif
static bool invalid_memory(uc_engine*uc,uc_mem_type type,uint64_t addr,int size,int64_t value,void*data){(void)value;(void)data;if(sparse_ram_fault(&guest_ram,type,addr,size))return true;bad_address=addr;bad_type=type;return false;}
static void cp(uc_engine*uc,unsigned op1,unsigned crn,unsigned crm,unsigned op2,uint64_t value){uc_arm64_cp_reg r={.op0=3,.op1=op1,.crn=crn,.crm=crm,.op2=op2,.val=value};require(uc_reg_write(uc,UC_ARM64_REG_CP_REG,&r),"set reset register");}
uc_err configure_vmapple_entropy(uc_engine*,EFI_SYSTEM_TABLE*);
EFI_STATUS open_vmapple_input(EFI_HANDLE,EFI_SYSTEM_TABLE*,EFI_FILE_PROTOCOL**,EFI_FILE_PROTOCOL**);
EFI_STATUS host_memory_budget(EFI_SYSTEM_TABLE*,uint64_t,uint64_t,uint64_t*);
EFI_STATUS run_vmapple_entry(EFI_HANDLE image,EFI_SYSTEM_TABLE*t){
 EFI_FILE_PROTOCOL*root=NULL,*file=NULL;void*bytes=NULL;
 EFI_STATUS s=open_vmapple_input(image,t,&root,&file);
 /* Enforce exact geometry and a firmware-derived commit ceiling before I/O.
  * Hash the bytes actually loaded into bounded guest RAM in one pass. Nothing
  * executes until the complete digest matches, so a second admission read adds
  * no integrity protection and doubles multi-gigabyte recovery-image I/O. */
 const UINTN chunk_bytes=1024*1024;UINT64 size=0;
 if(!EFI_ERROR(s))s=file->SetPosition(file,MAX_UINT64);
 if(!EFI_ERROR(s))s=file->GetPosition(file,&size);
 if(!EFI_ERROR(s)&&size!=VMREF_BYTES)s=EFI_COMPROMISED_DATA;
 if(!EFI_ERROR(s))s=t->BootServices->AllocatePool(EfiLoaderData,chunk_bytes,&bytes);
 D27Sha256 hash;
 if(EFI_ERROR(s)){if(file)file->Close(file);if(root)root->Close(root);if(bytes)t->BootServices->FreePool(bytes);bridge_output("FAIL reference capture admission\n");return s;}
 uc_engine*uc=NULL;require(uc_open(UC_ARCH_ARM64,UC_MODE_ARM,&uc),"open");require(uc_ctl_set_cpu_model(uc,UC_CPU_ARM64_VMAPPLE_PAC),"CPU model");
 uint64_t budget=0;s=host_memory_budget(t,VMREF_RAM_BYTES,VMREF_BYTES,&budget);
 if(EFI_ERROR(s)){file->Close(file);root->Close(root);t->BootServices->FreePool(bytes);uc_close(uc);bridge_output("FAIL insufficient host RAM for initialized image and runtime reserve\n");return s;}
 char budget_line[140];snprintf(budget_line,sizeof(budget_line),"VMAPPLE adaptive host commit ceiling=%llu logical=%llu\n",(unsigned long long)budget,(unsigned long long)VMREF_RAM_BYTES);bridge_output(budget_line);
 require(sparse_ram_init(&guest_ram,uc,VMREF_RAM_BASE,VMREF_RAM_BYTES,budget),"sparse physical RAM configuration");
 s=file->SetPosition(file,0);
 d27_sha_init(&hash);
 for(UINT64 at=0;!EFI_ERROR(s)&&at<VMREF_BYTES;){
  UINTN got=VMREF_BYTES-at;if(got>chunk_bytes)got=chunk_bytes;UINTN wanted=got;
  s=file->Read(file,&got,bytes);if(!EFI_ERROR(s)&&got!=wanted)s=EFI_DEVICE_ERROR;
  if(!EFI_ERROR(s)){d27_sha_update(&hash,bytes,got);if(has_nonzero(bytes,got)){require(sparse_ram_back(&guest_ram,VMREF_BASE+at,got),"initialized physical backing");require(uc_mem_write(uc,VMREF_BASE+at,bytes,got),"stream initialized RAM");}}at+=got;
 }
 if(!EFI_ERROR(s)){uint8_t digest[32];d27_sha_final(&hash,digest);if(memcmp(digest,vmref_sha256,32))s=EFI_SECURITY_VIOLATION;}
 file->Close(file);root->Close(root);t->BootServices->FreePool(bytes);
 if(EFI_ERROR(s)){uc_close(uc);bridge_output("FAIL reference capture admission: initialized RAM load\n");return s;}
 bridge_output("Verified complete initialized memory capture; beginning sparse-RAM VMAPPLE kernel replay\n");
 char host_brand[49];vmapple_host_cpu_brand(host_brand);
 require(vmapple_config_map(uc,VMREF_RAM_BYTES,1,host_brand),"virtual platform configuration RAM");
 require(vmapple_config_dma_map(uc,&config_dma,&guest_ram),"extended configuration DMA doorbell");
 UtilityFile utility; s=utility_file_open(image,t,&utility);
 if(EFI_ERROR(s)){uc_close(uc);bridge_output("FAIL open private utility storage file\n");return s;}
 require(vmapple_bdif_map(uc,&bdif_device,&guest_ram,utility.bytes,utility_file_transfer,&utility),"private utility storage BDIF");
 require(vmapple_gic_map(uc,&gic_device),"GICv3 MMIO and CPU interface");
 require(vmapple_aes_map(uc,&aes_device,&guest_ram),"guest-key AES accelerator");
 require(bridge_rtc_initialize()?UC_ERR_OK:UC_ERR_RESOURCE,"firmware calendar source");
 require(vmapple_rtc_map(uc,&rtc_device,bridge_rtc_clock,NULL),"virtual platform RTC");
 require(pl031_rtc_map(uc,&pl031_device,bridge_rtc_clock,NULL),"PL031 RTC");
 require(pci_ecam_map(uc,&pci_device),"virtual PCI host ECAM");
#ifdef TEST_STORAGE_PROFILE
 require(virtio_root_map(uc,&root_device,&guest_ram,VMROOT_BASE,VMROOT_BYTES),"read-only recovery-context VirtIO disk");pci_device.root=&root_device;
 bridge_output("Native storage root: read-only Apple VirtIO PCI endpoint enabled\n");
 require(virtio_aux_attach(&root_device,&aux_device,utility.bytes,utility_file_transfer,&utility),"private auxiliary VirtIO disk");pci_device.aux=&aux_device;
 bridge_output("Native storage AUX: writable private 32MiB Apple VirtIO role2 endpoint enabled\n");
#endif
 require(pl061_gpio_map(uc,&gpio_device),"virtual button GPIO");
 require(iosfc_map(uc,&iosfc_device,&guest_ram),"guest surface memory transport");
 require(pl011_console_map(uc,&console_uart,0x20010000ULL,console_byte,NULL),"polled console UART");
 require(uc_arm64_monitor_counter(uc,VMREF_CNTFRQ),"captured platform counter frequency");
 require(configure_vmapple_entropy(uc,t),"cryptographic VM entropy");
 require(uc_arm64_monitor_exceptions(uc,1),"guest architectural exception delivery");bridge_output("VMAPPLE exception-safe per-instruction PC synchronization enabled\n");
 cp(uc,6,1,1,0,0x30501);cp(uc,0,1,0,2,0x300000);cp(uc,4,1,1,2,0x33ff);
 /* VMAPPLE runs its kernel at guest EL1, unlike the physical SPTM reset. */
 cp(uc,4,1,1,0,(1ULL<<31)|(3ULL<<40));
 uint32_t pstate=0x400003c5;require(uc_reg_write(uc,UC_ARM64_REG_PSTATE,&pstate),"EL2 reset");uint64_t x0=VMREF_X0;require(uc_reg_write(uc,UC_ARM64_REG_X0,&x0),"actual x0");
 #ifdef TEST_FRAMEBUFFER_PROFILE
 require(bind_gop_framebuffer(uc,image,t,VMREF_X0),"real firmware framebuffer");
#endif
 uc_hook hook1,hook2;
#ifdef TEST_FRAMEBUFFER_PROFILE
 uc_hook fb_probe_hook,fb_aperture_hook;
 #ifdef TEST_STORAGE_PROFILE
 const uint64_t storage_points[]={0xfffffe0008a53c18ULL,0xfffffe0008a5300cULL,0xfffffe0008a5320cULL,0xfffffe0008a534acULL,0xfffffe0008a536e8ULL,0xfffffe0008a53718ULL,0xfffffe0008a7ca7cULL,0xfffffe0008a6f9ecULL,0xfffffe0009b43328ULL,0xfffffe0009386cc0ULL,0xfffffe0009386d1cULL,0xfffffe0009386d54ULL};
 uc_hook storage_hooks[sizeof(storage_points)/sizeof(storage_points[0])];for(unsigned si=0;si<sizeof(storage_points)/sizeof(storage_points[0]);si++)require(uc_hook_add(uc,&storage_hooks[si],UC_HOOK_CODE,storage_trace,NULL,storage_points[si],storage_points[si]),"native storage attachment observation");
#endif
 require(uc_hook_add(uc,&fb_probe_hook,UC_HOOK_CODE,framebuffer_trace,NULL,0xfffffe0009386c58ULL,0xfffffe0009386c58ULL),"native framebuffer probe observation");
 require(uc_hook_add(uc,&fb_aperture_hook,UC_HOOK_CODE,framebuffer_trace,NULL,0xfffffe0009387198ULL,0xfffffe0009387198ULL),"native framebuffer aperture observation");
#endif
#ifdef FAST_KERNEL_TRACE
 uc_hook block_hook;require(uc_hook_add(uc,&block_hook,UC_HOOK_BLOCK,block_trace,NULL,1,0),"block scheduling hook");
 require(uc_hook_add(uc,&hook1,UC_HOOK_CODE,trace,NULL,0xfffffe000a8194ecULL,0xfffffe000a8195a8ULL),"panic trace hook");
 bridge_output("VMAPPLE count metric: scheduled block slots (upper bound), not retired instructions or instruction visits\n");
 char budget_report[100];snprintf(budget_report,sizeof(budget_report),"VMAPPLE diagnostic scheduled-slot bound=%llu\n",(unsigned long long)VMAPPLE_BOOT_SLOT_BUDGET);bridge_output(budget_report);
#else
 require(uc_hook_add(uc,&hook1,UC_HOOK_CODE,trace,NULL,1,0),"trace hook");
#endif
 require(uc_hook_add(uc,&hook2,UC_HOOK_MEM_INVALID,invalid_memory,NULL,1,0),"fault hook");
 
#ifdef FAST_KERNEL_TRACE
 uc_err err=uc_emu_start(uc,VMREF_PC,0,0,0);
#else
 uc_err err=uc_emu_start(uc,VMREF_PC,0,0,1000000000);
#endif
uint64_t pc=0;require(uc_reg_read(uc,UC_ARM64_REG_PC,&pc),"final PC");
 console_flush();
 char report[256];snprintf(report,sizeof(report),"VMAPPLE kernel replay stopped: %s; " VM_COUNT_LABEL "=%llu; pc=0x%llx; last=0x%llx; memory_type=%u; address=0x%llx\n",uc_strerror(err),(unsigned long long)visits,(unsigned long long)pc,(unsigned long long)last_pc,(unsigned)bad_type,(unsigned long long)bad_address);bridge_output(report);
 uint32_t instruction=0;if(!uc_mem_read(uc,pc,&instruction,4)){snprintf(report,sizeof(report),"Stopped instruction: 0x%08x\n",instruction);bridge_output(report);}
 snprintf(report,sizeof(report),"VMAPPLE stop classification: %s\n",kernel_panic?"kernel panic entry":gic_device.bad_access||aes_device.bad_access||rtc_device.invalid||pl031_device.invalid||config_dma.invalid||pci_device.invalid||gpio_device.invalid||iosfc_device.invalid
#ifdef TEST_STORAGE_PROFILE
 ||root_device.invalid
#endif
 ?"unsupported virtual device access":"execution exception or instruction budget");bridge_output(report);
 snprintf(report,sizeof(report),"VMAPPLE physical RAM committed=%llu logical=%llu\n",(unsigned long long)guest_ram.committed,(unsigned long long)guest_ram.size);bridge_output(report);
 snprintf(report,sizeof(report),"VMAPPLE RAM fault detail: physical=0x%llx error=%u\n",(unsigned long long)guest_ram.last_physical,guest_ram.last_error);bridge_output(report);
 snprintf(report,sizeof(report),"VMAPPLE UART bytes=%llu invalid_access=%u\n",(unsigned long long)console_uart.transmitted,console_uart.bad_access);bridge_output(report);
 snprintf(report,sizeof(report),"VMAPPLE GIC accesses=%llu unsupported=%u\n",(unsigned long long)gic_device.accesses,gic_device.bad_access);bridge_output(report);
 uint64_t final_x0=0;uc_reg_read(uc,UC_ARM64_REG_X0,&final_x0);snprintf(report,sizeof(report),"VMAPPLE final x0=0x%llx\n",(unsigned long long)final_x0);bridge_output(report);
 unsigned long long counts[7];require(uc_arm64_vmapple_counters(uc,counts),"PAC counters");
 snprintf(report,sizeof(report),"VMAPPLE successful PAC calls: %llu,%llu,%llu,%llu,%llu,%llu,%llu\n",counts[0],counts[1],counts[2],counts[3],counts[4],counts[5],counts[6]);bridge_output(report);
 snprintf(report,sizeof(report),"VMAPPLE BDIF reads=%llu writes=%llu failed=%llu unsupported=%u\n",(unsigned long long)bdif_device.reads,(unsigned long long)bdif_device.writes,(unsigned long long)bdif_device.failed,bdif_device.bad_access);bridge_output(report);
 snprintf(report,sizeof(report),"VMAPPLE AES operations=%llu rejected=%llu unsupported=%u key_disable=0x%x\n",(unsigned long long)aes_device.operations,(unsigned long long)aes_device.rejected,aes_device.bad_access,aes_device.key_disable);bridge_output(report);
 snprintf(report,sizeof(report),"VMAPPLE RTC reads=%llu unsupported=%u; config unavailable=%llu invalid=%llu\n",(unsigned long long)rtc_device.reads,rtc_device.invalid,(unsigned long long)config_dma.unavailable,(unsigned long long)config_dma.invalid);bridge_output(report);
 snprintf(report,sizeof(report),"VMAPPLE PL031 reads=%llu unsupported=%u\n",(unsigned long long)pl031_device.reads,pl031_device.invalid);bridge_output(report);
 snprintf(report,sizeof(report),"VMAPPLE PCI reads=%llu writes=%llu unsupported=%u; endpoint topology profile-dependent\n",(unsigned long long)pci_device.reads,(unsigned long long)pci_device.writes,pci_device.invalid);bridge_output(report);
 snprintf(report,sizeof(report),"VMAPPLE GPIO accesses=%llu unsupported=%u; host button input unconnected\n",(unsigned long long)gpio_device.accesses,gpio_device.invalid);bridge_output(report);
 snprintf(report,sizeof(report),"VMAPPLE IOSFC maps=%llu unmaps=%llu commands=%llu unsupported=%u; renderer unconnected\n",(unsigned long long)iosfc_device.maps,(unsigned long long)iosfc_device.unmaps,(unsigned long long)iosfc_device.commands,iosfc_device.invalid);bridge_output(report);
 #ifdef TEST_STORAGE_PROFILE
 snprintf(report,sizeof(report),"VMAPPLE VirtIO root reads=%llu failures=%llu notifications=%llu status=0x%x invalid=%u\n",(unsigned long long)root_device.reads,(unsigned long long)root_device.failed,(unsigned long long)root_device.notifications,root_device.status,root_device.invalid);bridge_output(report);
#endif
 iosfc_close(&iosfc_device);
 utility_file_close(&utility);
 require(uc_close(uc),"close");
#ifdef TEST_FRAMEBUFFER_PROFILE
 gop_framebuffer_close();
#endif
 bridge_output("VMAPPLE replay uses the vma2 template; device implementation remains incomplete.\n");return EFI_SUCCESS;
}
