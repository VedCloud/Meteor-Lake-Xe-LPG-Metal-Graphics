ffffff8001407110 <__ZN23AppleACPIPlatformExpert14evaluateObjectEP20IOACPIPlatformDevicePK8OSSymbolPP8OSObjectS7_jj>:
ffffff8001407110: 55                   	pushq	%rbp
ffffff8001407111: 48 89 e5             	movq	%rsp, %rbp
ffffff8001407114: 41 57                	pushq	%r15
ffffff8001407116: 41 56                	pushq	%r14
ffffff8001407118: 41 55                	pushq	%r13
ffffff800140711a: 41 54                	pushq	%r12
ffffff800140711c: 53                   	pushq	%rbx
ffffff800140711d: 48 83 ec 58          	subq	$0x58, %rsp
ffffff8001407121: 45 89 cd             	movl	%r9d, %r13d
ffffff8001407124: 4c 89 c3             	movq	%r8, %rbx
ffffff8001407127: 49 89 ce             	movq	%rcx, %r14
ffffff800140712a: 49 89 f7             	movq	%rsi, %r15
ffffff800140712d: 48 c7 45 b0 00 00 00 00      	movq	$0x0, -0x50(%rbp)
ffffff8001407135: 48 85 c9             	testq	%rcx, %rcx
ffffff8001407138: 74 07                	je	0xffffff8001407141 <__ZN23AppleACPIPlatformExpert14evaluateObjectEP20IOACPIPlatformDevicePK8OSSymbolPP8OSObjectS7_jj+0x31>
ffffff800140713a: 49 c7 06 00 00 00 00 	movq	$0x0, (%r14)
ffffff8001407141: 41 bc c0 02 00 e0    	movl	$0xe00002c0, %r12d      ## imm = 0xE00002C0
ffffff8001407147: 4d 85 ff             	testq	%r15, %r15
ffffff800140714a: 74 39                	je	0xffffff8001407185 <__ZN23AppleACPIPlatformExpert14evaluateObjectEP20IOACPIPlatformDevicePK8OSSymbolPP8OSObjectS7_jj+0x75>
ffffff800140714c: 48 89 55 d0          	movq	%rdx, -0x30(%rbp)
ffffff8001407150: 49 8b 07             	movq	(%r15), %rax
ffffff8001407153: 4c 89 ff             	movq	%r15, %rdi
ffffff8001407156: ff 90 88 08 00 00    	callq	*0x888(%rax)
ffffff800140715c: 48 85 c0             	testq	%rax, %rax
ffffff800140715f: 74 24                	je	0xffffff8001407185 <__ZN23AppleACPIPlatformExpert14evaluateObjectEP20IOACPIPlatformDevicePK8OSSymbolPP8OSObjectS7_jj+0x75>
ffffff8001407161: 31 c9                	xorl	%ecx, %ecx
ffffff8001407163: 48 89 4d 90          	movq	%rcx, -0x70(%rbp)
ffffff8001407167: 48 89 4d 88          	movq	%rcx, -0x78(%rbp)
ffffff800140716b: 48 89 4d a0          	movq	%rcx, -0x60(%rbp)
ffffff800140716f: 48 89 4d a8          	movq	%rcx, -0x58(%rbp)
ffffff8001407173: 48 89 4d b8          	movq	%rcx, -0x48(%rbp)
ffffff8001407177: 81 38 49 70 43 61    	cmpl	$0x61437049, (%rax)     ## imm = 0x61437049
ffffff800140717d: 75 06                	jne	0xffffff8001407185 <__ZN23AppleACPIPlatformExpert14evaluateObjectEP20IOACPIPlatformDevicePK8OSSymbolPP8OSObjectS7_jj+0x75>
ffffff800140717f: 4c 39 78 08          	cmpq	%r15, 0x8(%rax)
ffffff8001407183: 74 12                	je	0xffffff8001407197 <__ZN23AppleACPIPlatformExpert14evaluateObjectEP20IOACPIPlatformDevicePK8OSSymbolPP8OSObjectS7_jj+0x87>
ffffff8001407185: 44 89 e0             	movl	%r12d, %eax
ffffff8001407188: 48 83 c4 58          	addq	$0x58, %rsp
ffffff800140718c: 5b                   	popq	%rbx
ffffff800140718d: 41 5c                	popq	%r12
ffffff800140718f: 41 5d                	popq	%r13
ffffff8001407191: 41 5e                	popq	%r14
ffffff8001407193: 41 5f                	popq	%r15
ffffff8001407195: 5d                   	popq	%rbp
ffffff8001407196: c3                   	retq
ffffff8001407197: 48 8b 40 10          	movq	0x10(%rax), %rax
ffffff800140719b: 48 89 45 98          	movq	%rax, -0x68(%rbp)
ffffff800140719f: 48 85 c0             	testq	%rax, %rax
ffffff80014071a2: 74 e1                	je	0xffffff8001407185 <__ZN23AppleACPIPlatformExpert14evaluateObjectEP20IOACPIPlatformDevicePK8OSSymbolPP8OSObjectS7_jj+0x75>
ffffff80014071a4: 41 bc c2 02 00 e0    	movl	$0xe00002c2, %r12d      ## imm = 0xE00002C2
ffffff80014071aa: 48 8b 7d d0          	movq	-0x30(%rbp), %rdi
ffffff80014071ae: 48 85 ff             	testq	%rdi, %rdi
ffffff80014071b1: 74 d2                	je	0xffffff8001407185 <__ZN23AppleACPIPlatformExpert14evaluateObjectEP20IOACPIPlatformDevicePK8OSSymbolPP8OSObjectS7_jj+0x75>
ffffff80014071b3: 48 8b 07             	movq	(%rdi), %rax
ffffff80014071b6: ff 90 30 01 00 00    	callq	*0x130(%rax)
ffffff80014071bc: 83 f8 04             	cmpl	$0x4, %eax
ffffff80014071bf: 77 c4                	ja	0xffffff8001407185 <__ZN23AppleACPIPlatformExpert14evaluateObjectEP20IOACPIPlatformDevicePK8OSSymbolPP8OSObjectS7_jj+0x75>
ffffff80014071c1: f6 05 49 d3 07 00 80 	testb	$-0x80, 0x7d349(%rip)   ## 0xffffff8001484511 <_gAppleACPIPlatformFlags+0x1>
ffffff80014071c8: 75 0d                	jne	0xffffff80014071d7 <__ZN23AppleACPIPlatformExpert14evaluateObjectEP20IOACPIPlatformDevicePK8OSSymbolPP8OSObjectS7_jj+0xc7>
ffffff80014071ca: 48 c7 45 c8 00 00 00 00      	movq	$0x0, -0x38(%rbp)
ffffff80014071d2: e9 83 00 00 00       	jmp	0xffffff800140725a <__ZN23AppleACPIPlatformExpert14evaluateObjectEP20IOACPIPlatformDevicePK8OSSymbolPP8OSObjectS7_jj+0x14a>
ffffff80014071d7: 4c 89 ff             	movq	%r15, %rdi
ffffff80014071da: e8 51 bd 6b ff       	callq	0xffffff8000ac2f30 <_uuid_parse+0xffffff8000ac2f30>
ffffff80014071df: 48 89 45 c8          	movq	%rax, -0x38(%rbp)
ffffff80014071e3: 4c 8d 7d b8          	leaq	-0x48(%rbp), %r15
ffffff80014071e7: 49 c7 07 00 00 00 00 	movq	$0x0, (%r15)
ffffff80014071ee: 48 8b 7d d0          	movq	-0x30(%rbp), %rdi
ffffff80014071f2: 48 8b 07             	movq	(%rdi), %rax
ffffff80014071f5: ff 90 48 01 00 00    	callq	*0x148(%rax)
ffffff80014071fb: ba 08 00 00 00       	movl	$0x8, %edx
ffffff8001407200: 4c 89 ff             	movq	%r15, %rdi
ffffff8001407203: 48 89 c6             	movq	%rax, %rsi
ffffff8001407206: e8 45 d1 0e ff       	callq	0xffffff80004f4350 <_uuid_parse+0xffffff80004f4350>
ffffff800140720b: 49 c7 c7 ff ff ff ff 	movq	$-0x1, %r15
ffffff8001407212: 45 85 ed             	testl	%r13d, %r13d
ffffff8001407215: 74 14                	je	0xffffff800140722b <__ZN23AppleACPIPlatformExpert14evaluateObjectEP20IOACPIPlatformDevicePK8OSSymbolPP8OSObjectS7_jj+0x11b>
ffffff8001407217: 48 8b 3b             	movq	(%rbx), %rdi
ffffff800140721a: 48 85 ff             	testq	%rdi, %rdi
ffffff800140721d: 74 0c                	je	0xffffff800140722b <__ZN23AppleACPIPlatformExpert14evaluateObjectEP20IOACPIPlatformDevicePK8OSSymbolPP8OSObjectS7_jj+0x11b>
ffffff800140721f: 48 8b 07             	movq	(%rdi), %rax
ffffff8001407222: ff 90 50 01 00 00    	callq	*0x150(%rax)
ffffff8001407228: 49 89 c7             	movq	%rax, %r15
ffffff800140722b: 48 8b 05 ce 1f 07 00 	movq	0x71fce(%rip), %rax     ## 0xffffff8001479200 <_uuid_parse+0xffffff8001479200>
ffffff8001407232: f7 00 f7 ff ff ff    	testl	$0xfffffff7, (%rax)     ## imm = 0xFFFFFFF7
ffffff8001407238: 74 20                	je	0xffffff800140725a <__ZN23AppleACPIPlatformExpert14evaluateObjectEP20IOACPIPlatformDevicePK8OSSymbolPP8OSObjectS7_jj+0x14a>
ffffff800140723a: e8 51 e9 12 ff       	callq	0xffffff8000535b90 <_uuid_parse+0xffffff8000535b90>
ffffff800140723f: 48 8b 4d b8          	movq	-0x48(%rbp), %rcx
ffffff8001407243: bf 11 00 17 06       	movl	$0x6170011, %edi        ## imm = 0x6170011
ffffff8001407248: 48 89 c6             	movq	%rax, %rsi
ffffff800140724b: 48 8b 55 c8          	movq	-0x38(%rbp), %rdx
ffffff800140724f: 4d 89 f8             	movq	%r15, %r8
ffffff8001407252: 45 31 c9             	xorl	%r9d, %r9d
ffffff8001407255: e8 16 15 4b ff       	callq	0xffffff80008b8770 <_uuid_parse+0xffffff80008b8770>
ffffff800140725a: 48 8b 7d d0          	movq	-0x30(%rbp), %rdi
ffffff800140725e: 45 85 ed             	testl	%r13d, %r13d
ffffff8001407261: 74 70                	je	0xffffff80014072d3 <__ZN23AppleACPIPlatformExpert14evaluateObjectEP20IOACPIPlatformDevicePK8OSSymbolPP8OSObjectS7_jj+0x1c3>
ffffff8001407263: 42 8d 04 ed 00 00 00 00      	leal	(,%r13,8), %eax
ffffff800140726b: 8d 3c 40             	leal	(%rax,%rax,2), %edi
ffffff800140726e: e8 1d 9d 64 ff       	callq	0xffffff8000a50f90 <_uuid_parse+0xffffff8000a50f90>
ffffff8001407273: 48 89 45 c0          	movq	%rax, -0x40(%rbp)
ffffff8001407277: 48 85 c0             	testq	%rax, %rax
ffffff800140727a: 74 76                	je	0xffffff80014072f2 <__ZN23AppleACPIPlatformExpert14evaluateObjectEP20IOACPIPlatformDevicePK8OSSymbolPP8OSObjectS7_jj+0x1e2>
ffffff800140727c: 45 89 ec             	movl	%r13d, %r12d
ffffff800140727f: 41 bf 01 00 00 00    	movl	$0x1, %r15d
ffffff8001407285: 4a 8b 74 fb f8       	movq	-0x8(%rbx,%r15,8), %rsi
ffffff800140728a: 48 8b 7d c0          	movq	-0x40(%rbp), %rdi
ffffff800140728e: 48 8d 55 b0          	leaq	-0x50(%rbp), %rdx
ffffff8001407292: e8 42 01 00 00       	callq	0xffffff80014073d9 <__ZL17appendParamObjectP6OSDataP8OSObjectPP7OSArray>
ffffff8001407297: 4d 39 e7             	cmpq	%r12, %r15
ffffff800140729a: 73 07                	jae	0xffffff80014072a3 <__ZN23AppleACPIPlatformExpert14evaluateObjectEP20IOACPIPlatformDevicePK8OSSymbolPP8OSObjectS7_jj+0x193>
ffffff800140729c: 49 ff c7             	incq	%r15
ffffff800140729f: 84 c0                	testb	%al, %al
ffffff80014072a1: 75 e2                	jne	0xffffff8001407285 <__ZN23AppleACPIPlatformExpert14evaluateObjectEP20IOACPIPlatformDevicePK8OSSymbolPP8OSObjectS7_jj+0x175>
ffffff80014072a3: 41 bc c2 02 00 e0    	movl	$0xe00002c2, %r12d      ## imm = 0xE00002C2
ffffff80014072a9: 84 c0                	testb	%al, %al
ffffff80014072ab: 0f 84 c1 00 00 00    	je	0xffffff8001407372 <__ZN23AppleACPIPlatformExpert14evaluateObjectEP20IOACPIPlatformDevicePK8OSSymbolPP8OSObjectS7_jj+0x262>
ffffff80014072b1: 48 8d 5d 88          	leaq	-0x78(%rbp), %rbx
ffffff80014072b5: 44 89 2b             	movl	%r13d, (%rbx)
ffffff80014072b8: 4c 8b 6d c0          	movq	-0x40(%rbp), %r13
ffffff80014072bc: 49 8b 45 00          	movq	(%r13), %rax
ffffff80014072c0: 4c 89 ef             	movq	%r13, %rdi
ffffff80014072c3: ff 90 78 01 00 00    	callq	*0x178(%rax)
ffffff80014072c9: 48 89 43 08          	movq	%rax, 0x8(%rbx)
ffffff80014072cd: 48 8b 7d d0          	movq	-0x30(%rbp), %rdi
ffffff80014072d1: eb 05                	jmp	0xffffff80014072d8 <__ZN23AppleACPIPlatformExpert14evaluateObjectEP20IOACPIPlatformDevicePK8OSSymbolPP8OSObjectS7_jj+0x1c8>
ffffff80014072d3: 31 db                	xorl	%ebx, %ebx
ffffff80014072d5: 45 31 ed             	xorl	%r13d, %r13d
ffffff80014072d8: 4d 85 f6             	testq	%r14, %r14
ffffff80014072db: 74 20                	je	0xffffff80014072fd <__ZN23AppleACPIPlatformExpert14evaluateObjectEP20IOACPIPlatformDevicePK8OSSymbolPP8OSObjectS7_jj+0x1ed>
ffffff80014072dd: 4c 8d 7d a0          	leaq	-0x60(%rbp), %r15
ffffff80014072e1: 49 c7 07 ff ff ff ff 	movq	$-0x1, (%r15)
ffffff80014072e8: 49 c7 47 08 00 00 00 00      	movq	$0x0, 0x8(%r15)
ffffff80014072f0: eb 0e                	jmp	0xffffff8001407300 <__ZN23AppleACPIPlatformExpert14evaluateObjectEP20IOACPIPlatformDevicePK8OSSymbolPP8OSObjectS7_jj+0x1f0>
ffffff80014072f2: 41 bc bd 02 00 e0    	movl	$0xe00002bd, %r12d      ## imm = 0xE00002BD
ffffff80014072f8: 45 31 ed             	xorl	%r13d, %r13d
ffffff80014072fb: eb 79                	jmp	0xffffff8001407376 <__ZN23AppleACPIPlatformExpert14evaluateObjectEP20IOACPIPlatformDevicePK8OSSymbolPP8OSObjectS7_jj+0x266>
ffffff80014072fd: 45 31 ff             	xorl	%r15d, %r15d
ffffff8001407300: 4d 85 f6             	testq	%r14, %r14
ffffff8001407303: 41 0f 95 c4          	setne	%r12b
ffffff8001407307: 48 8b 07             	movq	(%rdi), %rax
ffffff800140730a: ff 90 48 01 00 00    	callq	*0x148(%rax)
ffffff8001407310: 48 8b 7d 98          	movq	-0x68(%rbp), %rdi
ffffff8001407314: 48 89 c6             	movq	%rax, %rsi
ffffff8001407317: 48 89 da             	movq	%rbx, %rdx
ffffff800140731a: 4c 89 f9             	movq	%r15, %rcx
ffffff800140731d: e8 97 75 04 00       	callq	0xffffff800144e8b9 <_AcpiEvaluateObject>
ffffff8001407322: 89 c3                	movl	%eax, %ebx
ffffff8001407324: 48 8b 7d a8          	movq	-0x58(%rbp), %rdi
ffffff8001407328: 48 83 7d a0 00       	cmpq	$0x0, -0x60(%rbp)
ffffff800140732d: 0f 95 c0             	setne	%al
ffffff8001407330: 44 20 e0             	andb	%r12b, %al
ffffff8001407333: 48 85 ff             	testq	%rdi, %rdi
ffffff8001407336: 0f 95 c1             	setne	%cl
ffffff8001407339: 20 c1                	andb	%al, %cl
ffffff800140733b: 80 f9 01             	cmpb	$0x1, %cl
ffffff800140733e: 75 15                	jne	0xffffff8001407355 <__ZN23AppleACPIPlatformExpert14evaluateObjectEP20IOACPIPlatformDevicePK8OSSymbolPP8OSObjectS7_jj+0x245>
ffffff8001407340: 85 db                	testl	%ebx, %ebx
ffffff8001407342: 75 0c                	jne	0xffffff8001407350 <__ZN23AppleACPIPlatformExpert14evaluateObjectEP20IOACPIPlatformDevicePK8OSSymbolPP8OSObjectS7_jj+0x240>
ffffff8001407344: e8 6c 02 00 00       	callq	0xffffff80014075b5 <__ZL23newObjectFromACPIObjectPK11acpi_object>
ffffff8001407349: 49 89 06             	movq	%rax, (%r14)
ffffff800140734c: 48 8b 7d a8          	movq	-0x58(%rbp), %rdi
ffffff8001407350: e8 34 82 01 00       	callq	0xffffff800141f589 <_AcpiOsFree>
ffffff8001407355: 81 fb 01 10 00 00    	cmpl	$0x1001, %ebx           ## imm = 0x1001
ffffff800140735b: b8 c2 02 00 e0       	movl	$0xe00002c2, %eax       ## imm = 0xE00002C2
ffffff8001407360: 41 bc bc 02 00 e0    	movl	$0xe00002bc, %r12d      ## imm = 0xE00002BC
ffffff8001407366: 44 0f 44 e0          	cmovel	%eax, %r12d
ffffff800140736a: 85 db                	testl	%ebx, %ebx
ffffff800140736c: 44 0f 44 e3          	cmovel	%ebx, %r12d
ffffff8001407370: eb 04                	jmp	0xffffff8001407376 <__ZN23AppleACPIPlatformExpert14evaluateObjectEP20IOACPIPlatformDevicePK8OSSymbolPP8OSObjectS7_jj+0x266>
ffffff8001407372: 4c 8b 6d c0          	movq	-0x40(%rbp), %r13
ffffff8001407376: 48 8b 7d b0          	movq	-0x50(%rbp), %rdi
ffffff800140737a: 48 85 ff             	testq	%rdi, %rdi
ffffff800140737d: 74 06                	je	0xffffff8001407385 <__ZN23AppleACPIPlatformExpert14evaluateObjectEP20IOACPIPlatformDevicePK8OSSymbolPP8OSObjectS7_jj+0x275>
ffffff800140737f: 48 8b 07             	movq	(%rdi), %rax
ffffff8001407382: ff 50 28             	callq	*0x28(%rax)
ffffff8001407385: 4d 85 ed             	testq	%r13, %r13
ffffff8001407388: 74 0a                	je	0xffffff8001407394 <__ZN23AppleACPIPlatformExpert14evaluateObjectEP20IOACPIPlatformDevicePK8OSSymbolPP8OSObjectS7_jj+0x284>
ffffff800140738a: 49 8b 45 00          	movq	(%r13), %rax
ffffff800140738e: 4c 89 ef             	movq	%r13, %rdi
ffffff8001407391: ff 50 28             	callq	*0x28(%rax)
ffffff8001407394: f6 05 76 d1 07 00 80 	testb	$-0x80, 0x7d176(%rip)   ## 0xffffff8001484511 <_gAppleACPIPlatformFlags+0x1>
ffffff800140739b: 0f 84 e4 fd ff ff    	je	0xffffff8001407185 <__ZN23AppleACPIPlatformExpert14evaluateObjectEP20IOACPIPlatformDevicePK8OSSymbolPP8OSObjectS7_jj+0x75>
ffffff80014073a1: 48 8b 05 58 1e 07 00 	movq	0x71e58(%rip), %rax     ## 0xffffff8001479200 <_uuid_parse+0xffffff8001479200>
ffffff80014073a8: f7 00 f7 ff ff ff    	testl	$0xfffffff7, (%rax)     ## imm = 0xFFFFFFF7
ffffff80014073ae: 0f 84 d1 fd ff ff    	je	0xffffff8001407185 <__ZN23AppleACPIPlatformExpert14evaluateObjectEP20IOACPIPlatformDevicePK8OSSymbolPP8OSObjectS7_jj+0x75>
ffffff80014073b4: e8 d7 e7 12 ff       	callq	0xffffff8000535b90 <_uuid_parse+0xffffff8000535b90>
ffffff80014073b9: 48 8b 4d b8          	movq	-0x48(%rbp), %rcx
ffffff80014073bd: 4d 63 c4             	movslq	%r12d, %r8
ffffff80014073c0: bf 12 00 17 06       	movl	$0x6170012, %edi        ## imm = 0x6170012
ffffff80014073c5: 48 89 c6             	movq	%rax, %rsi
ffffff80014073c8: 48 8b 55 c8          	movq	-0x38(%rbp), %rdx
ffffff80014073cc: 45 31 c9             	xorl	%r9d, %r9d
ffffff80014073cf: e8 9c 13 4b ff       	callq	0xffffff80008b8770 <_uuid_parse+0xffffff80008b8770>
ffffff80014073d4: e9 ac fd ff ff       	jmp	0xffffff8001407185 <__ZN23AppleACPIPlatformExpert14evaluateObjectEP20IOACPIPlatformDevicePK8OSSymbolPP8OSObjectS7_jj+0x75>
