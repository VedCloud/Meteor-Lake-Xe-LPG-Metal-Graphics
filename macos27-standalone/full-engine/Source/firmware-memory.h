/* SPDX-License-Identifier: MIT
 * Ordinary RAM operations for the freestanding runtime. No SIMD state or
 * direction-flag assumptions; all accesses stay inside the supplied range.
 */
#ifndef DELL27_FIRMWARE_MEMORY_H
#define DELL27_FIRMWARE_MEMORY_H
#include <stdint.h>
#include <stddef.h>
typedef uint64_t d27_unaligned_word __attribute__((aligned(1), may_alias));
static inline void *d27_memory_copy(void *destination, const void *source, size_t bytes)
{
    unsigned char *d = destination;
    const unsigned char *s = source;
    if (!bytes || d == s) return destination;
    if ((uintptr_t)d < (uintptr_t)s || (uintptr_t)d - (uintptr_t)s >= bytes) {
        while (bytes >= sizeof(d27_unaligned_word)) {
            *(d27_unaligned_word *)d = *(const d27_unaligned_word *)s;
            d += sizeof(d27_unaligned_word);
            s += sizeof(d27_unaligned_word);
            bytes -= sizeof(d27_unaligned_word);
        }
        while (bytes--) *d++ = *s++;
    } else {
        while (bytes >= sizeof(d27_unaligned_word)) {
            bytes -= sizeof(d27_unaligned_word);
            *(d27_unaligned_word *)(d + bytes) = *(const d27_unaligned_word *)(s + bytes);
        }
        while (bytes) { --bytes; d[bytes] = s[bytes]; }
    }
    return destination;
}
static inline void *d27_memory_set(void *destination, size_t bytes, unsigned char value)
{
    unsigned char *d = destination;
    uint64_t word = UINT64_C(0x0101010101010101) * value;
    while (bytes >= sizeof(d27_unaligned_word)) {
        *(d27_unaligned_word *)d = word;
        d += sizeof(d27_unaligned_word);
        bytes -= sizeof(d27_unaligned_word);
    }
    while (bytes--) *d++ = value;
    return destination;
}
#endif
