	.build_version macos, 27, 0	sdk_version 27, 0
	.section	__TEXT,__text,regular,pure_instructions
	.globl	_acpiNameLength                 ## -- Begin function acpiNameLength
	.p2align	4
_acpiNameLength:                        ## @acpiNameLength
## %bb.0:
	pushq	%rbp
	movq	%rsp, %rbp
	movq	(%rdi), %rax
	popq	%rbp
	jmpq	*304(%rax)                      ## TAILCALL
                                        ## -- End function
.subsections_via_symbols
