// SPDX-License-Identifier: MIT
.section .text
.global _start
_start:
 movz x16, #0x4100, lsl #16
 msr oslar_el1, xzr
 msr daifclr, #15
 msr daifset, #15
 mrs x0, daif
 str x0, [x16]
 msr daifclr, #3
 mrs x0, daif
 str x0, [x16, #8]
 movz x0, #0x5000
 movk x0, #0x1234, lsl #16
 msr vbar_el1, x0
 isb
 mrs x0, vbar_el1
 str x0, [x16, #16]
 mrs x0, currentel
 str x0, [x16, #24]
 mrs x0, hcr_el2
 str x0, [x16, #32]
.global probe_stop
probe_stop:
 brk #0x27
// The reference image replaces only the preceding BRK with NOP. Its harness
// then writes the completed result buffer through QEMU semihosting and exits.
 adrp x1, open_args
 add x1, x1, :lo12:open_args
 mov x0, #1
 hlt #0xf000
 adrp x1, write_args
 add x1, x1, :lo12:write_args
 str x0, [x1]
 mov x0, #5
 hlt #0xf000
 adrp x1, exit_args
 add x1, x1, :lo12:exit_args
 mov x0, #0x20
 hlt #0xf000
terminal_name: .asciz ":tt"
.section .data
.balign 8
.global results
results: .space 80
open_args: .quad terminal_name,4,3
write_args: .quad 0,results,80
exit_args: .quad 0x20026,0
