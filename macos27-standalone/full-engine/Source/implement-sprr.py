from pathlib import Path
u=Path('/Volumes/Dell27Build/unicorn-for-efi')
def edit(name,a,b):
 p=u/name;s=p.read_text();assert s.count(a)==1,(name,s.count(a));p.write_text(s.replace(a,b))
edit('include/unicorn/arm64.h','    UC_CPU_ARM64_MAX,','    UC_CPU_ARM64_MAX,\n    /* Opt-in research profile: SPRR, not a complete Apple CPU. */\n    UC_CPU_ARM64_APPLE_SPRR,')
edit('qemu/target/arm/cpu.h','    /* Fields after this point are preserved across CPU reset. */','    /* Apple SPRR state is reset with architectural state. */\n    uint64_t sprr_config_el[3], sprr_pperm_el[3], sprr_uperm_el0;\n\n    /* Fields after this point are preserved across CPU reset. */')
edit('qemu/target/arm/cpu.h','    /* Coprocessor information */','    /* Opt-in implementation-defined permission model. Not GXF support. */\n    bool apple_sprr;\n\n    /* Coprocessor information */')
edit('qemu/target/arm/cpu64.c','struct ARMCPUInfo {','static void apple_sprr_initfn(struct uc_struct *uc, CPUState *obj)\n{\n    aarch64_max_initfn(uc, obj);\n    ARM_CPU(obj)->apple_sprr = true;\n}\n\nstruct ARMCPUInfo {')
edit('qemu/target/arm/cpu64.c','    { .name = "max",                .initfn = aarch64_max_initfn },','    { .name = "max",                .initfn = aarch64_max_initfn },\n    { .name = "apple-sprr-research", .initfn = apple_sprr_initfn },')
edit('qemu/target/arm/cpu64.c','uc->cpu_model >= sizeof(aarch64_cpus)','uc->cpu_model < 0 || uc->cpu_model >= ARRAY_SIZE(aarch64_cpus)')
# Implement only normal exception-level permissions. GXF instructions and registers
# deliberately remain undefined; guarded execution must not silently run as EL.
regs=[]
for name,crm,op2,kind,bank,access in [
 ('SPRR_UPERM_EL0',1,5,0,0,'PL0_RW'),
 ('SPRR_CONFIG_EL1',1,0,1,1,'PL1_RW'),('SPRR_CONFIG_EL2',14,2,1,2,'PL2_RW'),
 ('SPRR_CONFIG_EL12',15,4,1,1,'PL2_RW'),
 ('SPRR_PPERM_EL1',1,6,2,1,'PL1_RW'),('SPRR_PPERM_EL2',1,7,2,2,'PL2_RW'),
 ('SPRR_PPERM_EL12',15,7,2,1,'PL2_RW')]:
 regs.append('    { .name = "%s", .state = ARM_CP_STATE_AA64, .opc0 = 3, .opc1 = 6,\n      .crn = 15, .crm = %d, .opc2 = %d, .access = %s,\n      .type = ARM_CP_NO_RAW, .readfn = sprr_read, .writefn = sprr_write },'%(name,crm,op2,access))
