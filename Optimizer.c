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

void findCriticals(Instruction *);
void push(int, int);
void pop(int, int);
int isCritical(int, int);
void print_list();

/*
 * Linked list for holding critical registers
 */
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

/*
 * Starts at the end of the instruction list and iteratively traverses the list
 * backwards examining each instruction type and determining whether that
 * instruction should be marked as critical or not
 */
void findCriticals(Instruction *backward)
{

	while (backward)
	{
		switch (backward->opcode)
		{
		case LOADI:
			if (isCritical(backward->field2, 0))
			{
				backward->critical = 1;
			}
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

		case LOAD:
		case STORE:
		case OR:
		case AND:
		case READ:
		case WRITE:
			break;

			backward = backward->prev;
		}
	}
}

	/*
 * Push critical register into list
 */
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

	/*
 * Remove critical register from list
 */
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

	/*
 * Searches through list of critical registers and returns if register
 * in parameter appears
 */
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

	/*
 * Prints list of critical registers
 */
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
