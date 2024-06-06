.data
_prompt: .asciiz "Enter an integer:"
_ret: .asciiz "\n"
.globl main
.text
read:
  li $v0, 4
  la $a0, _prompt
  syscall
  li $v0, 5
  syscall
  jr $ra

write:
  li $v0, 1
  syscall
  li $v0, 4
  la $a0, _ret
  syscall
  move $v0, $0
  jr $ra

main:
  move $fp, $sp
  addi $sp, $sp, -68
  li $t1, 0
  move $t0, $t1
  sw $t0, -4($fp)
  li $t1, 1
  move $t0, $t1
  sw $t0, -8($fp)
  li $t1, 0
  move $t0, $t1
  sw $t0, -12($fp)
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal read
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  move $t0, $v0
  sw $t0, -16($fp)
  lw $t1, -16($fp)
  move $t0, $t1
  sw $t0, -20($fp)
label1:
  lw $t1, -12($fp)
  move $t0, $t1
  sw $t0, -24($fp)
  lw $t1, -20($fp)
  move $t0, $t1
  sw $t0, -28($fp)
  lw $t0, -24($fp)
  lw $t1, -28($fp)
  blt $t0, $t1, label2
  j label3
label2:
  lw $t1, -4($fp)
  move $t0, $t1
  sw $t0, -32($fp)
  lw $t1, -8($fp)
  move $t0, $t1
  sw $t0, -36($fp)
  lw $t1, -32($fp)
  lw $t2, -36($fp)
  add $t0, $t1, $t2
  sw $t0, -40($fp)
  lw $t1, -8($fp)
  move $t0, $t1
  sw $t0, -44($fp)
  lw $t0, -44($fp)
  move $a0, $t0
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal write
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  lw $t1, -8($fp)
  move $t0, $t1
  sw $t0, -48($fp)
  lw $t1, -48($fp)
  move $t0, $t1
  sw $t0, -4($fp)
  lw $t1, -40($fp)
  move $t0, $t1
  sw $t0, -52($fp)
  lw $t1, -52($fp)
  move $t0, $t1
  sw $t0, -8($fp)
  lw $t1, -12($fp)
  move $t0, $t1
  sw $t0, -56($fp)
  li $t1, 1
  move $t0, $t1
  sw $t0, -60($fp)
  lw $t1, -56($fp)
  lw $t2, -60($fp)
  add $t0, $t1, $t2
  sw $t0, -64($fp)
  lw $t1, -64($fp)
  move $t0, $t1
  sw $t0, -12($fp)
  j label1
label3:
  li $t1, 0
  move $t0, $t1
  sw $t0, -68($fp)
  lw $t0, -68($fp)
  move $v0, $t0
  jr $ra