extra='''/* SPRR register encodings and normal-EL permission matrix follow qemu-sptm
 * bad6336fb24211f8c4e8e946c259bd5b12417340. GPL-2.0-or-later.
 * EL1 aliases redirect under VHE; EL12 always names the EL1 bank. */
static uint64_t *sprr_register(CPUARMState *env, const ARMCPRegInfo *ri)
{
    int bank = 1;
    if (ri->crm == 1 && ri->opc2 == 5) { return &env->sprr_uperm_el0; }
    if ((ri->crm == 14 && ri->opc2 == 2) ||
        (ri->crm == 1 && ri->opc2 == 7)) { bank = 2; }
    else if (ri->crm == 1 && arm_current_el(env) == 2 &&
             (env->cp15.hcr_el2 & HCR_E2H)) { bank = 2; }
    return (ri->opc2 == 0 || ri->opc2 == 2 || ri->opc2 == 4) ?
        &env->sprr_config_el[bank] : &env->sprr_pperm_el[bank];
}
static uint64_t sprr_read(CPUARMState *env, const ARMCPRegInfo *ri)
{ return *sprr_register(env, ri); }
static void sprr_write(CPUARMState *env, const ARMCPRegInfo *ri, uint64_t value)
{
    *sprr_register(env, ri) = value;
    /* A permission change must invalidate already-cached translations. */
    tlb_flush(CPU(env_archcpu(env)));
}
static const ARMCPRegInfo apple_sprr_regs[] = {
'''+ '\n'.join(regs)+'''
    REGINFO_SENTINEL
};

'''
edit('qemu/target/arm/helper.c','void register_cp_regs_for_features(ARMCPU *cpu)',extra+'void register_cp_regs_for_features(ARMCPU *cpu)')
edit('qemu/target/arm/helper.c','    define_arm_cp_regs(cpu, cp_reginfo);','    define_arm_cp_regs(cpu, cp_reginfo);\n    if (cpu->apple_sprr) { define_arm_cp_regs(cpu, apple_sprr_regs); }')
edit('qemu/target/arm/helper.c','static bool get_phys_addr_lpae(CPUARMState *env, target_ulong address,\n                               MMUAccessType access_type, ARMMMUIdx mmu_idx,\n                               hwaddr *phys_ptr, MemTxAttrs *txattrs, int *prot,\n                               target_ulong *page_size_ptr,\n                               ARMMMUFaultInfo *fi, ARMCacheAttrs *cacheattrs)\n{','''/* Normal-EL SPRR permissions; guarded permissions require the unported GXF
 * execution state and are intentionally not advertised by this CPU profile. */
static int sprr_normal_prot(CPUARMState *env, unsigned descriptor_index)
{
    static const uint8_t matrix[16] = {
        0, PAGE_READ|PAGE_EXEC, PAGE_READ, PAGE_READ|PAGE_WRITE,
        0, PAGE_READ|PAGE_EXEC, PAGE_READ, 0,
        0, PAGE_EXEC, PAGE_READ, PAGE_READ|PAGE_WRITE,
        0, PAGE_READ|PAGE_EXEC, PAGE_READ, PAGE_READ|PAGE_WRITE
    };
    unsigned el = arm_current_el(env);
    uint64_t reg = el == 0 ? env->sprr_uperm_el0 : env->sprr_pperm_el[el];
    return matrix[(reg >> (4 * descriptor_index)) & 15];
}

static bool get_phys_addr_lpae(CPUARMState *env, target_ulong address,
                               MMUAccessType access_type, ARMMMUIdx mmu_idx,
                               hwaddr *phys_ptr, MemTxAttrs *txattrs, int *prot,
                               target_ulong *page_size_ptr,
                               ARMMMUFaultInfo *fi, ARMCacheAttrs *cacheattrs)
{''')
edit('qemu/target/arm/helper.c','    bool guarded = false;','    bool guarded = false;\n    unsigned sprr_index = 0;')
edit('qemu/target/arm/helper.c','        /* Extract attributes from the descriptor */','        /* SPRR indexes the leaf bits, before hierarchical AP/XN merging. */\n        sprr_index = extract64(descriptor, 53, 2) | (extract64(descriptor, 6, 2) << 2);\n        /* Extract attributes from the descriptor */')
edit('qemu/target/arm/helper.c','        *prot = get_S1prot(env, mmu_idx, aarch64, ap, ns, xn, pxn);','''        if (cpu->apple_sprr && aarch64 && arm_current_el(env) < 3 &&
            (env->sprr_config_el[1] || env->sprr_config_el[2])) {
            *prot = sprr_normal_prot(env, sprr_index);
        } else {
            *prot = get_S1prot(env, mmu_idx, aarch64, ap, ns, xn, pxn);
        }''')
p=Path('/Volumes/Dell27Build/UnicornBuild/boot-entry.c');p.write_text(p.read_text().replace('UC_CPU_ARM64_MAX','UC_CPU_ARM64_APPLE_SPRR'))
print('Applied opt-in SPRR state, register banking, TLB invalidation and stage-1 permissions')
