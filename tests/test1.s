# file: main.s
.extern handler
.global g
.section my_code_main
 ld 0x4000, %sp
 ld $handler, %r1
 csrwr %r1, %handler
 ld $0x1, %r1
 st %r1, wait
wait:
 ld my_counter, %r1
 push %r1
 ld $5, %r15
 beq %r1, %r2, wait
 jmp wait
 halt
 call 0x12345
 add %r1,%r2
 pop %r1
.global my_counter
.section my_data
g:
my_counter:
 .word 0
 .skip 3
numeroTres:
 .word numeroTres
.end