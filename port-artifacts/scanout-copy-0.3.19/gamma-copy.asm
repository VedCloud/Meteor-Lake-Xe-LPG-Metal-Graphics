0000000000007b90 <__ZL16nmxCopyWithGammaP9IOServicePVjPVKjjjjjPy>:
    7b90: 55                           	pushq	%rbp
    7b91: 48 89 e5                     	movq	%rsp, %rbp
    7b94: 41 57                        	pushq	%r15
    7b96: 41 56                        	pushq	%r14
    7b98: 41 55                        	pushq	%r13
    7b9a: 41 54                        	pushq	%r12
    7b9c: 53                           	pushq	%rbx
    7b9d: 48 81 ec 68 05 00 00         	subq	$0x568, %rsp            ## imm = 0x568
    7ba4: 45 89 cc                     	movl	%r9d, %r12d
    7ba7: 44 89 85 c0 fd ff ff         	movl	%r8d, -0x240(%rbp)
    7bae: 89 8d c8 fd ff ff            	movl	%ecx, -0x238(%rbp)
    7bb4: 48 89 d3                     	movq	%rdx, %rbx
    7bb7: 49 89 f6                     	movq	%rsi, %r14
    7bba: 49 89 fd                     	movq	%rdi, %r13
    7bbd: 48 8b 05 04 97 01 00         	movq	0x19704(%rip), %rax     ## 0x212c8 <_thread_call_enter_delayed+0x212c8>
    7bc4: 48 8b 00                     	movq	(%rax), %rax
    7bc7: 48 89 45 d0                  	movq	%rax, -0x30(%rbp)
    7bcb: 48 8d bd 90 fa ff ff         	leaq	-0x570(%rbp), %rdi
    7bd2: be 00 03 00 00               	movl	$0x300, %esi            ## imm = 0x300
    7bd7: e8 00 00 00 00               	callq	0x7bdc <__ZL16nmxCopyWithGammaP9IOServicePVjPVKjjjjjPy+0x4c>
    7bdc: 48 c7 85 80 fa ff ff 01 00 00 00     	movq	$0x1, -0x580(%rbp)
    7be7: 48 c7 85 88 fa ff ff 00 00 00 00     	movq	$0x0, -0x578(%rbp)
    7bf2: 31 c0                        	xorl	%eax, %eax
    7bf4: 66 66 66 2e 0f 1f 84 00 00 00 00 00  	nopw	%cs:(%rax,%rax)
    7c00: 88 84 05 90 fa ff ff         	movb	%al, -0x570(%rbp,%rax)
    7c07: 48 89 c1                     	movq	%rax, %rcx
    7c0a: 48 83 c9 01                  	orq	$0x1, %rcx
    7c0e: 88 8c 05 91 fa ff ff         	movb	%cl, -0x56f(%rbp,%rax)
    7c15: 48 89 c1                     	movq	%rax, %rcx
    7c18: 48 83 c9 02                  	orq	$0x2, %rcx
    7c1c: 88 8c 05 92 fa ff ff         	movb	%cl, -0x56e(%rbp,%rax)
    7c23: 48 89 c1                     	movq	%rax, %rcx
    7c26: 48 83 c9 03                  	orq	$0x3, %rcx
    7c2a: 88 8c 05 93 fa ff ff         	movb	%cl, -0x56d(%rbp,%rax)
    7c31: 48 89 c1                     	movq	%rax, %rcx
    7c34: 48 83 c9 04                  	orq	$0x4, %rcx
    7c38: 88 8c 05 94 fa ff ff         	movb	%cl, -0x56c(%rbp,%rax)
    7c3f: 48 89 c1                     	movq	%rax, %rcx
    7c42: 48 83 c9 05                  	orq	$0x5, %rcx
    7c46: 88 8c 05 95 fa ff ff         	movb	%cl, -0x56b(%rbp,%rax)
    7c4d: 48 89 c1                     	movq	%rax, %rcx
    7c50: 48 83 c9 06                  	orq	$0x6, %rcx
    7c54: 88 8c 05 96 fa ff ff         	movb	%cl, -0x56a(%rbp,%rax)
    7c5b: 48 89 c1                     	movq	%rax, %rcx
    7c5e: 48 83 c9 07                  	orq	$0x7, %rcx
    7c62: 88 8c 05 97 fa ff ff         	movb	%cl, -0x569(%rbp,%rax)
    7c69: 48 83 c0 08                  	addq	$0x8, %rax
    7c6d: 48 3d 00 01 00 00            	cmpq	$0x100, %rax            ## imm = 0x100
    7c73: 75 8b                        	jne	0x7c00 <__ZL16nmxCopyWithGammaP9IOServicePVjPVKjjjjjPy+0x70>
    7c75: 31 c0                        	xorl	%eax, %eax
    7c77: 66 0f 1f 84 00 00 00 00 00   	nopw	(%rax,%rax)
    7c80: 88 84 05 90 fb ff ff         	movb	%al, -0x470(%rbp,%rax)
    7c87: 8d 48 01                     	leal	0x1(%rax), %ecx
    7c8a: 88 8c 05 91 fb ff ff         	movb	%cl, -0x46f(%rbp,%rax)
    7c91: 8d 48 02                     	leal	0x2(%rax), %ecx
    7c94: 88 8c 05 92 fb ff ff         	movb	%cl, -0x46e(%rbp,%rax)
    7c9b: 8d 48 03                     	leal	0x3(%rax), %ecx
    7c9e: 88 8c 05 93 fb ff ff         	movb	%cl, -0x46d(%rbp,%rax)
    7ca5: 48 83 c0 04                  	addq	$0x4, %rax
    7ca9: 48 3d 00 01 00 00            	cmpq	$0x100, %rax            ## imm = 0x100
    7caf: 75 cf                        	jne	0x7c80 <__ZL16nmxCopyWithGammaP9IOServicePVjPVKjjjjjPy+0xf0>
    7cb1: 31 c0                        	xorl	%eax, %eax
    7cb3: 66 66 66 66 2e 0f 1f 84 00 00 00 00 00       	nopw	%cs:(%rax,%rax)
    7cc0: 88 84 05 90 fc ff ff         	movb	%al, -0x370(%rbp,%rax)
    7cc7: 8d 48 01                     	leal	0x1(%rax), %ecx
    7cca: 88 8c 05 91 fc ff ff         	movb	%cl, -0x36f(%rbp,%rax)
    7cd1: 8d 48 02                     	leal	0x2(%rax), %ecx
    7cd4: 88 8c 05 92 fc ff ff         	movb	%cl, -0x36e(%rbp,%rax)
    7cdb: 8d 48 03                     	leal	0x3(%rax), %ecx
    7cde: 88 8c 05 93 fc ff ff         	movb	%cl, -0x36d(%rbp,%rax)
    7ce5: 48 83 c0 04                  	addq	$0x4, %rax
    7ce9: 48 3d 00 01 00 00            	cmpq	$0x100, %rax            ## imm = 0x100
    7cef: 75 cf                        	jne	0x7cc0 <__ZL16nmxCopyWithGammaP9IOServicePVjPVKjjjjjPy+0x130>
    7cf1: 4d 85 ed                     	testq	%r13, %r13
    7cf4: 74 44                        	je	0x7d3a <__ZL16nmxCopyWithGammaP9IOServicePVjPVKjjjjjPy+0x1aa>
    7cf6: 48 8d 3d 0b c2 00 00         	leaq	0xc20b(%rip), %rdi      ## 0x13f08 <_thread_call_enter_delayed+0x13f08>
    7cfd: e8 00 00 00 00               	callq	0x7d02 <__ZL16nmxCopyWithGammaP9IOServicePVjPVKjjjjjPy+0x172>
    7d02: 48 85 c0                     	testq	%rax, %rax
    7d05: 74 33                        	je	0x7d3a <__ZL16nmxCopyWithGammaP9IOServicePVjPVKjjjjjPy+0x1aa>
    7d07: 49 89 c7                     	movq	%rax, %r15
    7d0a: 49 8b 45 00                  	movq	(%r13), %rax
    7d0e: 48 c7 04 24 00 00 00 00      	movq	$0x0, (%rsp)
    7d16: 48 8d 8d 80 fa ff ff         	leaq	-0x580(%rbp), %rcx
    7d1d: 4c 89 ef                     	movq	%r13, %rdi
    7d20: 4c 89 fe                     	movq	%r15, %rsi
    7d23: 31 d2                        	xorl	%edx, %edx
    7d25: 45 31 c0                     	xorl	%r8d, %r8d
    7d28: 45 31 c9                     	xorl	%r9d, %r9d
    7d2b: ff 90 b8 06 00 00            	callq	*0x6b8(%rax)
    7d31: 49 8b 07                     	movq	(%r15), %rax
    7d34: 4c 89 ff                     	movq	%r15, %rdi
    7d37: ff 50 28                     	callq	*0x28(%rax)
    7d3a: 48 8b 4d 18                  	movq	0x18(%rbp), %rcx
    7d3e: 48 85 c9                     	testq	%rcx, %rcx
    7d41: 74 0a                        	je	0x7d4d <__ZL16nmxCopyWithGammaP9IOServicePVjPVKjjjjjPy+0x1bd>
    7d43: 48 8b 85 88 fa ff ff         	movq	-0x578(%rbp), %rax
    7d4a: 48 89 01                     	movq	%rax, (%rcx)
    7d4d: 8b 85 84 fa ff ff            	movl	-0x57c(%rbp), %eax
    7d53: 85 c0                        	testl	%eax, %eax
    7d55: 0f 94 c1                     	sete	%cl
    7d58: 49 39 de                     	cmpq	%rbx, %r14
    7d5b: 0f 94 c2                     	sete	%dl
    7d5e: 08 ca                        	orb	%cl, %dl
    7d60: 0f 84 e8 00 00 00            	je	0x7e4e <__ZL16nmxCopyWithGammaP9IOServicePVjPVKjjjjjPy+0x2be>
    7d66: 85 c0                        	testl	%eax, %eax
    7d68: 74 11                        	je	0x7d7b <__ZL16nmxCopyWithGammaP9IOServicePVjPVKjjjjjPy+0x1eb>
    7d6a: 48 8d 35 77 9a 01 00         	leaq	0x19a77(%rip), %rsi     ## 0x217e8 <__ZL16gNmxGammaRefused>
    7d71: bf 01 00 00 00               	movl	$0x1, %edi
    7d76: e8 00 00 00 00               	callq	0x7d7b <__ZL16nmxCopyWithGammaP9IOServicePVjPVKjjjjjPy+0x1eb>
    7d7b: 8b 75 10                     	movl	0x10(%rbp), %esi
    7d7e: 44 39 e6                     	cmpl	%r12d, %esi
    7d81: 44 89 e1                     	movl	%r12d, %ecx
    7d84: 0f 42 ce                     	cmovbl	%esi, %ecx
    7d87: 83 e1 fc                     	andl	$-0x4, %ecx
    7d8a: 45 31 ff                     	xorl	%r15d, %r15d
    7d8d: 39 f1                        	cmpl	%esi, %ecx
    7d8f: 0f 87 0f 03 00 00            	ja	0x80a4 <__ZL16nmxCopyWithGammaP9IOServicePVjPVKjjjjjPy+0x514>
    7d95: 4d 85 f6                     	testq	%r14, %r14
    7d98: 0f 84 06 03 00 00            	je	0x80a4 <__ZL16nmxCopyWithGammaP9IOServicePVjPVKjjjjjPy+0x514>
    7d9e: 48 85 db                     	testq	%rbx, %rbx
    7da1: 0f 84 fd 02 00 00            	je	0x80a4 <__ZL16nmxCopyWithGammaP9IOServicePVjPVKjjjjjPy+0x514>
    7da7: 8b bd c0 fd ff ff            	movl	-0x240(%rbp), %edi
    7dad: 85 ff                        	testl	%edi, %edi
    7daf: 0f 84 ef 02 00 00            	je	0x80a4 <__ZL16nmxCopyWithGammaP9IOServicePVjPVKjjjjjPy+0x514>
    7db5: 8d 41 ff                     	leal	-0x1(%rcx), %eax
    7db8: 44 39 e0                     	cmpl	%r12d, %eax
    7dbb: 0f 83 e3 02 00 00            	jae	0x80a4 <__ZL16nmxCopyWithGammaP9IOServicePVjPVKjjjjjPy+0x514>
    7dc1: 49 39 de                     	cmpq	%rbx, %r14
    7dc4: 0f 94 c0                     	sete	%al
    7dc7: 41 39 f4                     	cmpl	%esi, %r12d
    7dca: 0f 94 c2                     	sete	%dl
    7dcd: 41 b7 01                     	movb	$0x1, %r15b
    7dd0: 84 d0                        	testb	%dl, %al
    7dd2: 0f 85 cc 02 00 00            	jne	0x80a4 <__ZL16nmxCopyWithGammaP9IOServicePVjPVKjjjjjPy+0x514>
    7dd8: 89 fa                        	movl	%edi, %edx
    7dda: 45 89 e0                     	movl	%r12d, %r8d
    7ddd: 41 89 f1                     	movl	%esi, %r9d
    7de0: 41 31 cc                     	xorl	%ecx, %r12d
    7de3: 31 ce                        	xorl	%ecx, %esi
    7de5: 44 09 e6                     	orl	%r12d, %esi
    7de8: 41 ba 01 00 00 00            	movl	$0x1, %r10d
    7dee: b8 01 00 00 00               	movl	$0x1, %eax
    7df3: 48 0f 44 c2                  	cmoveq	%rdx, %rax
    7df7: 49 0f 44 d2                  	cmoveq	%r10, %rdx
    7dfb: 48 0f af c1                  	imulq	%rcx, %rax
    7dff: 49 89 d3                     	movq	%rdx, %r11
    7e02: 49 f7 db                     	negq	%r11
    7e05: 66 66 2e 0f 1f 84 00 00 00 00 00     	nopw	%cs:(%rax,%rax)
    7e10: 48 89 c1                     	movq	%rax, %rcx
    7e13: 48 89 de                     	movq	%rbx, %rsi
    7e16: 4c 89 f7                     	movq	%r14, %rdi
    7e19: f3 a4                        	rep		movsb	(%rsi), %es:(%rdi)
    7e1b: 49 39 d2                     	cmpq	%rdx, %r10
    7e1e: b9 00 00 00 00               	movl	$0x0, %ecx
    7e23: 49 0f 42 c8                  	cmovbq	%r8, %rcx
    7e27: be 00 00 00 00               	movl	$0x0, %esi
    7e2c: 49 0f 42 f1                  	cmovbq	%r9, %rsi
    7e30: 49 01 ce                     	addq	%rcx, %r14
    7e33: 48 01 f3                     	addq	%rsi, %rbx
    7e36: 4b 8d 0c 13                  	leaq	(%r11,%r10), %rcx
    7e3a: 48 ff c1                     	incq	%rcx
    7e3d: 49 ff c2                     	incq	%r10
    7e40: 48 83 f9 01                  	cmpq	$0x1, %rcx
    7e44: 75 ca                        	jne	0x7e10 <__ZL16nmxCopyWithGammaP9IOServicePVjPVKjjjjjPy+0x280>
    7e46: 0f ae f8                     	sfence
    7e49: e9 56 02 00 00               	jmp	0x80a4 <__ZL16nmxCopyWithGammaP9IOServicePVjPVKjjjjjPy+0x514>
    7e4e: e8 00 00 00 00               	callq	0x7e53 <__ZL16nmxCopyWithGammaP9IOServicePVjPVKjjjjjPy+0x2c3>
    7e53: 49 89 c5                     	movq	%rax, %r13
    7e56: 48 c7 85 b8 fd ff ff 00 00 00 00     	movq	$0x0, -0x248(%rbp)
    7e61: 45 31 ff                     	xorl	%r15d, %r15d
    7e64: 41 f6 c6 03                  	testb	$0x3, %r14b
    7e68: 0f 85 ed 01 00 00            	jne	0x805b <__ZL16nmxCopyWithGammaP9IOServicePVjPVKjjjjjPy+0x4cb>
    7e6e: 4d 85 f6                     	testq	%r14, %r14
    7e71: 0f 84 e4 01 00 00            	je	0x805b <__ZL16nmxCopyWithGammaP9IOServicePVjPVKjjjjjPy+0x4cb>
    7e77: 48 85 db                     	testq	%rbx, %rbx
    7e7a: 0f 84 db 01 00 00            	je	0x805b <__ZL16nmxCopyWithGammaP9IOServicePVjPVKjjjjjPy+0x4cb>
    7e80: 8b 8d c0 fd ff ff            	movl	-0x240(%rbp), %ecx
    7e86: 83 bd 80 fa ff ff 01         	cmpl	$0x1, -0x580(%rbp)
    7e8d: 8b 55 10                     	movl	0x10(%rbp), %edx
    7e90: 8b bd c8 fd ff ff            	movl	-0x238(%rbp), %edi
    7e96: 0f 85 bf 01 00 00            	jne	0x805b <__ZL16nmxCopyWithGammaP9IOServicePVjPVKjjjjjPy+0x4cb>
    7e9c: 45 31 ff                     	xorl	%r15d, %r15d
    7e9f: 85 c9                        	testl	%ecx, %ecx
    7ea1: 0f 84 b4 01 00 00            	je	0x805b <__ZL16nmxCopyWithGammaP9IOServicePVjPVKjjjjjPy+0x4cb>
    7ea7: 85 ff                        	testl	%edi, %edi
    7ea9: 0f 84 ac 01 00 00            	je	0x805b <__ZL16nmxCopyWithGammaP9IOServicePVjPVKjjjjjPy+0x4cb>
    7eaf: 89 d8                        	movl	%ebx, %eax
    7eb1: 83 e0 03                     	andl	$0x3, %eax
    7eb4: 0f 85 a1 01 00 00            	jne	0x805b <__ZL16nmxCopyWithGammaP9IOServicePVjPVKjjjjjPy+0x4cb>
    7eba: 44 89 e6                     	movl	%r12d, %esi
    7ebd: 41 89 fc                     	movl	%edi, %r12d
    7ec0: 89 f6                        	movl	%esi, %esi
    7ec2: 4a 8d 04 a5 00 00 00 00      	leaq	(,%r12,4), %rax
    7eca: 45 31 ff                     	xorl	%r15d, %r15d
    7ecd: 48 89 b5 c8 fd ff ff         	movq	%rsi, -0x238(%rbp)
    7ed4: 48 39 f0                     	cmpq	%rsi, %rax
    7ed7: 0f 87 7e 01 00 00            	ja	0x805b <__ZL16nmxCopyWithGammaP9IOServicePVjPVKjjjjjPy+0x4cb>
    7edd: 89 d2                        	movl	%edx, %edx
    7edf: 48 39 d0                     	cmpq	%rdx, %rax
    7ee2: 0f 87 73 01 00 00            	ja	0x805b <__ZL16nmxCopyWithGammaP9IOServicePVjPVKjjjjjPy+0x4cb>
    7ee8: 89 d0                        	movl	%edx, %eax
    7eea: 0b 85 c8 fd ff ff            	orl	-0x238(%rbp), %eax
    7ef0: 83 e0 03                     	andl	$0x3, %eax
    7ef3: 0f 85 62 01 00 00            	jne	0x805b <__ZL16nmxCopyWithGammaP9IOServicePVjPVKjjjjjPy+0x4cb>
    7ef9: 4c 89 ad b0 fd ff ff         	movq	%r13, -0x250(%rbp)
    7f00: 89 c8                        	movl	%ecx, %eax
    7f02: 48 89 85 a8 fd ff ff         	movq	%rax, -0x258(%rbp)
    7f09: 31 c0                        	xorl	%eax, %eax
    7f0b: 4c 8d bd d0 fd ff ff         	leaq	-0x230(%rbp), %r15
    7f12: 48 89 9d 90 fd ff ff         	movq	%rbx, -0x270(%rbp)
    7f19: 4c 89 b5 98 fd ff ff         	movq	%r14, -0x268(%rbp)
    7f20: 48 89 95 a0 fd ff ff         	movq	%rdx, -0x260(%rbp)
    7f27: eb 33                        	jmp	0x7f5c <__ZL16nmxCopyWithGammaP9IOServicePVjPVKjjjjjPy+0x3cc>
    7f29: 0f 1f 80 00 00 00 00         	nopl	(%rax)
    7f30: 48 8b 85 c0 fd ff ff         	movq	-0x240(%rbp), %rax
    7f37: 48 ff c0                     	incq	%rax
    7f3a: 48 3b 85 a8 fd ff ff         	cmpq	-0x258(%rbp), %rax
    7f41: 48 8b 9d 90 fd ff ff         	movq	-0x270(%rbp), %rbx
    7f48: 4c 8b b5 98 fd ff ff         	movq	-0x268(%rbp), %r14
    7f4f: 48 8b 95 a0 fd ff ff         	movq	-0x260(%rbp), %rdx
    7f56: 0f 84 f2 00 00 00            	je	0x804e <__ZL16nmxCopyWithGammaP9IOServicePVjPVKjjjjjPy+0x4be>
    7f5c: 49 89 c5                     	movq	%rax, %r13
    7f5f: 4c 0f af ad c8 fd ff ff      	imulq	-0x238(%rbp), %r13
    7f67: 4d 01 f5                     	addq	%r14, %r13
    7f6a: 48 89 85 c0 fd ff ff         	movq	%rax, -0x240(%rbp)
    7f71: 49 89 c6                     	movq	%rax, %r14
    7f74: 4c 0f af f2                  	imulq	%rdx, %r14
    7f78: 49 01 de                     	addq	%rbx, %r14
    7f7b: be 00 02 00 00               	movl	$0x200, %esi            ## imm = 0x200
    7f80: 4c 89 ff                     	movq	%r15, %rdi
    7f83: e8 00 00 00 00               	callq	0x7f88 <__ZL16nmxCopyWithGammaP9IOServicePVjPVKjjjjjPy+0x3f8>
    7f88: 41 bb 80 00 00 00            	movl	$0x80, %r11d
    7f8e: 31 d2                        	xorl	%edx, %edx
    7f90: eb 2d                        	jmp	0x7fbf <__ZL16nmxCopyWithGammaP9IOServicePVjPVKjjjjjPy+0x42f>
    7f92: 66 66 66 66 66 2e 0f 1f 84 00 00 00 00 00    	nopw	%cs:(%rax,%rax)
    7fa0: 48 8d 3c 95 00 00 00 00      	leaq	(,%rdx,4), %rdi
    7fa8: 4c 01 ef                     	addq	%r13, %rdi
    7fab: 48 89 c1                     	movq	%rax, %rcx
    7fae: 4c 89 fe                     	movq	%r15, %rsi
    7fb1: f3 a4                        	rep		movsb	(%rsi), %es:(%rdi)
    7fb3: 4c 01 c2                     	addq	%r8, %rdx
    7fb6: 4c 39 e2                     	cmpq	%r12, %rdx
    7fb9: 0f 83 71 ff ff ff            	jae	0x7f30 <__ZL16nmxCopyWithGammaP9IOServicePVjPVKjjjjjPy+0x3a0>
    7fbf: 4d 89 e0                     	movq	%r12, %r8
    7fc2: 49 29 d0                     	subq	%rdx, %r8
    7fc5: 49 81 f8 80 00 00 00         	cmpq	$0x80, %r8
    7fcc: 4d 0f 43 c3                  	cmovaeq	%r11, %r8
    7fd0: 49 8d 34 96                  	leaq	(%r14,%rdx,4), %rsi
    7fd4: 42 8d 04 85 00 00 00 00      	leal	(,%r8,4), %eax
    7fdc: 48 89 c1                     	movq	%rax, %rcx
    7fdf: 4c 89 ff                     	movq	%r15, %rdi
    7fe2: f3 a4                        	rep		movsb	(%rsi), %es:(%rdi)
    7fe4: 49 39 d4                     	cmpq	%rdx, %r12
    7fe7: 74 b7                        	je	0x7fa0 <__ZL16nmxCopyWithGammaP9IOServicePVjPVKjjjjjPy+0x410>
    7fe9: 4c 89 c1                     	movq	%r8, %rcx
    7fec: 31 f6                        	xorl	%esi, %esi
    7fee: 66 90                        	nop
    7ff0: 8b 9c b5 d0 fd ff ff         	movl	-0x230(%rbp,%rsi,4), %ebx
    7ff7: 41 89 d9                     	movl	%ebx, %r9d
    7ffa: 0f b6 ff                     	movzbl	%bh, %edi
    7ffd: 44 0f b6 d3                  	movzbl	%bl, %r10d
    8001: 81 e3 00 00 00 ff            	andl	$0xff000000, %ebx       ## imm = 0xFF000000
    8007: 41 c1 e9 10                  	shrl	$0x10, %r9d
    800b: 45 0f b6 c9                  	movzbl	%r9b, %r9d
    800f: 46 0f b6 8c 0d 90 fa ff ff   	movzbl	-0x570(%rbp,%r9), %r9d
    8018: 41 c1 e1 10                  	shll	$0x10, %r9d
    801c: 41 09 d9                     	orl	%ebx, %r9d
    801f: 0f b6 bc 3d 90 fb ff ff      	movzbl	-0x470(%rbp,%rdi), %edi
    8027: c1 e7 08                     	shll	$0x8, %edi
    802a: 44 09 cf                     	orl	%r9d, %edi
    802d: 46 0f b6 8c 15 90 fc ff ff   	movzbl	-0x370(%rbp,%r10), %r9d
    8036: 41 09 f9                     	orl	%edi, %r9d
    8039: 44 89 8c b5 d0 fd ff ff      	movl	%r9d, -0x230(%rbp,%rsi,4)
    8041: 48 ff c6                     	incq	%rsi
    8044: 48 ff c9                     	decq	%rcx
    8047: 75 a7                        	jne	0x7ff0 <__ZL16nmxCopyWithGammaP9IOServicePVjPVKjjjjjPy+0x460>
    8049: e9 52 ff ff ff               	jmp	0x7fa0 <__ZL16nmxCopyWithGammaP9IOServicePVjPVKjjjjjPy+0x410>
    804e: 0f ae f8                     	sfence
    8051: 41 b7 01                     	movb	$0x1, %r15b
    8054: 4c 8b ad b0 fd ff ff         	movq	-0x250(%rbp), %r13
    805b: e8 00 00 00 00               	callq	0x8060 <__ZL16nmxCopyWithGammaP9IOServicePVjPVKjjjjjPy+0x4d0>
    8060: 4c 29 e8                     	subq	%r13, %rax
    8063: 48 8d b5 b8 fd ff ff         	leaq	-0x248(%rbp), %rsi
    806a: 48 89 c7                     	movq	%rax, %rdi
    806d: e8 00 00 00 00               	callq	0x8072 <__ZL16nmxCopyWithGammaP9IOServicePVjPVKjjjjjPy+0x4e2>
    8072: 48 8b bd b8 fd ff ff         	movq	-0x248(%rbp), %rdi
    8079: 48 8d 35 58 97 01 00         	leaq	0x19758(%rip), %rsi     ## 0x217d8 <__ZL11gNmxGammaNS>
    8080: e8 00 00 00 00               	callq	0x8085 <__ZL16nmxCopyWithGammaP9IOServicePVjPVKjjjjjPy+0x4f5>
    8085: 48 8d 05 54 97 01 00         	leaq	0x19754(%rip), %rax     ## 0x217e0 <__ZL15gNmxGammaCopies>
    808c: 48 8d 35 55 97 01 00         	leaq	0x19755(%rip), %rsi     ## 0x217e8 <__ZL16gNmxGammaRefused>
    8093: 45 84 ff                     	testb	%r15b, %r15b
    8096: 48 0f 45 f0                  	cmovneq	%rax, %rsi
    809a: bf 01 00 00 00               	movl	$0x1, %edi
    809f: e8 00 00 00 00               	callq	0x80a4 <__ZL16nmxCopyWithGammaP9IOServicePVjPVKjjjjjPy+0x514>
    80a4: 48 8b 05 1d 92 01 00         	movq	0x1921d(%rip), %rax     ## 0x212c8 <_thread_call_enter_delayed+0x212c8>
    80ab: 48 8b 00                     	movq	(%rax), %rax
    80ae: 48 3b 45 d0                  	cmpq	-0x30(%rbp), %rax
    80b2: 75 15                        	jne	0x80c9 <__ZL16nmxCopyWithGammaP9IOServicePVjPVKjjjjjPy+0x539>
    80b4: 44 89 f8                     	movl	%r15d, %eax
    80b7: 48 81 c4 68 05 00 00         	addq	$0x568, %rsp            ## imm = 0x568
    80be: 5b                           	popq	%rbx
    80bf: 41 5c                        	popq	%r12
    80c1: 41 5d                        	popq	%r13
    80c3: 41 5e                        	popq	%r14
    80c5: 41 5f                        	popq	%r15
    80c7: 5d                           	popq	%rbp
    80c8: c3                           	retq
    80c9: e8 00 00 00 00               	callq	0x80ce <__ZL16nmxCopyWithGammaP9IOServicePVjPVKjjjjjPy+0x53e>
    80ce: 66 90                        	nop
