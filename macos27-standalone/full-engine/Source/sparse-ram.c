/* SPDX-License-Identifier: MIT */
#include "sparse-ram.h"
#include <string.h>
uc_err sparse_ram_init(SparseRam*r,uc_engine*u,uint64_t base,uint64_t size,uint64_t limit){
 if(!r||!u||!size||base%RAM_BLOCK||size%RAM_BLOCK||limit%RAM_BLOCK||!limit||base>UINT64_MAX-size||size/RAM_BLOCK>RAM_MAX_BLOCKS||limit>size)return UC_ERR_ARG;
 memset(r,0,sizeof(*r));r->uc=u;r->base=base;r->size=size;r->limit=limit;return UC_ERR_OK;
}
uc_err sparse_ram_back(SparseRam*r,uint64_t physical,size_t bytes){
 if(!r||!bytes||physical<r->base||physical-r->base>=r->size||bytes>r->size-(physical-r->base))return UC_ERR_ARG;
 uint64_t first=(physical-r->base)/RAM_BLOCK,last=(physical-r->base+bytes-1)/RAM_BLOCK,needed=0;
 for(uint64_t i=first;i<=last;i++)if(!(r->mapped[i/8]&(1u<<(i%8))))needed+=RAM_BLOCK;
 if(needed>r->limit-r->committed)return UC_ERR_NOMEM;
 for(uint64_t i=first;i<=last;i++)if(!(r->mapped[i/8]&(1u<<(i%8)))){
  uc_err e=uc_mem_map(r->uc,r->base+i*RAM_BLOCK,RAM_BLOCK,UC_PROT_ALL);if(e)return e;
  r->mapped[i/8]|=1u<<(i%8);r->committed+=RAM_BLOCK;
 }
 return UC_ERR_OK;
}
bool sparse_ram_fault(SparseRam*r,uc_mem_type type,uint64_t virtual_address,int bytes){
 unsigned access;
 if(bytes<=0)return false;
 switch(type){case UC_MEM_READ_UNMAPPED:access=0;break;case UC_MEM_WRITE_UNMAPPED:access=1;break;case UC_MEM_FETCH_UNMAPPED:access=2;break;default:return false;}
 unsigned long long physical=0;
 r->last_error=uc_arm64_monitor_translate(r->uc,virtual_address,access,&physical);r->last_physical=physical;
 if(r->last_error)return false;
 r->last_error=sparse_ram_back(r,physical,(size_t)bytes);return r->last_error==UC_ERR_OK;
}
