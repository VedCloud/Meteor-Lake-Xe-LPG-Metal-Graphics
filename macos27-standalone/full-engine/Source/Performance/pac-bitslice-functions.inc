/* SPDX-License-Identifier: LGPL-2.1-or-later; QEMU QARMA S-boxes, parallel Boolean evaluation. */
static uint64_t pac_sub(uint64_t i) {
 const uint64_t m=UINT64_C(0x1111111111111111);
 uint64_t x0=i&m,x1=(i>>1)&m,x2=(i>>2)&m,x3=(i>>3)&m;
 uint64_t y0=m ^ (x0) ^ (x1) ^ (x2) ^ (x0&x2) ^ (x0&x1&x2) ^ (x0&x3) ^ (x0&x1&x3) ^ (x2&x3) ^ (x1&x2&x3);
 uint64_t y1=m ^ (x1) ^ (x0&x1) ^ (x2) ^ (x1&x2) ^ (x0&x1&x3) ^ (x0&x2&x3);
 uint64_t y2=(x0) ^ (x2) ^ (x1&x2) ^ (x1&x3) ^ (x0&x1&x3) ^ (x1&x2&x3);
 uint64_t y3=m ^ (x0) ^ (x0&x1) ^ (x3) ^ (x0&x3) ^ (x0&x1&x3) ^ (x2&x3) ^ (x0&x2&x3) ^ (x1&x2&x3);
 return y0|(y1<<1)|(y2<<2)|(y3<<3);
}
static uint64_t pac_inv_sub(uint64_t i) {
 const uint64_t m=UINT64_C(0x1111111111111111);
 uint64_t x0=i&m,x1=(i>>1)&m,x2=(i>>2)&m,x3=(i>>3)&m;
 uint64_t y0=m ^ (x0) ^ (x2) ^ (x1&x2) ^ (x0&x1&x2) ^ (x3) ^ (x0&x3) ^ (x1&x3) ^ (x0&x1&x3) ^ (x2&x3) ^ (x1&x2&x3);
 uint64_t y1=(x0) ^ (x0&x1) ^ (x2) ^ (x0&x2) ^ (x1&x2) ^ (x0&x1&x2) ^ (x3) ^ (x0&x3) ^ (x0&x2&x3);
 uint64_t y2=m ^ (x0&x1) ^ (x2) ^ (x0&x1&x2) ^ (x3) ^ (x0&x3) ^ (x1&x3) ^ (x0&x1&x3) ^ (x0&x2&x3) ^ (x1&x2&x3);
 uint64_t y3=(x0) ^ (x1) ^ (x0&x1) ^ (x2) ^ (x0&x2) ^ (x0&x3) ^ (x2&x3) ^ (x1&x2&x3);
 return y0|(y1<<1)|(y2<<2)|(y3<<3);
}
