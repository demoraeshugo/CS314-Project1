#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "InstrUtils.h"
#include "Utils.h"

int main()
{
	Instruction *head;

	head = ReadInstructionList(stdin);
	if (!head) {
		WARNING("No instructions\n");
		exit(EXIT_FAILURE);
	}

    Instruction *current;
    Instruction *previous;
    Instruction *next;

    current = head;
    previous = current->next;
    next = previous->next;

    int first, second, result, check;
    check = 0;

    while (next != NULL) {
        //Check the opcodes
        if ( current->opcode == LOADI && previous->opcode == LOADI &&
                (next->opcode == ADD 
                ||  next->opcode == SUB 
                || next->opcode == MUL
                || next->opcode == AND
                || next->opcode == OR) ){
            check = 1;

            first = current->field2;
            second = previous->field2;

            current->field1 = next->field1;

            if (next->opcode == ADD) {
                result = first+second;
                current->field2 = result;
            }
            else if (next->opcode == SUB) {
                result = first-second;
                current->field2 = result;
            }
            else if (next->opcode == MUL) {
                result = first*second;
                current->field2 = result;
            }
            else if (next->opcode == AND) {
                result = first&second;
                current->field2 = result;
            }
            else if (next->opcode == OR) {
                result = first|second;
                current->field2 = result;
            } else {
                ERROR("Unexpected instruction\n");
                exit(EXIT_FAILURE);
            }
        }

        //Check if there can be optimizations
        if (check == 0) {
            current = current->next;
            previous = previous->next;
            next = next->next;
        } else {
            Instruction *temp1;
            Instruction *temp2;

            temp1 = previous;
            temp2 = next;

            free(temp1);
            free(temp2);

            current->next = next->next;
            current = current->next;
            previous = current->next;
            next = previous->next;

            check = 0;
        }
    }

	if (head) {
		PrintInstructionList(stdout, head);
		DestroyInstructionList(head);
	}

	return EXIT_SUCCESS;
}