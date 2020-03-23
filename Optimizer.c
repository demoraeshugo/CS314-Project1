/*
 *********************************************
 *  314 Principles of Programming Languages  *
 *  Spring 2014                              *
 *  Authors: Ulrich Kremer                   *
 *           Hans Christian Woithe           *
 *********************************************
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "InstrUtils.h"
#include "Utils.h"
#include <stdbool.h>

/*
The dead code elimination optimizer expect the input file to be provided at the standard
input (stdin), and will write the generated code back to standard output (stdout).
*/

/*
The initial crucial instructions are
all READ and WRITE instructions

For all WRITE instruction, the algorithm has to detect
all instructions that contribute to the value of the variable that is written out

First, you
will need to find the store instruction that stores a value into the variable that is written
out. This STORE instruction is marked as critical and will reference a register, let’s say Rm.

Then you will find the instructions on which the computation of the register Rm depends
on and mark them as critical

This marking process terminates when no more instructions
can be marked as critical. 

. If this basic algorithm is performed for all WRITE instructions, the
instructions that were not marked critical can be deleted.
*/

/*

//Go through list of instructions

if READ, mark critical
if WRITE, look for contributors
	find STORE that stores value into the written var
	Mark STORE critical
	STORE->"Rm"
	look for contributors, repeat
If all have been read and no more can be marked, terminate
Instructions not marked cirital can be deleted and freed

Your dead-code eliminator will take a list of RISC instructions as input. For example, in
the following code

Input 				
LOADI Rx #c1
	loadi #ch1 into Rv
LOADI Ry #c2
	loadi #c2 into Ry
LOADI Rz #c3
	loadi #3 into Rx
ADD R1 Rx Ry
	add Rx and Ry and store result in R1
MUL R2 Rx Ry
	multiply Rx and Ry and store in R2
STORE a R1
	store R1 into a
WRITE a
	write a


Output
LOADI Rx #c1
LOADI Ry #c2
ADD R1 Rx Ry
STORE a R1
WRITE a

So starting from the end 
find what write does:
	a
	Find what stores into a
		Store a R1 = critical
			find what stores into R1
			ADD R1 Rx Ry = critical
				find what stores Rx
				find what stores Ry
					LOADI Rx #c1 = critical
					LOADI Ry #c2 = critical
						nothing else so break

	Go through list
		if not critical
			delete instruction
			free memory

Populate Data structure form STDIN
	Which?
Perform dead code elimination (Above)
Write Data structre to file
*/

/*
int main()
{
	Instruction *head;

	head = ReadInstructionList(stdin);
	if (!head) {
		WARNING("No instructions\n");
		exit(EXIT_FAILURE);
	}


	if (head) {
		PrintInstructionList(stdout, head);
		DestroyInstructionList(head);
	}
	return EXIT_SUCCESS;
}
*/

int main()
 {
 	Instruction *head;
 	head = ReadInstructionList(stdin);
 	if (!head) {
 		ERROR("No instructions\n");
 		exit(EXIT_FAILURE);
 	}

	/* YOUR CODE GOES HERE */

 	Instruction *first;
 	Instruction *second;
 	Instruction *third;

	Instruction *instr = head;

	do {
		first = instr;
		second = instr->next;
		third = second->next;
		if (first->opcode == LOADI && second->opcode == LOADI) {
			switch (third->opcode) {
				case ADD:
					if ((first->field1 == third->field2 && second->field1 == third->field3) || (first->field1 == third->field3 && second->field1 == third->field2)) {
						instr->field1 = third->field1;
						instr->field2 = first->field2 + second->field2;
						instr->next = third->next;
						third->next->prev = instr;
						free(second);
						free(third);
						// printf("Found ADD optimization.\n");
					}		
					break;
				case MUL:
					if ((first->field1 == third->field2 && second->field1 == third->field3) || (first->field1 == third->field3 && second->field1 == third->field2)) {
						instr->field1 = third->field1;
						instr->field2 = first->field2 * second->field2;
						instr->next = third->next;
						third->next->prev = instr;
						free(second);
						free(third);
						// printf("Found MUL optimization.\n");
					}		
					break;		
				case SUB:
					if (first->field1 == third->field2 && second->field1 == third->field3) {
						instr->field1 = third->field1;
						instr->field2 = first->field2 - second->field2;
						instr->next = third->next;
						third->next->prev = instr;
						free(second);
						free(third);
						// printf("Found SUB optimization.\n");
					}
					break;
				default:
					// printf("\nNot optimizable.\n");
					break;
			}
		}
		instr = instr->next;
	} while (instr != NULL && instr->next != NULL && instr->next->next != NULL);

 	PrintInstructionList(stdout, head);
 	DestroyInstructionList(head);
 	return EXIT_SUCCESS;
}