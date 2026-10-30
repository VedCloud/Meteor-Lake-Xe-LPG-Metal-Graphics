/* SPDX-License-Identifier: LGPL-2.1-or-later; QEMU QARMA mixing, parallel evaluation. */
static uint64_t pac_mult(uint64_t i)
{
    /* Rotate every nibble in parallel, then XOR the four 16-bit rows. */
    uint64_t one = ((i << 1) & UINT64_C(0xeeeeeeeeeeeeeeee)) |
                   ((i >> 3) & UINT64_C(0x1111111111111111));
    uint64_t two = ((i << 2) & UINT64_C(0xcccccccccccccccc)) |
                   ((i >> 2) & UINT64_C(0x3333333333333333));
    uint64_t a0=one&0xffff, a1=(one>>16)&0xffff;
    uint64_t a2=(one>>32)&0xffff, a3=one>>48;
    uint64_t b0=two&0xffff, b1=(two>>16)&0xffff;
    uint64_t b2=(two>>32)&0xffff, b3=two>>48;
    return (a3 ^ b2 ^ a1) | ((b3 ^ a2 ^ a0) << 16) |
           ((a3 ^ a1 ^ b0) << 32) | ((a2 ^ b1 ^ a0) << 48);
}
