	.section	__TEXT,__text,regular,pure_instructions
	.macosx_version_min 10, 11
	.globl	_main
	.align	4, 0x90
_main:                                  ## @main
	.cfi_startproc
## BB#0:
	pushq	%rbp
Ltmp0:
	.cfi_def_cfa_offset 16
Ltmp1:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
Ltmp2:
	.cfi_def_cfa_register %rbp
	subq	$48, %rsp
	movl	$0, -4(%rbp)
	callq	_getpid
	movl	%eax, -12(%rbp)         ## 4-byte Spill
	callq	_getppid
	leaq	L_.str(%rip), %rdi
	movl	-12(%rbp), %esi         ## 4-byte Reload
	movl	%eax, %edx
	movb	$0, %al
	callq	_printf
	movl	%eax, -16(%rbp)         ## 4-byte Spill
	callq	_fork
	movl	%eax, -8(%rbp)
	cmpl	$0, -8(%rbp)
	je	LBB0_2
## BB#1:
	callq	_getpid
	movl	%eax, -20(%rbp)         ## 4-byte Spill
	callq	_getppid
	leaq	L_.str1(%rip), %rdi
	movl	-8(%rbp), %ecx
	movl	-20(%rbp), %esi         ## 4-byte Reload
	movl	%eax, %edx
	movb	$0, %al
	callq	_printf
	movl	%eax, -24(%rbp)         ## 4-byte Spill
	jmp	LBB0_3
LBB0_2:
	callq	_getpid
	movl	%eax, -28(%rbp)         ## 4-byte Spill
	callq	_getppid
	leaq	L_.str2(%rip), %rdi
	movl	-28(%rbp), %esi         ## 4-byte Reload
	movl	%eax, %edx
	movb	$0, %al
	callq	_printf
	movl	%eax, -32(%rbp)         ## 4-byte Spill
LBB0_3:
	callq	_getpid
	leaq	L_.str3(%rip), %rdi
	movl	%eax, %esi
	movb	$0, %al
	callq	_printf
	xorl	%esi, %esi
	movl	%eax, -36(%rbp)         ## 4-byte Spill
	movl	%esi, %eax
	addq	$48, %rsp
	popq	%rbp
	retq
	.cfi_endproc

	.section	__TEXT,__cstring,cstring_literals
L_.str:                                 ## @.str
	.asciz	"ORIGINAL: PID=%d PPID=%d\n"

L_.str1:                                ## @.str1
	.asciz	"PARENT: PID=%d PPID=%d child=%d\n"

L_.str2:                                ## @.str2
	.asciz	"CHILD: PID=%d PPID=%d\n"

L_.str3:                                ## @.str3
	.asciz	"PID %d terminates.\n\n"


.subsections_via_symbols
