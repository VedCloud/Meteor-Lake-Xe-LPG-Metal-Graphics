
research/IO80211Family-24H32-x86_64.macho:	file format mach-o 64-bit x86-64

Disassembly of section __TEXT,__text:

000000000009f6e8 <__ZN22IO80211AWDLPeerManager15dequeueTxPacketEP15TxPacketRequest>:
   9f6e8:      	pushq	%rbp
   9f6e9:      	movq	%rsp, %rbp
   9f6ec:      	pushq	%r15
   9f6ee:      	pushq	%r14
   9f6f0:      	pushq	%r13
   9f6f2:      	pushq	%r12
   9f6f4:      	pushq	%rbx
   9f6f5:      	subq	$0x48, %rsp
   9f6f9:      	movq	%rdi, %r15
   9f6fc:      	movq	0x18(%rdi), %rax
   9f700:      	cmpq	$0x0, 0x520(%rax)
   9f708:      	je	0x9f741 <__ZN22IO80211AWDLPeerManager15dequeueTxPacketEP15TxPacketRequest+0x59>
   9f70a:      	cmpq	$0x0, 0x528(%rax)
   9f712:      	je	0x9f785 <__ZN22IO80211AWDLPeerManager15dequeueTxPacketEP15TxPacketRequest+0x9d>
   9f714:      	leaq	0x190366(%rip), %rdx    ## 0x22fa81 <_.str.410>
   9f71b:      	leaq	0x18c86d(%rip), %rcx    ## 0x22bf8f <_.str.17>
   9f722:      	leaq	0x19e7b9(%rip), %r8     ## 0x23dee2 <___FUNCTION__._ZN22IO80211AWDLPeerManager28preQueuePacketCheckQuietModeEP12ether_header+0x65>
   9f729:      	xorl	%r14d, %r14d
   9f72c:      	movabsq	$0x800000000000000, %rsi ## imm = 0x800000000000000
   9f736:      	movq	%r15, %rdi
   9f739:      	movl	$0x5727, %r9d           ## imm = 0x5727
   9f73f:      	jmp	0x9f76c <__ZN22IO80211AWDLPeerManager15dequeueTxPacketEP15TxPacketRequest+0x84>
   9f741:      	leaq	0x190339(%rip), %rdx    ## 0x22fa81 <_.str.410>
   9f748:      	leaq	0x18c840(%rip), %rcx    ## 0x22bf8f <_.str.17>
   9f74f:      	leaq	0x19e78c(%rip), %r8     ## 0x23dee2 <___FUNCTION__._ZN22IO80211AWDLPeerManager28preQueuePacketCheckQuietModeEP12ether_header+0x65>
   9f756:      	xorl	%r14d, %r14d
   9f759:      	movabsq	$0x800000000000000, %rsi ## imm = 0x800000000000000
   9f763:      	movq	%r15, %rdi
   9f766:      	movl	$0x5722, %r9d           ## imm = 0x5722
   9f76c:      	xorl	%eax, %eax
   9f76e:      	callq	0xca96e <__ZN18IO80211PeerManager8logDebugEyPKcz>
   9f773:      	movl	%r14d, %eax
   9f776:      	addq	$0x48, %rsp
   9f77a:      	popq	%rbx
   9f77b:      	popq	%r12
   9f77d:      	popq	%r13
   9f77f:      	popq	%r14
   9f781:      	popq	%r15
   9f783:      	popq	%rbp
   9f784:      	retq
   9f785:      	cmpq	$0x0, 0x530(%rax)
   9f78d:      	je	0x9f7d8 <__ZN22IO80211AWDLPeerManager15dequeueTxPacketEP15TxPacketRequest+0xf0>
   9f78f:      	movq	%rsi, %rbx
   9f792:      	movl	$0x3, %eax
   9f797:      	movq	$0x0, (%rbx,%rax,8)
   9f79f:      	incq	%rax
   9f7a2:      	cmpq	$0x7, %rax
   9f7a6:      	jne	0x9f797 <__ZN22IO80211AWDLPeerManager15dequeueTxPacketEP15TxPacketRequest+0xaf>
   9f7a8:      	movq	0x18(%r15), %rax
   9f7ac:      	movq	0x78(%rax), %rdi
   9f7b0:      	movq	(%rdi), %rax
   9f7b3:      	callq	*0x118(%rax)
   9f7b9:      	testb	%al, %al
   9f7bb:      	je	0x9f808 <__ZN22IO80211AWDLPeerManager15dequeueTxPacketEP15TxPacketRequest+0x120>
   9f7bd:      	movq	0x20(%r15), %rax
   9f7c1:      	cmpl	$0x0, (%rax)
   9f7c4:      	je	0x9f838 <__ZN22IO80211AWDLPeerManager15dequeueTxPacketEP15TxPacketRequest+0x150>
   9f7c6:      	cmpl	$0x4, 0x198(%rax)
   9f7cd:      	jae	0x9f855 <__ZN22IO80211AWDLPeerManager15dequeueTxPacketEP15TxPacketRequest+0x16d>
   9f7d3:      	jmp	0x9f85b <__ZN22IO80211AWDLPeerManager15dequeueTxPacketEP15TxPacketRequest+0x173>
   9f7d8:      	leaq	0x1902a2(%rip), %rdx    ## 0x22fa81 <_.str.410>
   9f7df:      	leaq	0x18c7a9(%rip), %rcx    ## 0x22bf8f <_.str.17>
   9f7e6:      	leaq	0x19e6f5(%rip), %r8     ## 0x23dee2 <___FUNCTION__._ZN22IO80211AWDLPeerManager28preQueuePacketCheckQuietModeEP12ether_header+0x65>
   9f7ed:      	xorl	%r14d, %r14d
   9f7f0:      	movabsq	$0x800000000000000, %rsi ## imm = 0x800000000000000
   9f7fa:      	movq	%r15, %rdi
   9f7fd:      	movl	$0x572c, %r9d           ## imm = 0x572C
   9f803:      	jmp	0x9f76c <__ZN22IO80211AWDLPeerManager15dequeueTxPacketEP15TxPacketRequest+0x84>
   9f808:      	leaq	0x18df78(%rip), %rdx    ## 0x22d787 <_.str.124+0x7e1>
   9f80f:      	leaq	0x19e6cc(%rip), %rcx    ## 0x23dee2 <___FUNCTION__._ZN22IO80211AWDLPeerManager28preQueuePacketCheckQuietModeEP12ether_header+0x65>
   9f816:      	xorl	%r14d, %r14d
   9f819:      	movabsq	$0x800000000000000, %rsi ## imm = 0x800000000000000
   9f823:      	movq	%r15, %rdi
   9f826:      	movl	$0x5738, %r8d           ## imm = 0x5738
   9f82c:      	xorl	%eax, %eax
   9f82e:      	callq	0xca96e <__ZN18IO80211PeerManager8logDebugEyPKcz>
   9f833:      	jmp	0x9f773 <__ZN22IO80211AWDLPeerManager15dequeueTxPacketEP15TxPacketRequest+0x8b>
   9f838:      	cmpl	$0x5, 0x28cc(%rax)
   9f83f:      	je	0x9f855 <__ZN22IO80211AWDLPeerManager15dequeueTxPacketEP15TxPacketRequest+0x16d>
   9f841:      	cmpb	$0x0, 0x4290(%rax)
   9f848:      	je	0x9f855 <__ZN22IO80211AWDLPeerManager15dequeueTxPacketEP15TxPacketRequest+0x16d>
   9f84a:      	movw	$0x0, 0x58(%rbx)
   9f850:      	xorl	%r14d, %r14d
   9f853:      	jmp	0x9f869 <__ZN22IO80211AWDLPeerManager15dequeueTxPacketEP15TxPacketRequest+0x181>
   9f855:      	movw	$0x0, 0x6(%rbx)
   9f85b:      	movq	%r15, %rdi
   9f85e:      	movq	%rbx, %rsi
   9f861:      	callq	0xd4b2e <__ZN18IO80211PeerManager15dequeueTxPacketEP15TxPacketRequest>
   9f866:      	movl	%eax, %r14d
   9f869:      	movq	0x18(%r15), %rax
   9f86d:      	movq	0x78(%rax), %rdi
   9f871:      	movq	(%rdi), %rax
   9f874:      	callq	*0x120(%rax)
   9f87a:      	movq	(%r15), %rax
   9f87d:      	movq	%r15, %rdi
   9f880:      	callq	*0x38(%rax)
   9f883:      	movq	%rax, %rdi
   9f886:      	callq	0x9f88b <__ZN22IO80211AWDLPeerManager15dequeueTxPacketEP15TxPacketRequest+0x1a3>
   9f88b:      	movq	0x18(%r15), %rcx
   9f88f:      	movq	0x530(%rcx), %rdx
   9f896:      	testq	%rdx, %rdx
   9f899:      	je	0x9f8a8 <__ZN22IO80211AWDLPeerManager15dequeueTxPacketEP15TxPacketRequest+0x1c0>
   9f89b:      	movq	0x10(%rdx), %rdx
   9f89f:      	movl	0x24(%rdx), %edx
   9f8a2:      	movq	%rdx, -0x30(%rbp)
   9f8a6:      	jmp	0x9f8b0 <__ZN22IO80211AWDLPeerManager15dequeueTxPacketEP15TxPacketRequest+0x1c8>
   9f8a8:      	movq	$0x0, -0x30(%rbp)
   9f8b0:      	movzwl	0x2(%rbx), %r9d
   9f8b5:      	movzwl	0x4(%rbx), %edx
   9f8b9:      	movq	%rdx, -0x68(%rbp)
   9f8bd:      	movzwl	0x6(%rbx), %edx
   9f8c1:      	movq	%rdx, -0x60(%rbp)
   9f8c5:      	movzwl	0x8(%rbx), %edx
   9f8c9:      	movq	%rdx, -0x58(%rbp)
   9f8cd:      	movzwl	0xa(%rbx), %edx
   9f8d1:      	movq	%rdx, -0x50(%rbp)
   9f8d5:      	movzwl	0xc(%rbx), %edx
   9f8d9:      	movq	%rdx, -0x48(%rbp)
   9f8dd:      	movzwl	0xe(%rbx), %edx
   9f8e1:      	movq	%rdx, -0x40(%rbp)
   9f8e5:      	movl	0x990(%rcx), %edx
   9f8eb:      	movq	%rdx, -0x38(%rbp)
   9f8ef:      	movq	0x20(%r15), %rdx
   9f8f3:      	movl	(%rdx), %r13d
   9f8f6:      	movl	$0x1, %r10d
   9f8fc:      	cmpl	$0x5, 0x28cc(%rdx)
   9f903:      	je	0x9f913 <__ZN22IO80211AWDLPeerManager15dequeueTxPacketEP15TxPacketRequest+0x22b>
   9f905:      	xorl	%r10d, %r10d
   9f908:      	cmpb	$0x0, 0x4290(%rdx)
   9f90f:      	sete	%r10b
   9f913:      	movzbl	0x6b9(%rcx), %r11d
   9f91b:      	leaq	0x19e5d0(%rip), %rdx    ## 0x23def2 <___FUNCTION__._ZN22IO80211AWDLPeerManager28preQueuePacketCheckQuietModeEP12ether_header+0x75>
   9f922:      	leaq	0x19e5b9(%rip), %r8     ## 0x23dee2 <___FUNCTION__._ZN22IO80211AWDLPeerManager28preQueuePacketCheckQuietModeEP12ether_header+0x65>
   9f929:      	xorl	%r12d, %r12d
   9f92c:      	movabsq	$0x400000000, %rsi      ## imm = 0x400000000
   9f936:      	movq	%r15, %rdi
   9f939:      	movq	%rax, %rcx
   9f93c:      	xorl	%eax, %eax
   9f93e:      	pushq	%r14
   9f940:      	pushq	%r11
   9f942:      	pushq	%r10
   9f944:      	pushq	%r13
   9f946:      	pushq	-0x38(%rbp)
   9f949:      	pushq	-0x30(%rbp)
   9f94c:      	pushq	-0x40(%rbp)
   9f94f:      	pushq	-0x48(%rbp)
   9f952:      	pushq	-0x50(%rbp)
   9f955:      	pushq	-0x58(%rbp)
   9f958:      	pushq	-0x60(%rbp)
   9f95b:      	pushq	-0x68(%rbp)
   9f95e:      	callq	0xca96e <__ZN18IO80211PeerManager8logDebugEyPKcz>
   9f963:      	addq	$0x60, %rsp
   9f967:      	movq	0x18(%rbx,%r12,8), %rax
   9f96c:      	movw	$0x0, 0x8(%rbx,%r12,2)
   9f974:      	testq	%rax, %rax
   9f977:      	je	0x9f989 <__ZN22IO80211AWDLPeerManager15dequeueTxPacketEP15TxPacketRequest+0x2a1>
   9f979:      	incw	0x8(%rbx,%r12,2)
   9f97f:      	movq	%rax, %rdi
   9f982:      	callq	0x9f987 <__ZN22IO80211AWDLPeerManager15dequeueTxPacketEP15TxPacketRequest+0x29f>
   9f987:      	jmp	0x9f974 <__ZN22IO80211AWDLPeerManager15dequeueTxPacketEP15TxPacketRequest+0x28c>
   9f989:      	incq	%r12
   9f98c:      	cmpq	$0x4, %r12
   9f990:      	jne	0x9f967 <__ZN22IO80211AWDLPeerManager15dequeueTxPacketEP15TxPacketRequest+0x27f>
   9f992:      	jmp	0x9f773 <__ZN22IO80211AWDLPeerManager15dequeueTxPacketEP15TxPacketRequest+0x8b>
   9f997:      	nop

