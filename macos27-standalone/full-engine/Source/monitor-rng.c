/* SPDX-License-Identifier: MIT. No deterministic fallback for runtime keys. */
#include "glue.h"
#include <Protocol/Rng.h>
#include <unicorn/unicorn.h>
#include "vmapple-config.h"
void bridge_output(const char*);
uc_err configure_vmapple_entropy(uc_engine*u,EFI_SYSTEM_TABLE*t){
 EFI_GUID guid=EFI_RNG_PROTOCOL_GUID;EFI_RNG_PROTOCOL*rng=NULL;
 EFI_STATUS s=t->BootServices->LocateProtocol(&guid,NULL,(VOID**)&rng);
 if(EFI_ERROR(s))return UC_ERR_RESOURCE;
 /* First 48 bytes belong to monitor keys/default IDs. The final 64 bytes
  * independently seed guest configuration; never expose monitor key bytes. */
 unsigned char material[48+64];s=rng->GetRNG(rng,NULL,sizeof(material),material);
 if(EFI_ERROR(s))return UC_ERR_RESOURCE;
 unsigned long long defaults[4];memcpy(defaults,material+16,sizeof(defaults));
 uc_err e=uc_arm64_vmapple_configure(u,material,defaults);
 if(!e)e=vmapple_config_seed(u,material+48,64);
 /* Volatile wipe prevents optimizing the monitor's temporary keys away. */
 volatile unsigned char*p=material;for(unsigned i=0;i<sizeof(material);i++)p[i]=0;
 p=(volatile unsigned char*)defaults;for(unsigned i=0;i<sizeof(defaults);i++)p[i]=0;
 if(!e)bridge_output("VMApple configuration entropy installed from firmware RNG (64 bytes; no seed values logged)\n");
 return e;
}
