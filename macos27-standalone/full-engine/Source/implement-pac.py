from pathlib import Path
u=Path('/Volumes/Dell27Build/unicorn-for-efi')
def edit(name,a,b):
 p=u/name;s=p.read_text();assert s.count(a)==1,(name,s.count(a));p.write_text(s.replace(a,b))
edit('include/unicorn/arm64.h','    UC_CPU_ARM64_APPLE_SPRR,','    UC_CPU_ARM64_APPLE_SPRR,\n    UC_CPU_ARM64_VMAPPLE_PAC,')
# API prototype must follow uc_engine/uc_err typedef: unicorn.h includes arm64.h
# before those definitions, so use opaque struct and int in the public extension.
edit('include/unicorn/arm64.h','// ARM64 coprocessor registers, use this with UC_ARM64_REG_CP_REG to','''/* Research extension. Configure before first HVC, after engine initialization.
 * Secret and defaults must come from the monitor's cryptographic RNG. They are
 * host-only state; this API does not expose them as guest system registers. */
struct uc_struct;
int uc_arm64_vmapple_configure(struct uc_struct *, const unsigned char secret[16],
                              const unsigned long long defaults[4]);

// ARM64 coprocessor registers, use this with UC_ARM64_REG_CP_REG to''')
edit('qemu/target/arm/cpu.h','    uint64_t sprr_config_el[3], sprr_pperm_el[3], sprr_uperm_el0;','''    uint64_t sprr_config_el[3], sprr_pperm_el[3], sprr_uperm_el0;
    struct {
        uint8_t secret[16];
        uint64_t defaults[4], diversifier;
        bool configured, initialized, el1_diversified;
    } vmapple_pac;''')
edit('qemu/target/arm/cpu.h','    bool apple_sprr;','    bool apple_sprr, vmapple_pac;')
edit('qemu/target/arm/cpu.h','typedef void ARMELChangeHookFn(ARMCPU *cpu, void *opaque);','''#ifdef TARGET_AARCH64
bool arm_vmapple_pac_hvc(CPUARMState *env);
#endif

typedef void ARMELChangeHookFn(ARMCPU *cpu, void *opaque);''')
edit('qemu/target/arm/cpu64.c','struct ARMCPUInfo {','''static void vmapple_pac_initfn(struct uc_struct *uc, CPUState *obj)
{
    aarch64_max_initfn(uc, obj);
    ARM_CPU(obj)->vmapple_pac = true;
}

struct ARMCPUInfo {''')
edit('qemu/target/arm/cpu64.c','    { .name = "apple-sprr-research", .initfn = apple_sprr_initfn },','    { .name = "apple-sprr-research", .initfn = apple_sprr_initfn },\n    { .name = "vmapple-pac-research", .initfn = vmapple_pac_initfn },')
edit('qemu/target/arm/unicorn_aarch64.c','static void arm64_set_pc(struct uc_struct *uc, uint64_t address)','''int uc_arm64_vmapple_configure(struct uc_struct *uc,
                              const unsigned char secret[16],
                              const unsigned long long defaults[4])
{
    if (!uc || uc->arch != UC_ARCH_ARM64 || !uc->cpu || !secret || !defaults ||
        !ARM_CPU(uc->cpu)->vmapple_pac) { return UC_ERR_ARG; }
    CPUARMState *env = &ARM_CPU(uc->cpu)->env;
    if (env->vmapple_pac.configured) { return UC_ERR_ARG; }
    memcpy(env->vmapple_pac.secret, secret, 16);
    memcpy(env->vmapple_pac.defaults, defaults, 32);
    env->vmapple_pac.configured = true;
    return UC_ERR_OK;
}

static void arm64_set_pc(struct uc_struct *uc, uint64_t address)''')
# Included in helper.c so it shares the engine's namespace and GPL license.
p=u/'qemu/target/arm/vmapple-pac.c.inc'
p.write_text('''/* SPDX-License-Identifier: GPL-2.0-or-later
 * VMAPPLE HVC ABI, backed by the existing software QARMA PAC implementation.
 * This is a consistent software VM cipher, not Apple's private hardware cipher.
 * Key derivation: AES-128(secret, input64LE || role64LE). Each instruction/data,
 * A/B/G role has a distinct domain. No success if entropy was not configured.
 */
#include "crypto/aes.h"
static ARMPACKey vmapple_derive(CPUARMState *env, uint64_t input, uint64_t role)
{
    uint8_t in[16], out[16]; AES_KEY aes; ARMPACKey key;
    for (unsigned i = 0; i < 8; i++) {
        in[i] = input >> (8*i); in[i+8] = role >> (8*i);
    }
    AES_set_encrypt_key(env->vmapple_pac.secret, 128, &aes);
    AES_encrypt(in, out, &aes);
    key.lo = ldq_le_p(out); key.hi = ldq_le_p(out+8);
    memset(&aes, 0, sizeof(aes)); memset(out, 0, sizeof(out));
    return key;
}
static void vmapple_set_a(CPUARMState *env, uint64_t input)
{
    env->keys.apia = vmapple_derive(env, input, 0x41495041);
    env->keys.apda = vmapple_derive(env, input, 0x41445041);
}
static void vmapple_set_b(CPUARMState *env, uint64_t input)
{
    env->keys.apib = vmapple_derive(env, input, 0x42495041);
    env->keys.apdb = vmapple_derive(env, input, 0x42445041);
}
bool arm_vmapple_pac_hvc(CPUARMState *env)
{
    if (!env_archcpu(env)->vmapple_pac || arm_current_el(env) != 1 ||
        (env->exception.syndrome & 0xffff) != 0) { return false; }
    uint64_t call = env->xregs[0];
    if (call < 0xc1000000ULL || call > 0xc1000006ULL) { return false; }
    if (!env->vmapple_pac.configured ||
        (call != 0xc1000000 && !env->vmapple_pac.initialized)) {
        env->xregs[0] = UINT64_MAX; return true;
    }
    switch (call) {
    case 0xc1000000:
        vmapple_set_a(env, env->vmapple_pac.defaults[0]);
        vmapple_set_b(env, env->vmapple_pac.defaults[1]);
        env->keys.apga = vmapple_derive(env, env->vmapple_pac.defaults[3], 0x41475041);
        env->vmapple_pac.diversifier = env->vmapple_pac.defaults[2];
        env->vmapple_pac.el1_diversified = false;
        env->vmapple_pac.initialized = true;
        break;
    case 0xc1000001:
        for (unsigned i=0; i<4; i++) { env->xregs[i+1] = env->vmapple_pac.defaults[i]; }
        break;
    case 0xc1000002: vmapple_set_a(env, env->xregs[1]); break;
    case 0xc1000003: vmapple_set_b(env, env->xregs[1]); break;
    case 0xc1000004: env->vmapple_pac.diversifier = env->xregs[1]; break;
    case 0xc1000005:
        if (env->xregs[1] > 1) { env->xregs[0] = UINT64_MAX; return true; }
        env->vmapple_pac.el1_diversified = env->xregs[1];
        env->vmapple_pac.diversifier = env->xregs[2];
        break;
    case 0xc1000006:
        env->keys.apga = vmapple_derive(env, env->xregs[1], 0x41475041); break;
    }
    env->xregs[0] = 0;
    return true;
}
''')
edit('qemu/target/arm/helper.c','#define ARM_CPU_FREQ 1000000000', '#ifdef TARGET_AARCH64\n#include "vmapple-pac.c.inc"\n#endif\n\n#define ARM_CPU_FREQ 1000000000')
edit('qemu/accel/tcg/cpu-exec.c','        // Unicorn: call registered interrupt callbacks','''#if defined(TARGET_AARCH64)
        /* HVC PC already points after the instruction. Handle only the opted-in
         * VMAPPLE PAC ABI. Unknown calls retain Unicorn's exception behavior. */
        if (cpu->exception_index == EXCP_HVC &&
            arm_vmapple_pac_hvc((CPUARMState *)cpu->env_ptr)) {
            cpu->exception_index = -1;
            *ret = EXCP_INTERRUPT;
            return false;
        }
#endif
        // Unicorn: call registered interrupt callbacks''')