000000000009f998 <__ZN22IO80211AWDLPeerManager16isAwdlInDataModeEv>:
   9f998:      	pushq	%rbp
   9f999:      	movq	%rsp, %rbp
   9f99c:      	movq	0x20(%rdi), %rcx
   9f9a0:      	movb	$0x1, %al
   9f9a2:      	cmpl	$0x5, 0x28cc(%rcx)
   9f9a9:      	je	0x9f9b5 <__ZN22IO80211AWDLPeerManager16isAwdlInDataModeEv+0x1d>
   9f9ab:      	cmpb	$0x0, 0x4290(%rcx)
   9f9b2:      	sete	%al
   9f9b5:      	popq	%rbp
   9f9b6:      	retq
   9f9b7:      	nop

000000000009f9b8 <__ZN22IO80211AWDLPeerManager19getMulticastPacketsEP15TxPacketRequest>:
   9f9b8:      	pushq	%rbp
   9f9b9:      	movq	%rsp, %rbp
   9f9bc:      	pushq	%rbx
   9f9bd:      	pushq	%rax
   9f9be:      	movq	%rdi, %rbx
   9f9c1:      	callq	0xd4428 <__ZN18IO80211PeerManager19getMulticastPacketsEP15TxPacketRequest>
   9f9c6:      	movq	0x20(%rbx), %rcx
   9f9ca:      	addl	%eax, 0x3ff8(%rcx)
   9f9d0:      	addq	$0x8, %rsp
   9f9d4:      	popq	%rbx
   9f9d5:      	popq	%rbp
   9f9d6:      	retq
   9f9d7:      	nop

