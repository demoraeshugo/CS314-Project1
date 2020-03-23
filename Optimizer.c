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

void findCriticals(Instruction *);
void push(int, int);
void pop(int, int);
int isCritical(int, int);
void print_list();

typedef struct node
{
	int reg;
	int c;
	struct node *next;
} node_t;

node_t *critHead = NULL;

int main()
{
	Instruction *head = NULL, *forward = NULL;

	head = ReadInstructionList(stdin);
	if (!head)
	{
		WARNING("No instructions\n");
		exit(EXIT_FAILURE);
	}

	//First instruction always critical
	head->critical = 1;

	//Jump to end of list
	forward = head;
	while (forward)
	{
		if (forward->next == NULL)
			break;
		forward = forward->next;
	}

	//Search for critical instructions
	findCriticals(forward);

	//Free non critical nodes
	forward = head;
	while (forward)
	{
		if (!forward->critical)
		{

			//Tail
			if (forward->next == NULL)
			{
				forward->prev->next = NULL;
				free(forward);
			}

			//Middle
			else
			{
				forward->prev->next = forward->next;
				forward->next->prev = forward->prev;
				free(forward);
			}
		}

		forward = forward->next;
	}

	//Print optimized instructions
	if (head)
		PrintInstructionList(stdout, head);

	//Free critical list
	node_t *temp = NULL;
	while (critHead)
	{
		temp = critHead;
		critHead = critHead->next;
		free(temp);
	}

	//Free entire instruction list
	while (head)
	{
		forward = head;
		head = head->next;
		free(forward);
	}

	return EXIT_SUCCESS;
}

void findCriticals(Instruction *backward)
{

	while (backward)
	{
		switch (backward->opcode)
		{
		case LOADI:
			if (isCritical(backward->field2, 0))
				backward->critical = 1;
			break;
		case ADD:
		case SUB:
		case MUL:
			if (isCritical(backward->field3, 0))
			{
				backward->critical = 1;
				pop(backward->field3, 0);
				push(backward->field1, 0);
				push(backward->field2, 0);
			}
			break;
		}

		backward = backward->prev;
	}
}

void push(int reg, int c)
{

	if (!critHead)
	{
		critHead = malloc(sizeof(node_t));

		critHead->reg = reg;
		critHead->c = c;

		return;
	}

	node_t *current = critHead;

	while (current->next != NULL)
	{
		if (current->next->reg == reg && current->next->c == c)
			return;
		current = current->next;
	}

	current->next = malloc(sizeof(node_t));
	current->next->reg = reg;
	current->next->c = c;
	current->next->next = NULL;
}

void pop(int reg, int c)
{

	node_t *current = critHead;
	node_t *tempNode = critHead;

	while (current)
	{

		if (current->reg == reg && current->c == c)
		{

			//head
			if (current == critHead)
			{

				critHead = critHead->next;
				free(current);
				return;
			}

			//tail
			else if (current->next == NULL)
			{
				tempNode->next = NULL;
				free(current);
				return;
			}

			//middle
			else
			{
				tempNode->next = current->next;
				free(current);
				return;
			}
		}

		tempNode = current;
		current = current->next;
	}
}

int isCritical(int reg, int c)
{
	node_t *current = critHead;

	while (current)
	{
		if (current->reg == reg && current->c == c)
			return 1;

		current = current->next;
	}

	return 0;
}

void print_list()
{
	node_t *current = critHead;

	while (current)
	{
		fprintf(stdout, "(%d, %d)  ", current->reg, current->c);
		current = current->next;
	}

	fprintf(stdout, "\n");
}
