	.file	1 "mbq1.c"

 # GNU C 2.7.2.3 [AL 1.1, MM 40, tma 0.1] SimpleScalar running sstrix compiled by GNU C

 # Cc1 defaults:
 # -mgas -mgpOPT

 # Cc1 arguments (-G value = 8, Cpu = default, ISA = 1):
 # -quiet -dumpbase -o

gcc2_compiled.:
__gnu_compiled_c:
	.globl	globvar
	.sdata
	.align	2
globvar:
	.word	1
	.text
	.align	2
	.globl	main

	.text

	.loc	1 6
	.ent	main
main:
	.frame	$fp,24,$31		# vars= 0, regs= 2/0, args= 16, extra= 0
	.mask	0xc0000000,-4
	.fmask	0x00000000,0
	subu	$sp,$sp,24
	sw	$31,20($sp)
	sw	$fp,16($sp)
	move	$fp,$sp
	sw	$4,24($fp)
	sw	$5,28($fp)
	jal	__main
	move	$3,$0
	li	$4,0x00000001		# 1
	li	$5,0x00000001		# 1
	move	$6,$0
	li	$7,0x00004000		# 16384
	move	$8,$0
	move	$9,$0
	li	$10,0x00989680		# 10000000
$L2:
	slt	$2,$8,$10
	bne	$2,$0,$L4
	j	$L3
$L4:
	lw	$3,globvar
	sw	$7,0($3)
	addu	$8,$8,1
 #APP
	nop
	nop
 #NO_APP
	j	$L2
$L3:
	move	$2,$0
	j	$L1
$L1:
	move	$sp,$fp			# sp not trusted here
	lw	$31,20($sp)
	lw	$fp,16($sp)
	addu	$sp,$sp,24
	j	$31
	.end	main
