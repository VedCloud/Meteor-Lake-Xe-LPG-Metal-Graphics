from pathlib import Path
u=Path('../unicorn-for-efi')
p=u/'qemu/target/arm/cpu.h';s=p.read_text().replace('bool arm_vmapple_pac_hvc(CPUARMState *env);','bool arm_vmapple_pac_hvc(CPUARMState *env);\nhwaddr arm_monitor_physical_address(CPUState *, vaddr, MMUAccessType, int, uintptr_t);');p.write_text(s)
p=u/'qemu/target/arm/tlb_helper.c';s=p.read_text();s+='''
#ifdef TARGET_AARCH64
/* Resolve monitor mappings through the real guest page tables. The legacy
 * Unicorn API assumes flat virtual maps; system profiles map physical RAM. */
hwaddr arm_monitor_physical_address(CPUState *cs, vaddr address,
                                   MMUAccessType access, int mmu_idx,
                                   uintptr_t retaddr)
{
    ARMCPU *cpu = ARM_CPU(cs); hwaddr physical;
    target_ulong page_size; int prot; MemTxAttrs attrs = {0};
    ARMMMUFaultInfo fi = {0};
    if (get_phys_addr(&cpu->env, address, access,
                     core_to_arm_mmu_idx(&cpu->env, mmu_idx),
                     &physical, &attrs, &prot, &page_size, &fi, NULL)) {
        cpu_restore_state(cs, retaddr, true);
        arm_deliver_fault(cpu, address, access, mmu_idx, &fi);
    }
    return physical;
}
#endif
''';p.write_text(s)
p=u/'qemu/accel/tcg/cputlb.c';s=p.read_text()
pos=s.index('static void notdirty_write(')
s=s[:pos]+'''/* Preserve flat-memory semantics for all generic CPU models. Only the
 * opt-in system profiles treat uc_mem_map addresses as guest physical RAM. */
static bool monitor_system_mmu(CPUState *cpu)
{
#ifdef TARGET_AARCH64
    return ARM_CPU(cpu)->apple_sprr || ARM_CPU(cpu)->vmapple_pac;
#else
    return false;
#endif
}
static MemoryRegion *monitor_memory_mapping(CPUArchState *env, target_ulong addr,
                                           MMUAccessType access, int mmu_idx,
                                           uintptr_t retaddr)
{
#ifdef TARGET_AARCH64
    if (monitor_system_mmu(env_cpu(env))) {
        hwaddr physical = arm_monitor_physical_address(env_cpu(env), addr,
                                                       access, mmu_idx, retaddr);
        return memory_mapping(env->uc, physical);
    }
#endif
    return memory_mapping(env->uc, addr);
}

'''+s[pos:]
s=s.replace('        mr = memory_mapping(cpu->uc, mem_vaddr);','''        if (monitor_system_mmu(cpu)) {
            ram_addr_t offset;
            RAMBlock *block = qemu_ram_block_from_host(cpu->uc,
                qemu_map_ram_ptr(cpu->uc, NULL, ram_addr), false, &offset);
            assert(block != NULL);
            mr = block->mr;
        } else {
            mr = memory_mapping(cpu->uc, mem_vaddr);
        }''')
s=s.replace('MemoryRegion *mr = memory_mapping(uc, addr);','MemoryRegion *mr = monitor_memory_mapping(env, addr, access_type, mmu_idx, retaddr);')
# Remaining occurrences are the load retry followed by store and store retry.
assert s.count('mr = memory_mapping(uc, addr);')==3
s=s.replace('mr = memory_mapping(uc, addr);','mr = monitor_memory_mapping(env, addr, access_type, mmu_idx, retaddr);',1)
s=s.replace('mr = memory_mapping(uc, addr);','mr = monitor_memory_mapping(env, addr, MMU_DATA_STORE, mmu_idx, retaddr);')
p.write_text(s)
# Map VA 1 GiB away from physical RAM. Flat host lookups must not pass this.
p=Path('sprr-tests.c');s=p.read_text().replace('0x10000,&desc,8','0x10008,&desc,8').replace('uint64_t x0=0x2000','uint64_t x0=0x40002000').replace('operation==2?0x2000:0x1000+operation*4','operation==2?0x40002000:0x40001000+operation*4')
s=s.replace('guest MMU permission checks and VHE/EL12 banking','nonidentity guest MMU permission checks and VHE/EL12 banking')
p.write_text(s)
# Update test requirements to explicitly require nonidentity translation coverage.
for f in ['test-firmware.py','test-vmapple-firmware.py']:
 p=Path(f);p.write_text(p.read_text().replace('768 guest MMU permission checks','768 nonidentity guest MMU permission checks'))
# Remove temporary whitespace-only diagnostic edit.
p=u/'qemu/target/arm/op_helper.c'
import subprocess
p.write_bytes(subprocess.check_output(['git','show','HEAD:qemu/target/arm/op_helper.c'],cwd=u))
