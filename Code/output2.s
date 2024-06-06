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

mod:
  move $fp, $sp
  addi $sp, $sp, -28
  lw $t1, 8($fp)
  move $t0, $t1
  sw $t0, -4($fp)
  lw $t1, 8($fp)
  move $t0, $t1
  sw $t0, -8($fp)
  lw $t1, 12($fp)
  move $t0, $t1
  sw $t0, -12($fp)
  lw $t1, -8($fp)
  lw $t2, -12($fp)
  div $t1, $t2
  mflo $t0
  sw $t0, -16($fp)
  lw $t1, 12($fp)
  move $t0, $t1
  sw $t0, -20($fp)
  lw $t1, -16($fp)
  lw $t2, -20($fp)
  mul $t0, $t1, $t2
  sw $t0, -24($fp)
  lw $t1, -4($fp)
  lw $t2, -24($fp)
  sub $t0, $t1, $t2
  sw $t0, -28($fp)
  lw $t0, -28($fp)
  move $v0, $t0
  jr $ra
quick_power_mod:
  move $fp, $sp
  addi $sp, $sp, -128
  li $t1, 1
  move $t0, $t1
  sw $t0, -4($fp)
  lw $t1, 8($fp)
  move $t0, $t1
  sw $t0, -8($fp)
  li $t1, 0
  move $t0, $t1
  sw $t0, -12($fp)
  lw $t0, -8($fp)
  lw $t1, -12($fp)
  ble $t0, $t1, label1
  j label5
label5:
  lw $t1, 12($fp)
  move $t0, $t1
  sw $t0, -16($fp)
  li $t1, 0
  move $t0, $t1
  sw $t0, -20($fp)
  lw $t0, -16($fp)
  lw $t1, -20($fp)
  ble $t0, $t1, label1
  j label4
label4:
  lw $t1, 16($fp)
  move $t0, $t1
  sw $t0, -24($fp)
  li $t1, 0
  move $t0, $t1
  sw $t0, -28($fp)
  lw $t0, -24($fp)
  lw $t1, -28($fp)
  ble $t0, $t1, label1
  j label2
label1:
  li $t1, 1
  move $t0, $t1
  sw $t0, -32($fp)
  li $t1, 0
  lw $t2, -32($fp)
  sub $t0, $t1, $t2
  sw $t0, -36($fp)
  lw $t0, -36($fp)
  move $v0, $t0
  jr $ra
  j label3
label2:
  lw $t1, 8($fp)
  move $t0, $t1
  sw $t0, -40($fp)
  lw $t1, 16($fp)
  move $t0, $t1
  sw $t0, -44($fp)
  lw $t0, -44($fp)
  addi $sp, $sp, -4
  sw $t0, 0($sp)
  lw $t0, -40($fp)
  addi $sp, $sp, -4
  sw $t0, 0($sp)
  addi $sp, $sp, -8
  sw $ra, 4($sp)
  sw $fp, 0($sp)
  move $fp, $sp
  jal mod
  move $sp, $fp
  lw $fp, 0($sp)
  lw $ra, 4($sp)
  addi $sp, $sp, 8
  addi $sp, $sp, 8
  move $t0, $v0
  sw $t0, -48($fp)
  lw $t1, -48($fp)
  move $t0, $t1
  sw $t0, 8($fp)
label6:
  lw $t1, 12($fp)
  move $t0, $t1
  sw $t0, -52($fp)
  li $t1, 0
  move $t0, $t1
  sw $t0, -56($fp)
  lw $t0, -52($fp)
  lw $t1, -56($fp)
  bne $t0, $t1, label7
  j label8
label7:
  lw $t1, 12($fp)
  move $t0, $t1
  sw $t0, -60($fp)
  li $t1, 2
  move $t0, $t1
  sw $t0, -64($fp)
  lw $t0, -64($fp)
  addi $sp, $sp, -4
  sw $t0, 0($sp)
  lw $t0, -60($fp)
  addi $sp, $sp, -4
  sw $t0, 0($sp)
  addi $sp, $sp, -8
  sw $ra, 4($sp)
  sw $fp, 0($sp)
  move $fp, $sp
  jal mod
  move $sp, $fp
  lw $fp, 0($sp)
  lw $ra, 4($sp)
  addi $sp, $sp, 8
  addi $sp, $sp, 8
  move $t0, $v0
  sw $t0, -68($fp)
  li $t1, 1
  move $t0, $t1
  sw $t0, -72($fp)
  lw $t0, -68($fp)
  lw $t1, -72($fp)
  beq $t0, $t1, label9
  j label10
label9:
  lw $t1, -4($fp)
  move $t0, $t1
  sw $t0, -76($fp)
  lw $t1, 8($fp)
  move $t0, $t1
  sw $t0, -80($fp)
  lw $t1, -76($fp)
  lw $t2, -80($fp)
  mul $t0, $t1, $t2
  sw $t0, -84($fp)
  lw $t1, 16($fp)
  move $t0, $t1
  sw $t0, -88($fp)
  lw $t0, -88($fp)
  addi $sp, $sp, -4
  sw $t0, 0($sp)
  lw $t0, -84($fp)
  addi $sp, $sp, -4
  sw $t0, 0($sp)
  addi $sp, $sp, -8
  sw $ra, 4($sp)
  sw $fp, 0($sp)
  move $fp, $sp
  jal mod
  move $sp, $fp
  lw $fp, 0($sp)
  lw $ra, 4($sp)
  addi $sp, $sp, 8
  addi $sp, $sp, 8
  move $t0, $v0
  sw $t0, -92($fp)
  lw $t1, -92($fp)
  move $t0, $t1
  sw $t0, -4($fp)
