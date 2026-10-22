/* SPDX-License-Identifier: MIT */
#ifndef DELL27_BOOT_INPUT_H
#define DELL27_BOOT_INPUT_H
#include <stdint.h>
#include <stddef.h>
typedef struct { uint32_t state[8]; uint64_t bytes; uint8_t block[64]; size_t used; } D27Sha256;
void d27_sha_init(D27Sha256 *);
void d27_sha_update(D27Sha256 *, const void *, size_t);
void d27_sha_final(D27Sha256 *, uint8_t out[32]);
/* Validates the complete load-command region of an ARM64 MH_FILESET.
 * file_bytes is the actual file size; available may contain only its prefix.
 * Returns zero on acceptance. It does not map segments or execute any code. */
int d27_kernel_header(const uint8_t *, size_t available, uint64_t file_bytes);
#endif