# Compute effective A/B keys at the point of PAC use. This handles both ERET
# and exception entry automatically; no out-of-sync key swap on EL changes.
extra='''/* Automatically select EL0-diversified A/B keys for PAC operations.
 * Generic G keys do not participate in EL0 diversification. */
static ARMPACKey vmapple_effective_key(CPUARMState *env, ARMPACKey *base)
{
    ARMPACKey result = *base;
    if (env_archcpu(env)->vmapple_pac && env->vmapple_pac.initialized &&
        (arm_current_el(env) == 0 ||
         (arm_current_el(env) == 1 && env->vmapple_pac.el1_diversified))) {
        uint8_t raw[16], in[16] = {0}, out[16]; AES_KEY aes;
        stq_le_p(raw, base->lo); stq_le_p(raw+8, base->hi);
        stq_le_p(in, env->vmapple_pac.diversifier);
        stq_le_p(in+8, 0x5649445f304c45ULL); /* EL0_DIV domain */
        AES_set_encrypt_key(raw, 128, &aes); AES_encrypt(in, out, &aes);
        result.lo = ldq_le_p(out); result.hi = ldq_le_p(out+8);
        memset(&aes, 0, sizeof(aes)); memset(raw, 0, sizeof(raw));
        memset(out, 0, sizeof(out));
    }
    return result;
}

'''
edit('qemu/target/arm/pauth_helper.c','static uint64_t pauth_addpac(', '#include "crypto/aes.h"\n\n'+extra+'static uint64_t pauth_addpac(')
edit('qemu/target/arm/pauth_helper.c','pac = pauth_computepac(ext_ptr, modifier, *key);','pac = pauth_computepac(ext_ptr, modifier, vmapple_effective_key(env, key));')
edit('qemu/target/arm/pauth_helper.c','pac = pauth_computepac(orig_ptr, modifier, *key);','pac = pauth_computepac(orig_ptr, modifier, vmapple_effective_key(env, key));')
print('Connected VMAPPLE PAC HVC dispatch, key derivation, and per-operation EL0 diversification')
