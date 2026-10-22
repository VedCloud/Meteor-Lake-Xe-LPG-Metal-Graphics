/* SPDX-License-Identifier: MIT */
#ifndef D27_MACHO_IMAGE_H
#define D27_MACHO_IMAGE_H
#include "a64_core.h"
#define D27_MAX_SEGMENTS 16
#define D27_MAX_IMAGE_BYTES (64u*1024u*1024u)
typedef struct {uint64_t address,bytes,offset,file_bytes;unsigned permissions;} D27Segment;
typedef struct {uint64_t base,bytes,entry;size_t count;D27Segment segments[D27_MAX_SEGMENTS];} D27Image;
/* ARM64 MH_EXECUTE only, LC_UNIXTHREAD ARM_THREAD_STATE64; no relocations.
 * Parse the complete file first. Invalid input leaves output unmodified. */
int d27_image_plan(const uint8_t *file,size_t length,D27Image *out);
/* Reparse instead of trusting a caller-provided plan. The entire image is staged
 * in private host memory. Guest physical addresses are never host pointers. */
int d27_image_load(const uint8_t *file,size_t length,uint64_t guest_base,
 uint8_t *buffer,size_t capacity,D27Region *regions,size_t region_capacity,
 D27Image *out,uint64_t *entry);
#endif
