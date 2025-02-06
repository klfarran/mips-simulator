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
addi	$v0, $zero, 10
syscall
	
