// SPDX-License-Identifier: MIT
// Same scalar payload runs in the firmware interpreter and QEMU's ARM CPU.
.section .text
.global _start
_start:
 movz x16, #0x4100, lsl #16
 add x17, x16, #1, lsl #12
 mov sp, x17
 mov x0, #10
 mov x1, #0
1: add x1, x1, x0
 subs x0, x0, #1
 b.ne 1b
 str x1, [x16]
 movz x2, #0xffff
 movk x2, #0xabcd, lsl #16
 str x2, [x16, #8]
 add w3, w2, #1
 str w3, [x16, #16]
 strb w3, [x16, #20]
 ldrb w4, [x16, #9]
 ldrh w5, [x16, #10]
 movn x6, #0
 adds x7, x6, #1
 mrs x8, nzcv
 str x8, [x16, #24]
 b.cs 2f
 mov x9, #0xdead
 b 3f
2: bl subroutine
3: str x9, [x16, #32]
 sub sp, sp, #32
 add x10, sp, #0
 str x10, [x16, #40]
 add sp, sp, #32
 adr x11, _start
 str x11, [x16, #48]
 eor x12, x2, x6, lsr #16
 str x12, [x16, #56]
 movz w13, #0x8000, lsl #16
 adds w14, w13, w13
 mrs x15, nzcv
 str x15, [x16, #64]
 msr nzcv, x8
 cbz x0, 4f
 mov x9, #0xbeef
4: cbnz x9, 5f
 mov x9, #0xbad
5: str x9, [x16, #72]
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
subroutine:
 mov x9, #42
 ret
terminal_name: .asciz ":tt"
.section .data
.balign 8
.global results
results: .space 80
open_args: .quad terminal_name,4,3
write_args: .quad 0,results,80
exit_args: .quad 0x20026,0
