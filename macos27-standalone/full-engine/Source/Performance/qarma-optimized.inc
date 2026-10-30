/*
 * ARM v8.3-PAuth Operations
 *
 * Copyright (c) 2019 Linaro, Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

static uint64_t optimized_pac_cell_shuffle(uint64_t i)
{
    uint64_t o = 0;

    o |= extract64(i, 52, 4);
    o |= extract64(i, 24, 4) << 4;
    o |= extract64(i, 44, 4) << 8;
    o |= extract64(i,  0, 4) << 12;

    o |= extract64(i, 28, 4) << 16;
    o |= extract64(i, 48, 4) << 20;
    o |= extract64(i,  4, 4) << 24;
    o |= extract64(i, 40, 4) << 28;

    o |= extract64(i, 32, 4) << 32;
    o |= extract64(i, 12, 4) << 36;
    o |= extract64(i, 56, 4) << 40;
    o |= extract64(i, 20, 4) << 44;

    o |= extract64(i,  8, 4) << 48;
    o |= extract64(i, 36, 4) << 52;
    o |= extract64(i, 16, 4) << 56;
    o |= extract64(i, 60, 4) << 60;

    return o;
}

static uint64_t optimized_pac_cell_inv_shuffle(uint64_t i)
{
    uint64_t o = 0;

    o |= extract64(i, 12, 4);
    o |= extract64(i, 24, 4) << 4;
    o |= extract64(i, 48, 4) << 8;
    o |= extract64(i, 36, 4) << 12;

    o |= extract64(i, 56, 4) << 16;
    o |= extract64(i, 44, 4) << 20;
    o |= extract64(i,  4, 4) << 24;
    o |= extract64(i, 16, 4) << 28;

    o |= i & MAKE_64BIT_MASK(32, 4);
    o |= extract64(i, 52, 4) << 36;
    o |= extract64(i, 28, 4) << 40;
    o |= extract64(i,  8, 4) << 44;

    o |= extract64(i, 20, 4) << 48;
    o |= extract64(i,  0, 4) << 52;
    o |= extract64(i, 40, 4) << 56;
    o |= i & MAKE_64BIT_MASK(60, 4);

    return o;
}

static uint64_t optimized_pac_sub(uint64_t i) {
 const uint64_t m=UINT64_C(0x1111111111111111);
 uint64_t x0=i&m,x1=(i>>1)&m,x2=(i>>2)&m,x3=(i>>3)&m;
 uint64_t y0=m ^ (x0) ^ (x1) ^ (x2) ^ (x0&x2) ^ (x0&x1&x2) ^ (x0&x3) ^ (x0&x1&x3) ^ (x2&x3) ^ (x1&x2&x3);
 uint64_t y1=m ^ (x1) ^ (x0&x1) ^ (x2) ^ (x1&x2) ^ (x0&x1&x3) ^ (x0&x2&x3);
 uint64_t y2=(x0) ^ (x2) ^ (x1&x2) ^ (x1&x3) ^ (x0&x1&x3) ^ (x1&x2&x3);
 uint64_t y3=m ^ (x0) ^ (x0&x1) ^ (x3) ^ (x0&x3) ^ (x0&x1&x3) ^ (x2&x3) ^ (x0&x2&x3) ^ (x1&x2&x3);
 return y0|(y1<<1)|(y2<<2)|(y3<<3);
}

static uint64_t optimized_pac_inv_sub(uint64_t i) {
 const uint64_t m=UINT64_C(0x1111111111111111);
 uint64_t x0=i&m,x1=(i>>1)&m,x2=(i>>2)&m,x3=(i>>3)&m;
 uint64_t y0=m ^ (x0) ^ (x2) ^ (x1&x2) ^ (x0&x1&x2) ^ (x3) ^ (x0&x3) ^ (x1&x3) ^ (x0&x1&x3) ^ (x2&x3) ^ (x1&x2&x3);
 uint64_t y1=(x0) ^ (x0&x1) ^ (x2) ^ (x0&x2) ^ (x1&x2) ^ (x0&x1&x2) ^ (x3) ^ (x0&x3) ^ (x0&x2&x3);
 uint64_t y2=m ^ (x0&x1) ^ (x2) ^ (x0&x1&x2) ^ (x3) ^ (x0&x3) ^ (x1&x3) ^ (x0&x1&x3) ^ (x0&x2&x3) ^ (x1&x2&x3);
 uint64_t y3=(x0) ^ (x1) ^ (x0&x1) ^ (x2) ^ (x0&x2) ^ (x0&x3) ^ (x2&x3) ^ (x1&x2&x3);
 return y0|(y1<<1)|(y2<<2)|(y3<<3);
}

static int optimized_rot_cell(int cell, int n)
{
    /* 4-bit rotate left by n.  */
    cell |= cell << 4;
    return extract32(cell, 4 - n, 4);
}

static uint64_t optimized_pac_mult(uint64_t i)
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

static uint64_t optimized_tweak_cell_rot(uint64_t cell)
{
    return (cell >> 1) | (((cell ^ (cell >> 1)) & 1) << 3);
}

