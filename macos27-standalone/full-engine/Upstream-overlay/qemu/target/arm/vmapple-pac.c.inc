/* SPDX-License-Identifier: GPL-2.0-or-later
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
/* PSCI 1.1 discovery and affinity for the monitor's single running CPU.
 * ABI values follow QEMU target/arm/tcg/psci.c and Arm DEN0022. Power-state
 * transitions are not advertised or acknowledged without an implementation.
 * In particular a guest reset must never reset the physical host firmware.
 */
static bool vmapple_psci(CPUARMState *env, uint64_t call)
{
    if ((call & 0xffff0000ULL) != 0x84000000ULL &&
        (call & 0xffff0000ULL) != 0xc4000000ULL) return false;
    uint64_t target = env->xregs[1];
    uint64_t level = env->xregs[2];
    if (!(call & (1ULL << 30))) { target = (uint32_t)target; level = (uint32_t)level; }
    int64_t result = -1; /* PSCI_NOT_SUPPORTED */
    switch (call) {
    case 0x84000000: result = 0x10001; break; /* PSCI_VERSION */
    case 0x8400000a: /* PSCI_FEATURES */
        switch ((uint32_t)target) {
        case 0x84000000: case 0x8400000a:
        case 0x84000003: case 0xc4000003:
        case 0x84000004: case 0xc4000004:
        case 0x84000006: result = 0; break;
        }
        break;
    case 0x84000004: case 0xc4000004: /* AFFINITY_INFO */
        result = level > 3 || (target & 0xff00ffffffULL) !=
                 env_archcpu(env)->mp_affinity ? -2 : 0; break;
    case 0x84000003: case 0xc4000003: /* CPU_ON */
        result = (target & 0xff00ffffffULL) == env_archcpu(env)->mp_affinity ? -4 : -2;
        break; /* CPU0 is already on; no phantom secondary CPU. */
    case 0x84000006: result = 2; break; /* MIGRATE_INFO_TYPE: no trusted OS */
    }
    env->xregs[0] = result;
    return true;
}
bool arm_vmapple_pac_hvc(CPUARMState *env)
{
    if (!env_archcpu(env)->vmapple_pac || arm_current_el(env) != 1 ||
        (env->exception.syndrome & 0xffff) != 0) { return false; }
    uint64_t call = env->xregs[0];
    if (vmapple_psci(env, call)) return true;
    /* No Apple OEM provider is installed yet. SMCCC discovery must report
     * NOT_SUPPORTED rather than invent an Apple UID or advertise OEM devices. */
    if (call == 0x8300ff01ULL || call == 0x8300ff03ULL ||
        call == 0x8300feffULL) { env->xregs[0] = UINT64_MAX; return true; }
    /* VMAPPLE_PAC_NOP is the published CPU-service no-operation. It
     * deliberately changes no key state and is separate from unknown calls. */
    if (call == 0xc10000f0ULL) { env->xregs[0] = 0; return true; }
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
    env->vmapple_pac.calls[call - 0xc1000000ULL]++;
    env->xregs[0] = 0;
    return true;
}
