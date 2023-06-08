This was done as a project for my System Software class. The aim of this project was to implement a tool in the translation chain and an emulator for an abstract computer system. The project solution includes the source code implementing a two-pass assembly, linker, and emulator.

Part 1: Assembly

The input of this assembly is a text (.s) file thet contains an assembly source code. Assembly translates this code into a ELF-like object (.o) file. 
The following syntax is used for this assembly: 
	-halt: halts the exectuion; 
	-int: triggers a software interrupt; 
	-iret: pop pc; pop status; 
	-call operand: push pc; pc <= operand; 
	-ret: pop pc; 
	-jmp operand: pc <= operand; 
	-beq %gpr1, %gpr2, operand: if (gpr1 == gpr2) pc <= operand; 
	-bne %gpr1, %gpr2, operand: if (gpr1 != gpr2) pc <= operand; 
	-bgt %gpr1, %gpr2, operand: if (gpr1 signed> gpr2) pc <= operand; 
	-push %gpr: sp <= sp - 4; mem32[sp] <= gpr; 
	-pop %gpr: gpr <= mem32[sp]; sp <= sp + 4; 
	-xchg %gprS, %gprD: temp <= gprD; gprD <= gprS; gprS <= temp; 
	-add %gprS, %gprD: gprD <= gprD + gprS; 
	-sub %gprS, %gprD: gprD <= gprD - gprS; 
	-mul %gprS, %gprD: gprD <= gprD * gprS; 
	-div %gprS, %gprD: gprD <= gprD / gprS; 
	-not %gpr: gpr <= ~gpr; 
	-and %gprS, %gprD: gprD <= gprD & gprS; 
	-or %gprS, %gprD: gprD <= gprD | gprS 
	-xor %gprS, %gprD: gprD <= gprD ^ gprS; 
	-shl %gprS, %gprD: gprD <= gprD << gprS; 
	-shr %gprS, %gprD: gprD <= gprD >> gprS; 
	-ld operand, %gpr: gpr <= operand; 
	-st %gpr, operand: operand <= gpr; 
	-csrrd %csr, %gpr: gpr <= csr 
	-csrwr %gpr, %csr: csr <= gpr;

Syntax notation operand for branch and call instructions can be either a symbol or a literal value. For store instructions it can be a symbol or literal but its used for memory direct addresing (their value is used for storing the value of a register into that location) , [%gpr] which is used for register indirect addressing and [%gpr + literal/symbol] which is used for register indirect addressing with offest. The load instruction can use syntax notation operand same as store with the addition of $literal, $symbol - used for immediete addressing and %gpr which is used for register direct addresing. Syntax notation %gpr represents a 32-bit general purpouse register which is written as %r0-%r15, %sp (which is also noted as %r14) and %pc (as %r15). Syntax notation %csr represents 32-bit control status registers. There are 3 of them : %handler, %status and %cause. Every machine instruction is 4 bytes long.

This assembly also uses following directives: 
	-.word <symbol/literal/ list of them>, initilazes current memory location (4 bytes) with values of all symbols and literals listed (4 bytes for each literal/symbol listed) 
	-.skip initilizes the next value bytes with zeros 
	-.section defines the start of a new section with name and end previous section 
	-.extern imports a symbol from a different assembly file 
	-.global exports a symbol, so it can be used in other files 
	-.end, ends the assembling of current file

Examples of this assembly syntax are in tests directive.

Part 2: Linker

This linker gets object files generated from this assembly as input, links them together and outputs a .hex file which represents an executable program. 
It also receives following commands from command line: 
	-o (output file): specifies the ouptut file 
	-hex: shows linker directions how to exactly link files by outuput like this: 
	00000000: 00 01 02 03 04 05 06 07 
	00000008: 08 09 0A 0B 0C 0D 0E 0F 
	00000010: 10 11 12 13 14 15 16 17 
	This instruction is mandatory for executing the linker. 
	-place=section_name@location : places the listed section from the listed location. This command can be used multiple times and every section that is listed with this command is printed by the -hex command.

Part 3: Emulator

Emulator receives a .hex file generated by the linker and emulates the program in it and as ouput shows the processor state (values of general purpouse registers) after the halt instruction. Program counter starts at 0x40000000. Example of the ouput:

Emulated processor executed halt instruction 
Emulated processor state: 
r0=0x00000000   r1=0x00000000  r2=0x00000000  r3=0x00000000 
r4=0x00000000   r5=0x00000000  r6=0x00000000  r7=0x00000000 
r8=0x00000000   r9=0x00000000 r10=0x00000000 r11=0x00000000 
r12=0x00000000 r13=0x00000000 r14=0x00000000 r15=0x00000000

The result of translating these programs is obtained through makefile. Examples of how to run these programs is shown in tests/start.sh script.