static uint64_t optimized_tweak_shuffle(uint64_t i)
{
    uint64_t o = 0;

    o |= extract64(i, 16, 4) << 0;
    o |= extract64(i, 20, 4) << 4;
    o |= optimized_tweak_cell_rot(extract64(i, 24, 4)) << 8;
    o |= extract64(i, 28, 4) << 12;

    o |= optimized_tweak_cell_rot(extract64(i, 44, 4)) << 16;
    o |= extract64(i,  8, 4) << 20;
    o |= extract64(i, 12, 4) << 24;
    o |= optimized_tweak_cell_rot(extract64(i, 32, 4)) << 28;

    o |= extract64(i, 48, 4) << 32;
    o |= extract64(i, 52, 4) << 36;
    o |= extract64(i, 56, 4) << 40;
    o |= optimized_tweak_cell_rot(extract64(i, 60, 4)) << 44;

    o |= optimized_tweak_cell_rot(extract64(i,  0, 4)) << 48;
    o |= extract64(i,  4, 4) << 52;
    o |= optimized_tweak_cell_rot(extract64(i, 40, 4)) << 56;
    o |= optimized_tweak_cell_rot(extract64(i, 36, 4)) << 60;

    return o;
}

static uint64_t optimized_tweak_cell_inv_rot(uint64_t cell)
{
    return ((cell << 1) & 0xf) | ((cell & 1) ^ (cell >> 3));
}

static uint64_t optimized_tweak_inv_shuffle(uint64_t i)
{
    uint64_t o = 0;

    o |= optimized_tweak_cell_inv_rot(extract64(i, 48, 4));
    o |= extract64(i, 52, 4) << 4;
    o |= extract64(i, 20, 4) << 8;
    o |= extract64(i, 24, 4) << 12;

    o |= extract64(i,  0, 4) << 16;
    o |= extract64(i,  4, 4) << 20;
    o |= optimized_tweak_cell_inv_rot(extract64(i,  8, 4)) << 24;
    o |= extract64(i, 12, 4) << 28;

    o |= optimized_tweak_cell_inv_rot(extract64(i, 28, 4)) << 32;
    o |= optimized_tweak_cell_inv_rot(extract64(i, 60, 4)) << 36;
    o |= optimized_tweak_cell_inv_rot(extract64(i, 56, 4)) << 40;
    o |= optimized_tweak_cell_inv_rot(extract64(i, 16, 4)) << 44;

    o |= extract64(i, 32, 4) << 48;
    o |= extract64(i, 36, 4) << 52;
    o |= extract64(i, 40, 4) << 56;
    o |= optimized_tweak_cell_inv_rot(extract64(i, 44, 4)) << 60;

    return o;
}

static uint64_t optimized_pauth_computepac(uint64_t data, uint64_t modifier,
                                 ARMPACKey key)
{
    static const uint64_t RC[5] = {
        0x0000000000000000ull,
        0x13198A2E03707344ull,
        0xA4093822299F31D0ull,
        0x082EFA98EC4E6C89ull,
        0x452821E638D01377ull,
    };
    const uint64_t alpha = 0xC0AC29B7C97C50DDull;
    /*
     * Note that in the ARM pseudocode, key0 contains bits <127:64>
     * and key1 contains bits <63:0> of the 128-bit key.
     */
    uint64_t key0 = key.hi, key1 = key.lo;
    uint64_t workingval, runningmod, roundkey, modk0;
    int i;

    modk0 = (key0 << 63) | ((key0 >> 1) ^ (key0 >> 63));
    runningmod = modifier;
    workingval = data ^ key0;

    for (i = 0; i <= 4; ++i) {
        roundkey = key1 ^ runningmod;
        workingval ^= roundkey;
        workingval ^= RC[i];
        if (i > 0) {
            workingval = optimized_pac_cell_shuffle(workingval);
            workingval = optimized_pac_mult(workingval);
        }
        workingval = optimized_pac_sub(workingval);
        runningmod = optimized_tweak_shuffle(runningmod);
    }
    roundkey = modk0 ^ runningmod;
    workingval ^= roundkey;
    workingval = optimized_pac_cell_shuffle(workingval);
    workingval = optimized_pac_mult(workingval);
    workingval = optimized_pac_sub(workingval);
    workingval = optimized_pac_cell_shuffle(workingval);
    workingval = optimized_pac_mult(workingval);
    workingval ^= key1;
    workingval = optimized_pac_cell_inv_shuffle(workingval);
    workingval = optimized_pac_inv_sub(workingval);
    workingval = optimized_pac_mult(workingval);
    workingval = optimized_pac_cell_inv_shuffle(workingval);
    workingval ^= key0;
    workingval ^= runningmod;
    for (i = 0; i <= 4; ++i) {
        workingval = optimized_pac_inv_sub(workingval);
        if (i < 4) {
            workingval = optimized_pac_mult(workingval);
            workingval = optimized_pac_cell_inv_shuffle(workingval);
        }
        runningmod = optimized_tweak_inv_shuffle(runningmod);
        roundkey = key1 ^ runningmod;
        workingval ^= RC[4 - i];
        workingval ^= roundkey;
        workingval ^= alpha;
    }
    workingval ^= modk0;

    return workingval;
}

