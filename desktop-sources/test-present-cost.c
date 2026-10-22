#include <assert.h>
#include <stdio.h>
#include "nvmtl_present_cost.h"
int main(void)
{
    nvmtl_present_cost c={0};
    uint64_t a[]={100,110,130,160,200,250};
    assert(!nvmtl_present_cost_add(&c,a,400));
    assert(c.count==1 && c.bytes==400 && c.elapsed[0]==10 && c.elapsed[4]==50 && c.max_total==150);
    uint64_t invalid[]={100,110,109,160,200,250};
    nvmtl_present_cost saved=c;
    assert(!nvmtl_present_cost_add(&c,invalid,123));
    assert(!memcmp(&saved,&c,sizeof c));
    uint64_t b[]={2000000100ull,2000000110ull,2000000130ull,2000000160ull,2000000200ull,2000000250ull};
    assert(nvmtl_present_cost_add(&c,b,600));
    assert(c.count==2 && c.bytes==1000 && c.elapsed[2]==60 && c.max_total==150);
    nvmtl_present_cost_reset(&c);
    assert(c.reports==1 && !c.count && !c.bytes && !c.max_total);
    for(unsigned i=1;i<8;i++){
        assert(!nvmtl_present_cost_add(&c,a,400));
        assert(nvmtl_present_cost_add(&c,b,600));
        nvmtl_present_cost_reset(&c);
    }
    saved=c;
    assert(!nvmtl_present_cost_add(&c,a,400) && !memcmp(&saved,&c,sizeof c));
    puts("PASS present cost: exact phase sums/bytes/max, delayed reporting, invalid clocks ignored, eight-report cap; no device operations");
}
