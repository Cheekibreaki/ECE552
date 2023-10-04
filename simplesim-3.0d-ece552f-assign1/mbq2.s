	.file	1 "mbq2.c"

 # GNU C 2.7.2.3 [AL 1.1, MM 40, tma 0.1] SimpleScalar running sstrix compiled by GNU C

 # Cc1 defaults:
 # -mgas -mgpOPT

 # Cc1 arguments (-G value = 8, Cpu = default, ISA = 1):
 # -quiet -dumpbase -o

gcc2_compiled.:
__gnu_compiled_c:
	.text
	.align	2
	.globl	main

	.text

	.loc	1 11
	.ent	main
main:
	.frame	$fp,112,$31		# vars= 56, regs= 9/0, args= 16, extra= 0
	.mask	0xc07f0000,-8
	.fmask	0x00000000,0
	subu	$sp,$sp,112
	sw	$31,104($sp)
	sw	$fp,100($sp)
	sw	$22,96($sp)
	sw	$21,92($sp)
	sw	$20,88($sp)
	sw	$19,84($sp)
	sw	$18,80($sp)
	sw	$17,76($sp)
	sw	$16,72($sp)
	move	$fp,$sp
	sw	$4,112($fp)
	sw	$5,116($fp)
	jal	__main
	lw	$3,116($fp)
	addu	$2,$3,4
	lw	$4,0($2)
	jal	atoi
	move	$16,$2
	li	$4,0x00000004		# 4
	jal	malloc
	sw	$2,28($fp)
	li	$4,0x00000004		# 4
	jal	malloc
	sw	$2,32($fp)
	li	$4,0x00000004		# 4
	jal	malloc
	sw	$2,36($fp)
	move	$22,$0
$L2:
	slt	$2,$22,$16
	bne	$2,$0,$L5
	j	$L3
$L5:
	addu	$17,$20,$21
	addu	$19,$18,$20
	addu	$21,$17,$20
	addu	$17,$19,$20
	addu	$22,$22,1
 #APP
	nop
	nop
	nop
 #NO_APP
$L4:
	j	$L2
$L3:
	move	$2,$0
	j	$L1
$L1:
	move	$sp,$fp			# sp not trusted here
	lw	$31,104($sp)
	lw	$fp,100($sp)
	lw	$22,96($sp)
	lw	$21,92($sp)
	lw	$20,88($sp)
	lw	$19,84($sp)
	lw	$18,80($sp)
	lw	$17,76($sp)
	lw	$16,72($sp)
	addu	$sp,$sp,112
	j	$31
	.end	main
