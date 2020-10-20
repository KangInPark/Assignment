	.file	"project1.cpp"
	.text
	.globl	_Z4sqrtf
	.type	_Z4sqrtf, @function
_Z4sqrtf:
.LFB30:
	.cfi_startproc
	movaps	%xmm0, %xmm3
	mulss	.LC1(%rip), %xmm0
	pxor	%xmm2, %xmm2
	jmp	.L4
.L5:
	movaps	%xmm3, %xmm1
	divss	%xmm0, %xmm1
	addss	%xmm0, %xmm1
	movaps	%xmm0, %xmm2
	mulss	.LC1(%rip), %xmm1
	movaps	%xmm1, %xmm0
.L4:
	ucomiss	%xmm2, %xmm0
	jp	.L5
	jne	.L5
	rep ret
	.cfi_endproc
.LFE30:
	.size	_Z4sqrtf, .-_Z4sqrtf
	.globl	_Z4funcii
	.type	_Z4funcii, @function
_Z4funcii:
.LFB31:
	.cfi_startproc
	pushq	%r12
	.cfi_def_cfa_offset 16
	.cfi_offset 12, -16
	pushq	%rbp
	.cfi_def_cfa_offset 24
	.cfi_offset 6, -24
	pushq	%rbx
	.cfi_def_cfa_offset 32
	.cfi_offset 3, -32
	subq	$16, %rsp
	.cfi_def_cfa_offset 48
	cmpl	$127, %edi
	je	.L14
	movslq	%edi, %rax
	movslq	%esi, %rdx
	salq	$7, %rdx
	addq	%rdx, %rax
	leaq	memo(%rip), %rdx
	movss	(%rdx,%rax,4), %xmm1
	movss	%xmm1, 12(%rsp)
	ucomiss	.LC0(%rip), %xmm1
	jp	.L6
	jne	.L6
	movl	%esi, %r12d
	movl	%edi, %ebp
	movl	$1, %ebx
	movss	.LC2(%rip), %xmm4
	movss	%xmm4, 12(%rsp)
	jmp	.L11
.L14:
	movslq	%esi, %rsi
	leaq	0(,%rsi,8), %rax
	subq	%rsi, %rax
	leaq	0(,%rax,4), %rdx
	leaq	map(%rip), %rax
	movss	(%rax,%rdx), %xmm3
	movss	%xmm3, 12(%rsp)
	movslq	%edi, %rdi
	salq	$7, %rsi
	addq	%rdi, %rsi
	leaq	memo(%rip), %rax
	movss	%xmm3, (%rax,%rsi,4)
	jmp	.L6
.L10:
	addl	$1, %ebx
.L11:
	cmpl	$6, %ebx
	jg	.L9
	movl	%ebp, %eax
	movl	%ebx, %ecx
	sarl	%cl, %eax
	testb	$1, %al
	jne	.L10
	movl	$1, %edi
	sall	%cl, %edi
	orl	%ebp, %edi
	movl	%ebx, %esi
	call	_Z4funcii
	movslq	%ebx, %rax
	movslq	%r12d, %rcx
	leaq	0(,%rcx,8), %rdx
	subq	%rcx, %rdx
	addq	%rdx, %rax
	leaq	map(%rip), %rdx
	addss	(%rdx,%rax,4), %xmm0
	movss	12(%rsp), %xmm2
	ucomiss	%xmm0, %xmm2
	jbe	.L10
	movss	%xmm0, 12(%rsp)
	jmp	.L10
.L9:
	movslq	%ebp, %rbp
	movslq	%r12d, %rsi
	salq	$7, %rsi
	addq	%rsi, %rbp
	leaq	memo(%rip), %rax
	movss	12(%rsp), %xmm5
	movss	%xmm5, (%rax,%rbp,4)
.L6:
	movss	12(%rsp), %xmm0
	addq	$16, %rsp
	.cfi_def_cfa_offset 32
	popq	%rbx
	.cfi_def_cfa_offset 24
	popq	%rbp
	.cfi_def_cfa_offset 16
	popq	%r12
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE31:
	.size	_Z4funcii, .-_Z4funcii
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC3:
	.string	"%d "
	.text
	.globl	_Z6traveliif
	.type	_Z6traveliif, @function
_Z6traveliif:
.LFB32:
	.cfi_startproc
	cmpl	$127, %edi
	je	.L22
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	pushq	%rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	subq	$8, %rsp
	.cfi_def_cfa_offset 32
	movl	$0, %ebx
	jmp	.L19
.L17:
	addl	$1, %ebx
.L19:
	cmpl	$6, %ebx
	jg	.L15
	movl	%edi, %eax
	movl	%ebx, %ecx
	sarl	%cl, %eax
	testb	$1, %al
	jne	.L17
	movslq	%ebx, %rax
	movslq	%esi, %rcx
	leaq	0(,%rcx,8), %rdx
	subq	%rcx, %rdx
	addq	%rax, %rdx
	leaq	map(%rip), %rcx
	movaps	%xmm0, %xmm2
	subss	(%rcx,%rdx,4), %xmm2
	movl	$1, %ebp
	movl	%ebx, %ecx
	sall	%cl, %ebp
	orl	%edi, %ebp
	movslq	%ebp, %rdx
	salq	$7, %rax
	addq	%rdx, %rax
	leaq	memo(%rip), %rdx
	movss	(%rdx,%rax,4), %xmm1
	ucomiss	%xmm1, %xmm2
	jp	.L17
	jne	.L17
	leal	1(%rbx), %edx
	leaq	.LC3(%rip), %rsi
	movl	$1, %edi
	movl	$0, %eax
	call	__printf_chk@PLT
	movslq	%ebp, %rdx
	movslq	%ebx, %rax
	salq	$7, %rax
	addq	%rdx, %rax
	leaq	memo(%rip), %rdx
	movss	(%rdx,%rax,4), %xmm0
	movl	%ebx, %esi
	movl	%ebp, %edi
	call	_Z6traveliif
