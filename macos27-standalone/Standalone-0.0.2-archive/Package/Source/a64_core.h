/* SPDX-License-Identifier: MIT */
#ifndef DELL27_A64_CORE_H
#define DELL27_A64_CORE_H
#include <stdint.h>
#include <stddef.h>
enum D27Permission { D27_READ=1, D27_WRITE=2, D27_EXEC=4 };
enum D27Stop { D27_RUNNING=0, D27_BREAKPOINT, D27_UNSUPPORTED, D27_FETCH_FAULT,
 D27_DATA_FAULT, D27_PC_ALIGNMENT, D27_SP_ALIGNMENT, D27_BUDGET, D27_BAD_MAP };
typedef struct { uint64_t base; uint8_t *bytes; size_t length; unsigned permissions; } D27Region;
typedef struct {
 uint64_t x[31],sp,pc,retired;
 uint32_t nzcv,instruction,syndrome;
 uint64_t fault_address;
 enum D27Stop stop;
} D27Cpu;
/* Single-CPU, little-endian, flat-address bring-up interpreter. Guest addresses
 * are resolved only through explicit private buffers, never host pointer casts.
 * Unsupported privileged/system instructions stop; no fake monitor services.
 * No MMU, exception delivery, floating point/SIMD, PAC or atomic instructions.
 * This is an instruction-probe backend, not the production XNU execution engine. */
enum D27Stop d27_a64_run(D27Cpu *,D27Region *,size_t count,uint64_t budget);
#endif