000000000009f9d8 <__ZN22IO80211AWDLPeerManager17getUnicastPacketsEP15TxPacketRequest17apple80211_wme_ac>:
   9f9d8:      	pushq	%rbp
   9f9d9:      	movq	%rsp, %rbp
   9f9dc:      	pushq	%rbx
   9f9dd:      	pushq	%rax
   9f9de:      	movq	%rdi, %rbx
   9f9e1:      	callq	0xd45b6 <__ZN18IO80211PeerManager17getUnicastPacketsEP15TxPacketRequest17apple80211_wme_ac>
   9f9e6:      	movq	0x20(%rbx), %rcx
   9f9ea:      	addl	%eax, 0x3ff0(%rcx)
   9f9f0:      	addq	$0x8, %rsp
   9f9f4:      	popq	%rbx
   9f9f5:      	popq	%rbp
   9f9f6:      	retq
   9f9f7:      	nop

000000000009f9f8 <__ZN22IO80211AWDLPeerManager20adjustAwdlSnapHeaderEP23IO80211PacketDescriptorP11IO80211Peer>:
   9f9f8:      	pushq	%rbp
   9f9f9:      	movq	%rsp, %rbp
   9f9fc:      	pushq	%rbx
   9f9fd:      	pushq	%rax
   9f9fe:      	testq	%rsi, %rsi
   9fa01:      	je	0x9fa1a <__ZN22IO80211AWDLPeerManager20adjustAwdlSnapHeaderEP23IO80211PacketDescriptorP11IO80211Peer+0x22>
   9fa03:      	movq	0x18(%rsi), %rax
   9fa07:      	movq	0x28(%rax), %rbx
   9fa0b:      	testq	%rbx, %rbx
   9fa0e:      	je	0x9fa43 <__ZN22IO80211AWDLPeerManager20adjustAwdlSnapHeaderEP23IO80211PacketDescriptorP11IO80211Peer+0x4b>
   9fa10:      	movq	%rbx, %rsi
   9fa13:      	callq	0x9fa7c <__ZN22IO80211AWDLPeerManager20adjustAwdlSnapHeaderEP6__mbufP11IO80211Peer>
   9fa18:      	jmp	0x9fa71 <__ZN22IO80211AWDLPeerManager20adjustAwdlSnapHeaderEP23IO80211PacketDescriptorP11IO80211Peer+0x79>
   9fa1a:      	leaq	0x190060(%rip), %rdx    ## 0x22fa81 <_.str.410>
   9fa21:      	leaq	0x18c567(%rip), %rcx    ## 0x22bf8f <_.str.17>
   9fa28:      	leaq	0x19e531(%rip), %r8     ## 0x23df60 <___FUNCTION__._ZN22IO80211AWDLPeerManager20adjustAwdlSnapHeaderEP23IO80211PacketDescriptorP11IO80211Peer>
   9fa2f:      	xorl	%ebx, %ebx
   9fa31:      	movabsq	$0x800000000000000, %rsi ## imm = 0x800000000000000
   9fa3b:      	movl	$0x5781, %r9d           ## imm = 0x5781
   9fa41:      	jmp	0x9fa6a <__ZN22IO80211AWDLPeerManager20adjustAwdlSnapHeaderEP23IO80211PacketDescriptorP11IO80211Peer+0x72>
   9fa43:      	leaq	0x190037(%rip), %rdx    ## 0x22fa81 <_.str.410>
   9fa4a:      	leaq	0x18c53e(%rip), %rcx    ## 0x22bf8f <_.str.17>
   9fa51:      	leaq	0x19e508(%rip), %r8     ## 0x23df60 <___FUNCTION__._ZN22IO80211AWDLPeerManager20adjustAwdlSnapHeaderEP23IO80211PacketDescriptorP11IO80211Peer>
   9fa58:      	xorl	%ebx, %ebx
   9fa5a:      	movabsq	$0x800000000000000, %rsi ## imm = 0x800000000000000
