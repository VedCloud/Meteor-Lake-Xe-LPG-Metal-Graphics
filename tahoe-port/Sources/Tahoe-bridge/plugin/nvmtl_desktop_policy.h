#pragma once
#include <stdint.h>
#include <string.h>
static inline int nvmtl_desktop_stall_policy(uint16_t vendor, uint16_t device,
                                            const char *process, const char *existing)
{
    return vendor == 0x8086 && device == 0x7d45 && process &&
           strcmp(process, "WindowServer") == 0 && existing == NULL;
}

static inline int nvmtl_windowserver_serial_policy(uint16_t vendor, uint16_t device, const char *process)
{
    return vendor == 0x8086 && device == 0x7d45 && process && !strcmp(process,"WindowServer");
}

/* Owned readback staging, independent of the app TLS/suballocation/park paths,
   only for the physical Meteor Lake WindowServer candidate. */
static inline int nvmtl_windowserver_fresh_readback(uint16_t vendor,uint16_t device,const char *process)
{
    return nvmtl_windowserver_serial_policy(vendor,device,process);
}

/* Set before the Intel ICD is loaded: its debug parser is process-once. */
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
static inline int nvmtl_windowserver_disable_ccs(uint16_t vendor,uint16_t device,const char *process)
{
    if(!nvmtl_windowserver_serial_policy(vendor,device,process))return 0;
    const char *previous=getenv("INTEL_DEBUG");
    char flags[4096];
    int n=snprintf(flags,sizeof flags,"%s%snoccs",previous?previous:"",previous&&*previous?",":"");
    if(n<0||(size_t)n>=sizeof flags)return -E2BIG;
    if(setenv("INTEL_DEBUG",flags,1))return -errno;
    return 1;
}
