/* SPDX-License-Identifier: GPL-2.0-or-later
 * One-vCPU GICv3 subset: no ITS/LPIs/security extension; 288 interrupt IDs.
 * Unsupported accesses fail explicitly. Pending/active/enable/group/priority
 * state is shared by the MMIO and system-register interfaces.
 */
static unsigned mon_gic_best(CPUARMState *e,unsigned group)
{
    if(!e->monitor_gic.configured || (e->monitor_gic.waker&2) ||
       !e->monitor_gic.igrpen[group] || !(e->monitor_gic.dist_ctlr&(group?6:1)))return 1023;
    unsigned best=1023,priority=e->monitor_gic.pmr,running=256;
    for(unsigned i=0;i<288;i++)if((e->monitor_gic.active[i/32]&~e->monitor_gic.dropped[i/32])&(1u<<(i%32)))if(e->monitor_gic.priority[i]<running)running=e->monitor_gic.priority[i];
    for(unsigned i=0;i<288;i++){
        uint32_t bit=1u<<(i%32);unsigned bank=i/32;
        if(((e->monitor_gic.pending[bank]|e->monitor_gic.level[bank])&e->monitor_gic.enable[bank]&bit)&&
           !(e->monitor_gic.active[bank]&bit)&&!!(e->monitor_gic.group[bank]&bit)==group&&
           e->monitor_gic.priority[i]<priority&&e->monitor_gic.priority[i]<running){best=i;priority=e->monitor_gic.priority[i];}
    }
    return best;
}
static void mon_gic_update(CPUARMState *e)
{
    CPUState*cpu=CPU(env_archcpu(e));
    unsigned prior_interrupts = cpu->interrupt_request;
    bool active=false;for(unsigned i=0;i<9;i++)active|=e->monitor_gic.active[i]!=0;
    e->monitor_gic.waker=(e->monitor_gic.waker&2)|((e->monitor_gic.waker&2)&&!active?4:0);
    if(mon_gic_best(e,0)!=1023){if(!(cpu->interrupt_request&CPU_INTERRUPT_FIQ))cpu_interrupt(cpu,CPU_INTERRUPT_FIQ);}else if(cpu->interrupt_request&CPU_INTERRUPT_FIQ)cpu_reset_interrupt(cpu,CPU_INTERRUPT_FIQ);
    if(mon_gic_best(e,1)!=1023){if(!(cpu->interrupt_request&CPU_INTERRUPT_HARD))cpu_interrupt(cpu,CPU_INTERRUPT_HARD);}else if(cpu->interrupt_request&CPU_INTERRUPT_HARD)cpu_reset_interrupt(cpu,CPU_INTERRUPT_HARD);
    /* Unicorn's stripped tcg_handle_interrupt only sets interrupt_request.
     * Kick the next TB entry back to cpu_handle_interrupt, without setting
     * cpu_exit/uc_emu_stop or manufacturing an architectural exception. */
    if ((cpu->interrupt_request & ~prior_interrupts) &
        (CPU_INTERRUPT_HARD | CPU_INTERRUPT_FIQ)) {
        cpu->icount_decr_ptr->u16.high = -1;
    }
}
void arm_monitor_gic_level(CPUARMState *e,unsigned id,bool value)
{
    uint32_t bit=1u<<(id%32);if(!!(e->monitor_gic.level[id/32]&bit)==value)return;if(value)e->monitor_gic.level[id/32]|=bit;else e->monitor_gic.level[id/32]&=~bit;mon_gic_update(e);
}
void arm_monitor_gic_init(CPUARMState *e)
{
    memset(&e->monitor_gic,0,sizeof(e->monitor_gic));e->monitor_gic.configured=true;e->monitor_gic.waker=6;e->monitor_gic.pmr=255;e->monitor_gic.dist_ctlr=64;memset(e->monitor_gic.priority,0xa0,288);mon_gic_update(e);
}
void arm_monitor_devices_poll(CPUARMState *e)
{
    if(!e->monitor_gic.configured)return;
    ARMGenericTimer *timer=&e->cp15.c14_timer[GTIMER_VIRT];
    /* A disabled timer cannot assert its interrupt. Avoid expensive clock
     * conversion on every TB until guest software enables the timer. */
    if(!(timer->ctl&1)){timer->ctl&=~4u;if(e->monitor_gic.level[0]&(1u<<27))arm_monitor_gic_level(e,27,false);return;}
    uint64_t ticks=muldiv64(qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL),env_archcpu(e)->gt_cntfrq_hz,NANOSECONDS_PER_SECOND)-e->cp15.cntvoff_el2;
    bool expired=(timer->ctl&1)&&((int64_t)(ticks-timer->cval)>=0);
    timer->ctl=(timer->ctl&~4u)|(expired?4:0);
    arm_monitor_gic_level(e,27,expired&&!(timer->ctl&2));
}
static CPAccessResult mon_gic_access(CPUARMState*e,const ARMCPRegInfo*r,bool read)
{return e->monitor_gic.configured?CP_ACCESS_OK:CP_ACCESS_TRAP;}
static unsigned mon_gic_group(const ARMCPRegInfo*r){return r->crm==12?1:0;}
static uint64_t mon_gic_read(CPUARMState*e,const ARMCPRegInfo*r)
{
    unsigned group=mon_gic_group(r);
    if(r->crn==4)return e->monitor_gic.pmr;
    if(r->crm==8 || (r->crm==12&&r->opc2<4)){
        if(r->opc2==3)return e->monitor_gic.bpr[group];
        unsigned id=mon_gic_best(e,group);
        if(r->opc2==0&&id<288){uint32_t bit=1u<<(id%32);e->monitor_gic.pending[id/32]&=~bit;e->monitor_gic.active[id/32]|=bit;e->monitor_gic.dropped[id/32]&=~bit;mon_gic_update(e);}return id;
    }
    if(r->crm==11){unsigned priority=255;for(unsigned i=0;i<288;i++)if(((e->monitor_gic.active[i/32]&~e->monitor_gic.dropped[i/32])&(1u<<(i%32)))&&e->monitor_gic.priority[i]<priority)priority=e->monitor_gic.priority[i];return priority;}
    switch(r->opc2){case 4:return e->monitor_gic.ctlr|(7u<<8);case 5:return 7;case 6:return e->monitor_gic.igrpen[0];case 7:return e->monitor_gic.igrpen[1];}return 0;
}
static void mon_gic_write(CPUARMState*e,const ARMCPRegInfo*r,uint64_t value)
{
    if(r->crm==11&&r->opc2>=5){
        /* One PE at affinity 0. IRM excludes this PE, so has no targets.
         * RS selects a group of 16 target affinities; only RS=0 reaches PE0.
         * This model has no secure/other-security-state Group 1 target. */
        unsigned id=(value>>24)&15;unsigned group=r->opc2==7?0:1;
        bool self=!(value&(1ULL<<40))&&!(value&0x00ff000000000000ULL)&&
                  !(value&0x000000ff00000000ULL)&&!(value&0x00ff0000ULL)&&
                  !(value&0x0000f00000000000ULL)&&(value&1);
        if(self&&r->opc2!=6&&!!(e->monitor_gic.group[0]&(1u<<id))==group)
            e->monitor_gic.pending[0]|=1u<<id;
    }
    else if(r->crn==4)e->monitor_gic.pmr=value&255;
    else if((r->crm==8||r->crm==12)&&r->opc2==3)e->monitor_gic.bpr[mon_gic_group(r)]=value&7;
    else if((r->crm==8||r->crm==12||r->crm==11)&&r->opc2==1){unsigned id=value&0xffffff;if(id<288){uint32_t bit=1u<<(id%32);if(r->crm!=11&&(e->monitor_gic.ctlr&2))e->monitor_gic.dropped[id/32]|=bit;else {e->monitor_gic.active[id/32]&=~bit;e->monitor_gic.dropped[id/32]&=~bit;}}}
    else if(r->crm==12){switch(r->opc2){case 4:e->monitor_gic.ctlr=value&2;break;case 6:e->monitor_gic.igrpen[0]=value&1;break;case 7:e->monitor_gic.igrpen[1]=value&1;break;}}
    mon_gic_update(e);
}
int arm_monitor_gic_mmio(CPUARMState*e,unsigned region,uint64_t offset,unsigned size,bool write,uint64_t*value)
{
    if(region>1 || (size!=1&&size!=2&&size!=4&&size!=8)||offset%size)return -1;
    uint64_t out=0;uint32_t v=*value;unsigned first=0,limit=9;uint64_t off=offset;
    if(region==1){
        if(offset==8&&size==8){if(!write)out=16;goto done;} /* Affinity 0, last redistributor */
        if(offset==0||offset==4){if(!write)out=offset==4?0x43b:0;goto done;}
        if(offset==0x14){if(write)e->monitor_gic.waker=(v&2)?6:0;else out=e->monitor_gic.waker;goto done;}
        if(offset<0x10000||offset>=0x20000)return -1;off-=0x10000;limit=1;
    }else{
        if(offset==0){if(write)e->monitor_gic.dist_ctlr=(v&0x37)|64;else out=e->monitor_gic.dist_ctlr;goto done;}
        if(offset==4){if(!write)out=8|(8u<<19);goto done;}
        if(offset==8){if(!write)out=0x43b;goto done;}
    }
    if(off>=0x400&&off<0x400+limit*32){unsigned index=off-0x400;if(index+size>limit*32)return -1;for(unsigned i=0;i<size;i++)if(write)e->monitor_gic.priority[index+i]=(*value>>(8*i))&255;else out|=(uint64_t)e->monitor_gic.priority[index+i]<<(8*i);goto done;}
    if(region==0&&off>=0x6100&&off<0x6100+256*8&&size==8){if(write&&*value)return -1;goto done;}
    if(region==0&&off>=0xd00&&off<0xd24){goto done;}
    if(size!=4)return -1;
    uint32_t*array=NULL;bool set=false,clear=false;unsigned bank=0;
    if(off>=0x80&&off<0x80+limit*4){array=e->monitor_gic.group;bank=(off-0x80)/4;}
    else if(off>=0x100&&off<0x100+limit*4){array=e->monitor_gic.enable;bank=(off-0x100)/4;set=true;}
    else if(off>=0x180&&off<0x180+limit*4){array=e->monitor_gic.enable;bank=(off-0x180)/4;clear=true;}
    else if(off>=0x200&&off<0x200+limit*4){array=e->monitor_gic.pending;bank=(off-0x200)/4;set=true;}
    else if(off>=0x280&&off<0x280+limit*4){array=e->monitor_gic.pending;bank=(off-0x280)/4;clear=true;}
    else if(off>=0x300&&off<0x300+limit*4){array=e->monitor_gic.active;bank=(off-0x300)/4;set=true;}
    else if(off>=0x380&&off<0x380+limit*4){array=e->monitor_gic.active;bank=(off-0x380)/4;clear=true;}
    else if(off>=0xc00&&off<0xc00+limit*8){array=e->monitor_gic.config;bank=(off-0xc00)/4;}
    if(array){if(write){if(set)array[bank]|=v;else if(clear)array[bank]&=~v;else array[bank]=v;}else out=array[bank];goto done;}
    /* Architectural peripheral ID words. No LPIs, ITS or extra processors. */
    if((off>=0xffd0&&off<=0xfffc)||(region==0&&off>=0xffd0&&off<=0xfffc)){if(!write)out=off==0xffe8?0x30:0;goto done;}
    return -1;
done:if(!write)*value=out;mon_gic_update(e);return 0;
}
