#include <assert.h>
#include <pthread.h>
#include <stdint.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
typedef struct { void *buf, *map; size_t size; } nvk_buffer;
static _Atomic unsigned made, gone;
static int fail_create;
static int nvmtl_vk_buffer_create(size_t size,int mode,nvk_buffer *b)
{
    assert(mode==3);
    if(fail_create){memset(b,0,sizeof *b);return -1;}
    b->buf=b->map=malloc(size);assert(b->buf);b->size=size;made++;return 0;
}
static void nvmtl_vk_buffer_destroy(nvk_buffer *b)
{
    if(b->buf){free(b->buf);gone++;}memset(b,0,sizeof *b);
}
#include "nvmtl_readback_stage.h"
static void *worker(void *arg)
{
    unsigned char value=(uintptr_t)arg;
    for(unsigned i=0;i<1000;i++){
        nvk_buffer b={0};int lease;
        assert(!nvmtl_readback_stage_acquire(128,1,&b,&lease));
        memset(b.map,value,128);
        usleep(1); /* Lease must survive both simulated completion and CPU read. */
        for(unsigned k=0;k<128;k++)assert(((unsigned char *)b.map)[k]==value);
        nvmtl_readback_stage_release(&b,lease,1);
    }
    return NULL;
}
int main(void)
{
    nvk_buffer a={0},b={0};int la,lb;
    assert(!nvmtl_readback_stage_acquire(64,1,&a,&la)&&la==0);
    void *first=a.buf;
    nvmtl_readback_stage_release(&a,la,1);
    assert(!nvmtl_readback_stage_acquire(64,1,&b,&lb)&&lb==0&&b.buf==first&&made==1&&!gone);
    nvmtl_readback_stage_release(&b,lb,1);
    assert(!nvmtl_readback_stage_acquire(128,1,&b,&lb)&&lb==0&&made==2&&gone==1);
    nvmtl_readback_stage_release(&b,lb,2);
    assert(!nvmtl_readback_slots[0].buffer.buf&&gone==2);
    fail_create=1;assert(nvmtl_readback_stage_acquire(64,1,&a,&la));fail_create=0;
    assert(!nvmtl_readback_stage_acquire(64,1,&a,&la)&&la==0);
    nvmtl_readback_stage_release(&a,la,0);
    assert(nvmtl_readback_slots[0].quarantined&&gone==2);
    assert(!nvmtl_readback_stage_acquire(64,1,&b,&lb)&&lb==1);
    nvmtl_readback_stage_release(&b,lb,1);
    nvk_buffer held[4]={0};int leases[4];
    for(unsigned i=0;i<4;i++)assert(!nvmtl_readback_stage_acquire(64,1,&held[i],&leases[i]));
    assert(leases[0]==1&&leases[1]==2&&leases[2]==3&&leases[3]==-1);
    for(unsigned i=0;i<4;i++)nvmtl_readback_stage_release(&held[i],leases[i],1);
    assert(!nvmtl_readback_stage_acquire(NVMTL_READBACK_KEEP_MAX+1,1,&a,&la)&&la==-1);
    nvmtl_readback_stage_release(&a,la,1);
    assert(!nvmtl_readback_stage_acquire(64,0,&a,&la)&&la==-1);
    nvmtl_readback_stage_release(&a,la,1);
    pthread_t threads[8];
    for(uintptr_t i=0;i<8;i++)assert(!pthread_create(&threads[i],NULL,worker,(void *)(i+1)));
    for(unsigned i=0;i<8;i++)pthread_join(threads[i],NULL);
    unsigned retained=0;
    for(unsigned i=0;i<4;i++)if(nvmtl_readback_slots[i].buffer.buf)retained++;
    assert(made-gone==retained&&retained<=4);
    /* Fixture teardown models process exit after all simulated GPU activity. */
    for(unsigned i=0;i<4;i++)nvmtl_vk_buffer_destroy(&nvmtl_readback_slots[i].buffer);
    assert(made==gone);
    puts("PASS stage leases ASan/UBSan: safe reuse, growth, invalid-map discard, allocation failure unlock, uncertain-submit quarantine, contention/oversize/fresh fallback; 8000 concurrent lease/copy cycles, bounded retained allocations");
}
