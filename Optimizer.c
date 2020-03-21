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


int main()
{
	Instruction *head;

	head = ReadInstructionList(stdin);
	if (!head) {
		WARNING("No instructions\n");
		exit(EXIT_FAILURE);
	}
	/* YOUR CODE GOES HERE */
	Instruction *first;
 	Instruction *second;
 	Instruction *third;

	Instruction *instr = head;

	bool con1;
	bool con2;
	bool con3;
	bool con4;

	do {
		first = instr;
		second = instr->next;
		third = second->next;
		if (first->opcode == LOADI && second->opcode == LOADI) {
			con1 = first->field1 == third->field2;
			con2 = second->field1 == third->field3;
			con3 = first->field1 == third->field3;
			con4 = second->field1 == third->field2;
			switch (third->opcode) {
				case ADD:
					if (( con1 && con2) || ( con3 && con4 )) {
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
					if (( con1 && con2) || ( con3 && con4 )) {
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
					if ( con1 && con2 ) {
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


