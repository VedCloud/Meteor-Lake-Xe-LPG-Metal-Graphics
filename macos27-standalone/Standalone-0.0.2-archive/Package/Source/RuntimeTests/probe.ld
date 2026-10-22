/* SPDX-License-Identifier: MIT */
ENTRY(_start)
SECTIONS { . = 0; .text : { *(.text*) } . = 0x41000000; .data : { *(.data*) } /DISCARD/ : { *(.comment) *(.note*) } }
