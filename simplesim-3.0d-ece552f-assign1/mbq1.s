	.file	1 "mbq1.c"

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

	.loc	1 5
	.ent	main
main:
	.frame	$fp,32,$31		# vars= 0, regs= 4/0, args= 16, extra= 0
	.mask	0xc0030000,-4
	.fmask	0x00000000,0
	subu	$sp,$sp,32
	sw	$31,28($sp)
	sw	$fp,24($sp)
	sw	$17,20($sp)
	sw	$16,16($sp)
	move	$fp,$sp
	sw	$4,32($fp)
	sw	$5,36($fp)
	jal	__main
	move	$16,$0
	lw	$3,36($fp)
	addu	$2,$3,4
	lw	$4,0($2)
	jal	atoi
	move	$17,$2
	li	$3,0x00000001		# 1
	li	$4,0x00000001		# 1
	li	$5,0x00000001		# 1
$L2:
	slt	$2,$16,$17
	bne	$2,$0,$L4
	j	$L3
$L4:
	addu	$3,$3,1
	addu	$4,$4,$3
	addu	$16,$16,1
	addu	$3,$3,1
	addu	$5,$5,1
	addu	$4,$4,$3
	j	$L2
$L3:
	move	$2,$0
	j	$L1
$L1:
	move	$sp,$fp			# sp not trusted here
	lw	$31,28($sp)
	lw	$fp,24($sp)
	lw	$17,20($sp)
	lw	$16,16($sp)
	addu	$sp,$sp,32
	j	$31
	.end	main
