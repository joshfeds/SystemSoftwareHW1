// Homework #1 - PM/0-Machine
// COP3402-0002, Fall 2022, Dr. John Aedo
// Group 32 - Jarod Davies, Joshua Federman, Nicholas Rolland, Polina Skvortsova
// Due 10/7/22

// To compile: gcc driver.c lex.o parser.o vm.c
// To execute: ./a.out input.txt -v
// input.txt is any input test file
// Tags: -v prints virtual machine trace, -l prints lexeme list, -s prints symbol table, -c prints assembly code
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compiler.h"

int base(int *stack, int BP, int L);
void print_instruction(int PC, instruction IR);
void print_stack(int PC, int BP, int SP, int *stack);

// From the assignment:
// "The P-machine is a stack machine with two memory areas:
// 		the 'text' section which contains the instructions for the VM to execute,
//		and the 'stack', which is organized as a data-stack to be used by the PM/0 CPU."

// The instruction array code is the "text" section and contains all instructions to be executed.

// Appendix B contains a lot of the info for implementing the execute function
void execute(int trace_flag, instruction *code)
{
	// TODO: Declare int array as the 'stack' section with ARRAY_SIZE and initialize values to 0
	int stack[ARRAY_SIZE];
	for(int i = 0; i < ARRAY_SIZE; i++) {
		stack[i] = 0;
	}
	// TODO: Declare five registers to handle the stack and text segments
	//	BP, SP, PC, halt are ints, IR is an instruction
	int BP = 0, SP = -1, PC = 0, halt = 0;
	instruction IR;

	// P-Machine loops while halt is false
	while (halt == 0)
	{
		// Step 1: Fetch cycle
		IR = code[PC];
		PC++;

		// Step 1.5: Prints instruction first as outlined in Appendix D
		if(trace_flag) {
			if(PC == 1) {
				printf("VM Exectution:\n");
				printf("\t\t\t\tPC\tBP\tSP\tstack\n");
				printf("Initial Values:\t\t\t0\t0\t-1\n");
			}
			print_instruction(PC, IR);
		}

		// Step 2: Execute cycle
		// WRITE SWITCH CASE FOR OPS IN APPENDIX B HERE
		switch (IR.op)
		{
			// Case 1: Pushes a constant value (literal) M onto the stack 
			case LIT:
				SP++;
				stack[SP] = IR.m;
				break;
			//Case 2: Operations that perform on data from the top of the stack
			case OPR:
				switch(IR.m){
					case ADD:
						SP--;
						stack[SP] += stack[SP + 1];
						break;
					case SUB:
						SP--;
						stack[SP] -= stack[SP + 1];
						break;
					case MUL:
						SP--;
						stack[SP] *= stack[SP + 1];
						break;
					case DIV:
						SP--;
						stack[SP] /= stack[SP + 1];
						break;
					case EQL:
						SP--;
						stack[SP] = stack[SP] == stack[SP + 1];
						break;
					case NEQ:
						SP--;
						stack[SP] = stack[SP] != stack[SP + 1];
						break;
					case LSS:
						SP--;
						stack[SP] = stack[SP] < stack[SP + 1];
						break;
					case LEQ:
						SP--;
						stack[SP] = stack[SP] <= stack[SP + 1];
						break;
					case GTR:
						SP--;
						stack[SP] = stack[SP] > stack[SP + 1];
						break;
					default:
						SP--;
						stack[SP] = stack[SP] >= stack[SP + 1];
						break;	 
				}
				break;
			// Case 3: Load value to top of stack from the stack location at offset M from L lexicographical levels down 
			case LOD:
				SP++;
				stack[SP] = stack[base(stack, BP, IR.l) + IR.m];
				break;
			// Case 4: Store value at top of stack in the stack location at offset M from L lexicographical levels down 
			case STO:
				stack[base(stack, BP, IR.l) + IR.m] = stack[SP];
				SP--;
				break;
			// Case 5: Call procedure at code index M (generates new Activation Record and PC ! M) 
			case CAL:
				stack[SP + 1] = base(stack, BP, IR.l);
				stack[SP + 2] = BP;
				stack[SP + 3] = PC;
				BP = SP + 1;
				PC = IR.m;
				break;
			// Case 6: Return from the current procedure. Restores the environment using information from the activation record.
			case RTN:
				SP = BP - 1;
				BP = stack[SP + 2];
				PC = stack[SP + 3];
				break;
			// Cases 7-10 by nick
			// Case 7: Allocate M memory words in the stack, and increase SP respectively.
			case INC:
				SP = SP + IR.m;
				break;
			// Case 8: Jumps / moves the PC to instruction M
			case JMP:
				PC = IR.m;
				break;
			// Case 9: Jump to instruction M if top of stack is 0
			case JPC:
				if (stack[SP] == 0)
					PC = IR.m;
				SP = SP - 1;
				break;
			// Case 10: Perform a system operation.
			case SYS:
				switch (IR.m)
				{
					// Write to the console.
					case WRT:
						printf("\nOutput : %d", stack[SP]);
						printf("\n\t\t\t\t");
						SP = SP - 1;
						break;
					// Read input from the console.
					case RED:
						SP = SP + 1;
						printf("\nInput : ");
						scanf("%d", &stack[SP]);
						printf("\t\t\t\t");
						break;
					// Halt the program.
					case HLT:
						halt = 1;
						break;
				}
		}

		// Step 3: Print the current step
		if (trace_flag)
		{
			print_stack(PC, BP, SP, stack);
		}
	}
	// From a message in #cop3402_aedo:
	// trace_flag only determines whether the info is printed, and is separate from halt

	

	
}

