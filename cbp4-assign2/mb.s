	.file	1 "mb.c"

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

	.loc	1 4
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
	move	$4,$0
	move	$5,$0
	li	$6,0x000003e8		# 1000
	li	$7,0x00000001		# 1
	addu	$4,$3,$6
	addu	$5,$3,$3
$L2:
	beq	$5,$3,$L3
	j	$L4
$L3:
	addu	$5,$3,$7
	bne	$5,$3,$L5
	j	$L6
$L5:
	.set	noreorder
	nop
	.set	reorder
$L4:
	addu	$5,$3,$3
$L6:
	subu	$4,$4,1
	beq	$4,$3,$L7
	j	$L2
$L7:
	move	$2,$0
	j	$L1
$L1:
	move	$sp,$fp			# sp not trusted here
	lw	$31,20($sp)
	lw	$fp,16($sp)
	addu	$sp,$sp,24
	j	$31
	.end	main