.L15:
	addq	$8, %rsp
	.cfi_def_cfa_offset 24
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	ret
.L22:
	.cfi_restore 3
	.cfi_restore 6
	rep ret
	.cfi_endproc
.LFE32:
	.size	_Z6traveliif, .-_Z6traveliif
	.section	.rodata.str1.1
.LC4:
	.string	"1 "
.LC5:
	.string	"1"
.LC6:
	.string	"%f"
	.text
	.globl	main
	.type	main, @function
main:
.LFB33:
	.cfi_startproc
	pushq	%r13
	.cfi_def_cfa_offset 16
	.cfi_offset 13, -16
	pushq	%r12
	.cfi_def_cfa_offset 24
	.cfi_offset 12, -24
	pushq	%rbp
	.cfi_def_cfa_offset 32
	.cfi_offset 6, -32
	pushq	%rbx
	.cfi_def_cfa_offset 40
	.cfi_offset 3, -40
	subq	$88, %rsp
	.cfi_def_cfa_offset 128
	movq	%fs:40, %rax
	movq	%rax, 72(%rsp)
	xorl	%eax, %eax
	movl	$0, 16(%rsp)
	movl	$0, 20(%rsp)
	movl	$8, 24(%rsp)
	movl	$6, 28(%rsp)
	movl	$2, 32(%rsp)
	movl	$4, 36(%rsp)
	movl	$6, 40(%rsp)
	movl	$7, 44(%rsp)
	movl	$1, 48(%rsp)
	movl	$3, 52(%rsp)
	movl	$9, 56(%rsp)
	movl	$4, 60(%rsp)
	movl	$2, 64(%rsp)
	movl	$3, 68(%rsp)
	movl	$0, %r13d
	jmp	.L31
.L34:
	movslq	%ebx, %rcx
	movslq	%r13d, %rdx
	leaq	0(,%rdx,8), %rax
	subq	%rdx, %rax
	movq	%rax, %rdx
	movq	%rcx, %rax
	addq	%rdx, %rax
	leaq	map(%rip), %rdx
	movl	$0x00000000, (%rdx,%rax,4)
.L29:
	addl	$1, %ebx
.L30:
	cmpl	$6, %ebx
	jg	.L27
	cmpl	%ebx, %r13d
	je	.L34
	movslq	%ebx, %r12
	movl	16(%rsp,%r12,8), %eax
	movslq	%r13d, %rbp
	subl	16(%rsp,%rbp,8), %eax
	movl	20(%rsp,%r12,8), %edx
	subl	20(%rsp,%rbp,8), %edx
	imull	%eax, %eax
	imull	%edx, %edx
	addl	%edx, %eax
	pxor	%xmm0, %xmm0
	cvtsi2ss	%eax, %xmm0
	call	_Z4sqrtf
	leaq	0(,%rbp,8), %rax
	subq	%rbp, %rax
	addq	%rax, %r12
	leaq	map(%rip), %rax
	movss	%xmm0, (%rax,%r12,4)
	jmp	.L29
.L27:
	addl	$1, %r13d
.L31:
	cmpl	$6, %r13d
	jg	.L26
	movl	$0, %ebx
	jmp	.L30
.L26:
	movl	$0, %esi
	movl	$1, %edi
	call	_Z4funcii
	movss	%xmm0, 12(%rsp)
	leaq	.LC4(%rip), %rsi
	movl	$1, %edi
	movl	$0, %eax
	call	__printf_chk@PLT
	movss	12(%rsp), %xmm0
	movl	$0, %esi
	movl	$1, %edi
	call	_Z6traveliif
	leaq	.LC5(%rip), %rdi
	call	puts@PLT
	pxor	%xmm0, %xmm0
	cvtss2sd	12(%rsp), %xmm0
	leaq	.LC6(%rip), %rsi
	movl	$1, %edi
	movl	$1, %eax
	call	__printf_chk@PLT
	movl	$0, %eax
	movq	72(%rsp), %rcx
	xorq	%fs:40, %rcx
	jne	.L35
	addq	$88, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 40
	popq	%rbx
	.cfi_def_cfa_offset 32
	popq	%rbp
	.cfi_def_cfa_offset 24
	popq	%r12
	.cfi_def_cfa_offset 16
	popq	%r13
	.cfi_def_cfa_offset 8
	ret
.L35:
	.cfi_restore_state
	call	__stack_chk_fail@PLT
	.cfi_endproc
.LFE33:
	.size	main, .-main
	.globl	memo
	.bss
	.align 32
	.type	memo, @object
	.size	memo, 3584
memo:
	.zero	3584
	.globl	map
	.align 32
	.type	map, @object
	.size	map, 196
map:
	.zero	196
	.section	.rodata.cst4,"aM",@progbits,4
	.align 4
.LC0:
	.long	0
	.align 4
.LC1:
	.long	1056964608
	.align 4
.LC2:
	.long	1287568416
	.ident	"GCC: (Ubuntu 7.3.0-27ubuntu1~18.04) 7.3.0"
	.section	.note.GNU-stack,"",@progbits