label10:
  lw $t1, 12($fp)
  move $t0, $t1
  sw $t0, -96($fp)
  li $t1, 2
  move $t0, $t1
  sw $t0, -100($fp)
  lw $t1, -96($fp)
  lw $t2, -100($fp)
  div $t1, $t2
  mflo $t0
  sw $t0, -104($fp)
  lw $t1, -104($fp)
  move $t0, $t1
  sw $t0, 12($fp)
  lw $t1, 8($fp)
  move $t0, $t1
  sw $t0, -108($fp)
  lw $t1, 8($fp)
  move $t0, $t1
  sw $t0, -112($fp)
  lw $t1, -108($fp)
  lw $t2, -112($fp)
  mul $t0, $t1, $t2
  sw $t0, -116($fp)
  lw $t1, 16($fp)
  move $t0, $t1
  sw $t0, -120($fp)
  lw $t0, -120($fp)
  addi $sp, $sp, -4
  sw $t0, 0($sp)
  lw $t0, -116($fp)
  addi $sp, $sp, -4
  sw $t0, 0($sp)
  addi $sp, $sp, -8
  sw $ra, 4($sp)
  sw $fp, 0($sp)
  move $fp, $sp
  jal mod
  move $sp, $fp
  lw $fp, 0($sp)
  lw $ra, 4($sp)
  addi $sp, $sp, 8
  addi $sp, $sp, 8
  move $t0, $v0
  sw $t0, -124($fp)
  lw $t1, -124($fp)
  move $t0, $t1
  sw $t0, 8($fp)
  j label6
label8:
  lw $t1, -4($fp)
  move $t0, $t1
  sw $t0, -128($fp)
  lw $t0, -128($fp)
  move $v0, $t0
  jr $ra
label3:
main:
  move $fp, $sp
  addi $sp, $sp, -124
  li $t1, 0
  move $t0, $t1
  sw $t0, -16($fp)
label11:
  lw $t1, -16($fp)
  move $t0, $t1
  sw $t0, -20($fp)
  li $t1, 3
  move $t0, $t1
  sw $t0, -24($fp)
  lw $t0, -20($fp)
  lw $t1, -24($fp)
  blt $t0, $t1, label12
  j label13
label12:
  move $t0, $t1
  lw $t2, -16($fp)
  move $t1, $t2
  sw $t1, -32($fp)
  lw $t2, -32($fp)
  li $t3, 4
  mul $t1, $t2, $t3
  sw $t1, -36($fp)
  lw $t3, -36($fp)
  add $t1, $t2, $t3
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal read
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  move $t2, $v0
  sw $t2, -44($fp)
  lw $t3, -44($fp)
  move $t2, $t3
  lw $t4, -16($fp)
  move $t3, $t4
  sw $t3, -48($fp)
  li $t4, 1
  move $t3, $t4
  sw $t3, -52($fp)
  lw $t4, -48($fp)
  lw $t5, -52($fp)
  add $t3, $t4, $t5
  sw $t3, -56($fp)
  lw $t4, -56($fp)
  move $t3, $t4
  sw $t3, -16($fp)
  j label11
label13:
  move $t3, $t4
  li $t5, 0
  move $t4, $t5
  sw $t4, -64($fp)
  lw $t5, -64($fp)
  li $t6, 4
  mul $t4, $t5, $t6
  sw $t4, -68($fp)
  lw $t6, -68($fp)
  add $t4, $t5, $t6
  move $t5, $t6
  sw $t5, -76($fp)
  move $t5, $t6
  li $t7, 1
  move $t6, $t7
  sw $t6, -84($fp)
  lw $t7, -84($fp)
  li $s0, 4
  mul $t6, $t7, $s0
  sw $t6, -88($fp)
  lw $s0, -88($fp)
  add $t6, $t7, $s0
  move $t7, $s0
  sw $t7, -96($fp)
  move $t7, $s0
  li $s0, 2
  move $s0, $s0
  sw $s0, -104($fp)
  lw $s0, -104($fp)
  li $s0, 4
  mul $s0, $s0, $s0
  sw $s0, -104($fp)
  lw $s0, -108($fp)
  add $s0, $s0, $s0
  sw $s0, -108($fp)
  move $s0, $s0
  sw $s0, -116($fp)
  lw $s0, -116($fp)
  addi $sp, $sp, -4
  sw $s0, 0($sp)
  lw $s0, -96($fp)
  addi $sp, $sp, -4
  sw $s0, 0($sp)
  lw $s0, -76($fp)
  addi $sp, $sp, -4
  sw $s0, 0($sp)
  addi $sp, $sp, -8
  sw $ra, 4($sp)
  sw $fp, 0($sp)
  move $fp, $sp
  jal quick_power_mod
  move $sp, $fp
  lw $fp, 0($sp)
  lw $ra, 4($sp)
  addi $sp, $sp, 8
  addi $sp, $sp, 12
  move $s0, $v0
  sw $s0, -120($fp)
  lw $s0, -120($fp)
  move $a0, $s0
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal write
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  li $s0, 0
  move $s0, $s0
  sw $s0, -124($fp)
  lw $s0, -124($fp)
  move $v0, $s0
  jr $ra
