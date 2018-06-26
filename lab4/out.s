.data
_ret: .asciiz "\n"
var: .word 0, 0, 0, 0, 0, 0, 0, 0
.globl main
.text
write:
  li $v0, 1
  syscall
  li $v0, 4
  la $a0, _ret
  syscall
  move $v0, $0
  jr $ra
multi :
  sw $a0, 0($s0)
  sw $a1, 4($s0)
  lw $t1, 0($s0)
  lw $t2, 4($s0)
  mul $t0, $t1, $t2
  sw $t0, 20($s0)
  lw $v0, 20($s0)
  jr $ra
main :
la $s0, var
  li $t0, 10
  sw $t0, 8($s0)
  li $t0, 1
  sw $t0, 12($s0)
  li $t0, 1
  sw $t0, 16($s0)
label1 :
  lw $t0, 16($s0)
  li $t1, 10
  ble $t0, $t1, label2
  j label3
label2 :
  lw $a0 16($s0)
  lw $a1 12($s0)
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal multi
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  sw $v0, 24($s0)
  lw $t1, 24($s0)
  move $t0, $t1
  sw $t0, 12($s0)
  lw $t0, 12($s0)
  move $a0, $t0
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal write
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  lw $t1, 16($s0)
  addi $t0, $t1, 1
  sw $t0, 16($s0)
  j label1
label3 :
  li $t0, 0
  sw $t0, 28($s0)
  lw $v0, 28($s0)
  jr $ra