int base(int *stack, int BP, int L)
{
	while (L > 0)
	{
		BP = stack[BP];
		L--;
	}
	return BP;
}


void print_stack(int PC, int BP, int SP, int *stack)
{
	int i;
	printf("%d\t%d\t%d\t", PC, BP, SP);
	for (i = 0; i <= SP; i++)
		printf("%d ", stack[i]);
	printf("\n");
}

void print_instruction(int PC, instruction IR)
{
	char opname[4];
	
	switch (IR.op)
	{
		case LIT : strcpy(opname, "LIT"); break;
		case OPR :
			switch (IR.m)
			{
				case ADD : strcpy(opname, "ADD"); break;
				case SUB : strcpy(opname, "SUB"); break;
				case MUL : strcpy(opname, "MUL"); break;
				case DIV : strcpy(opname, "DIV"); break;
				case EQL : strcpy(opname, "EQL"); break;
				case NEQ : strcpy(opname, "NEQ"); break;
				case LSS : strcpy(opname, "LSS"); break;
				case LEQ : strcpy(opname, "LEQ"); break;
				case GTR : strcpy(opname, "GTR"); break;
				case GEQ : strcpy(opname, "GEQ"); break;
				default : strcpy(opname, "err"); break;
			}
			break;
		case LOD : strcpy(opname, "LOD"); break;
		case STO : strcpy(opname, "STO"); break;
		case CAL : strcpy(opname, "CAL"); break;
		case RTN : strcpy(opname, "RTN"); break;
		case INC : strcpy(opname, "INC"); break;
		case JMP : strcpy(opname, "JMP"); break;
		case JPC : strcpy(opname, "JPC"); break;
		case SYS : 
			switch (IR.m)
			{
				case WRT : strcpy(opname, "WRT"); break;
				case RED : strcpy(opname, "RED"); break;
				case HLT : strcpy(opname, "HLT"); break;
				default : strcpy(opname, "err"); break;
			}
			break;
		default : strcpy(opname, "err"); break;
	}
	
	printf("%d\t%s\t%d\t%d\t", PC - 1, opname, IR.l, IR.m);
}
