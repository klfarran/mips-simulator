addi 	$t0, $zero, 17
addi	$v0, $zero, 0
syscall	 
addi	$t1, $zero, 45
addi	$v0, $zero, 0
syscall
add	$s0, $t0, $t1
sub	$s1, $t0, $t1
sub	$s2, $t1, $t0
addi	$sp, $sp, -4
sw	$t0, 0($sp)
andi	$t0, $t0, 1
syscall
lw	$t0, 0($sp)
addi	$v0, $zero, 0
syscall
add 	$zero, $v0, $t0 
add	$a0, $zero, $zero
add	$s3, $zero, $zero
Loop_Start:
slti	$t0, $a0, 11
beq	$t0, $zero, After_Loop
add	$s3, $s3, $a0
addi	$a0, $a0, 1
add	$v0, $zero, $zero
syscall
j 	Loop_Start
After_Loop:
addi	$v0, $zero, 10
syscall
	
