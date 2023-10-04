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
	.frame	$fp,56,$31		# vars= 0, regs= 10/0, args= 16, extra= 0
	.mask	0xc0ff0000,-4
	.fmask	0x00000000,0
	subu	$sp,$sp,56
	sw	$31,52($sp)
	sw	$fp,48($sp)
	sw	$23,44($sp)
	sw	$22,40($sp)
	sw	$21,36($sp)
	sw	$20,32($sp)
	sw	$19,28($sp)
	sw	$18,24($sp)
	sw	$17,20($sp)
	sw	$16,16($sp)
	move	$fp,$sp
	sw	$4,56($fp)
	sw	$5,60($fp)
	jal	__main
	move	$16,$0
	li	$17,0x00000001		# 1
	li	$18,0x00000001		# 1
	move	$19,$0
	move	$22,$0
	li	$23,0x000186a1		# 100001
	lw	$3,60($fp)
	addu	$2,$3,4
	lw	$4,0($2)
	jal	atoi
	move	$3,$2
	subu	$2,$3,1
	sltu	$3,$2,5
	beq	$3,$0,$L24
	sll	$3,$2,2
	la	$4,$L23
	addu	$2,$3,$4
	lw	$3,0($2)
	j	$3
	.rdata
	.align	3
	.align	2
$L23:
	.word	$L3
	.word	$L7
	.word	$L11
	.word	$L15
	.word	$L19
	.text
$L3:
	.set	noreorder
	nop
	.set	reorder
$L4:
	slt	$2,$22,$23
	bne	$2,$0,$L6
	j	$L5
$L6:
	lw	$17,globvar
	addu	$16,$17,$23
	addu	$22,$22,1
 #APP
	nop
	nop
 #NO_APP
	j	$L4
$L5:
	j	$L2
$L7:
	.set	noreorder
	nop
	.set	reorder
$L8:
	slt	$2,$22,$23
	bne	$2,$0,$L10
	j	$L9
$L10:
	lw	$17,globvar
	move	$18,$19
	addu	$16,$17,$23
	addu	$22,$22,1
 #APP
	nop
	nop
 #NO_APP
	j	$L8
$L9:
	j	$L2
$L11:
	.set	noreorder
	nop
	.set	reorder
$L12:
	slt	$2,$22,$23
	bne	$2,$0,$L14
	j	$L13
$L14:
	lw	$17,globvar
	addu	$16,$17,$23
	addu	$18,$17,$23
	addu	$19,$17,$23
	addu	$22,$22,1
 #APP
	nop
	nop
 #NO_APP
	j	$L12
$L13:
	j	$L2
$L15:
	.set	noreorder
	nop
	.set	reorder
$L16:
	slt	$2,$22,$23
	bne	$2,$0,$L18
	j	$L17
$L18:
	addu	$17,$20,$21
	addu	$19,$18,$20
	addu	$21,$17,$20
	addu	$17,$19,$20
	addu	$22,$22,1
 #APP
	nop
	nop
 #NO_APP
	j	$L16
$L17:
	j	$L2
$L19:
	.set	noreorder
	nop
	.set	reorder
$L20:
	slt	$2,$22,$23
	bne	$2,$0,$L22
	j	$L21
$L22:
	addu	$17,$20,$21
	addu	$19,$18,$20
	addu	$21,$17,$19
	addu	$22,$22,1
 #APP
	nop
	nop
 #NO_APP
	j	$L20
$L21:
	j	$L2
$L24:
$L2:
	move	$2,$0
	j	$L1
$L1:
	move	$sp,$fp			# sp not trusted here
	lw	$31,52($sp)
	lw	$fp,48($sp)
	lw	$23,44($sp)
	lw	$22,40($sp)
	lw	$21,36($sp)
	lw	$20,32($sp)
	lw	$19,28($sp)
	lw	$18,24($sp)
	lw	$17,20($sp)
	lw	$16,16($sp)
	addu	$sp,$sp,56
	j	$31
	.end	main
