/* -------------------------------------------------

            CFG for tinyL LANGUAGE

     PROGRAM ::= STMTLIST !
     STMTLIST ::= STMT MORESTMTS
     MORESTMTS ::= ; STMTLIST | epsilon
     STMT ::= ASSIGN | READ | PRINT
     ASSIGN ::= VAR = EXPR
     READ ::= % VAR
     PRINT ::= $ VAR
     EXPR ::= ARITH_EXPR | 
     	      LOGICAL_EXPR|
     	      VAR | 
              DIGIT
     ARITH_EXPR ::= + EXPR EXPR |
              		- EXPR EXPR |
              		* EXPR EXPR 
     LOGICAL_EXPR ::= | EXPR EXPR |
     				  & EXPR EXPR
              
     VAR ::= a | b | c | d | e | f 
     DIGIT ::= 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9

     NOTE: tokens are exactly a single character long

     Example expressions:

           +12!
           +1b!
           +*34&78!
           -*+1+2a58!

     Example programs;

         %a;%b;c=&3*ab;d=+c1;$d!
         %a;b=|*+1+2a58;$b!

 ---------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "Instr.h"
#include "InstrUtils.h"
#include "Utils.h"

#define MAX_BUFFER_SIZE 500
#define EMPTY_FIELD 0xFFFFF
#define token *buffer

/* GLOBALS */
static char *buffer = NULL;	 /* read buffer */
static int regnum = 1;		 /* for next free virtual register number */
static FILE *outfile = NULL; /* output of code generation */

/* Utilities */
static void CodeGen(OpCode opcode, int field1, int field2, int field3);
static inline void next_token();
static inline int next_register();
static inline int is_digit(char c);
static inline int to_digit(char c);
static inline int is_identifier(char c);
static char *read_input(FILE *f);

/* Routines for recursive descending parser LL(1) */
static void program();
static void stmtlist();
static void morestmts();
static void stmt();
static void assign();
static void read();
static void print();
static int expr();
static int var();
static int digit();
static int arith_expr();
static int logical_expr();

/*************************************************************************/
/* Definitions for recursive descending parser LL(1)                     */
/*
			Class : 							LL
			Direction of Scanning : 			Left-to-Right		
			Derivation Discovered : 			Left-Most
			Parse Tree Construction : 			Top-Down
			Algorithim Used : 					Predictive
																		*/
/*************************************************************************/

//Done
static int digit()
{
	//DIGIT ::= 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9
	int reg;

	if (!is_digit(token))
	{
		ERROR("Expected digit\n");
		exit(EXIT_FAILURE);
	}
	reg = next_register();
	CodeGen(LOADI, to_digit(token), reg, EMPTY_FIELD);
	next_token();
	return reg;
}

//Done
static int var()
{
	/* YOUR CODE GOES HERE */
	//VAR ::= a | b | c | d | e | f
	int reg;

	if (!is_identifier(token))
	{
		ERROR("Expected identfier\n");
		exit(EXIT_FAILURE);
	}
	reg = next_register();
	CodeGen(LOAD, 0, token, reg);
	next_token();
	return reg;
}

//Done
static int expr()
{
	switch (token)
	{
	case '+':
	case '-':
	case '*':
		return arith_expr();
	case '&':
	case '|':
		return logical_expr();
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
		return var();
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		return digit();
	default:
		ERROR("Symbol %c unknown\n", token);
		exit(EXIT_FAILURE);
	}
}

int loadRegisters(OpCode opcode) {
	int reg, leftReg, rightReg;

	next_token();

	leftReg = expr();
	rightReg = expr();
	reg = next_register();

	CodeGen(opcode, leftReg, rightReg, reg);

	return reg;
}

//Done
static int arith_expr()
{
	switch (token)
	{
	case '+':
		return loadRegisters(ADD);
	case '-':
		return loadRegisters(SUB);
	case '*':
		return loadRegisters(MUL);
	default:
		ERROR("Symbol %c unknown\n", token);
		exit(EXIT_FAILURE);
	}
}

//Done
static int logical_expr()
{
	/* YOUR CODE GOES HERE */
	int reg, leftReg, rightReg;

	switch (token)
	{
	case '&':
		next_token();
		leftReg = expr();
		rightReg = expr();
		reg = next_register();
		CodeGen(AND, leftReg, rightReg, reg);
		return reg;
	case '|':
		next_token();
		leftReg = expr();
		rightReg = expr();
		reg = next_register();
		CodeGen(OR, leftReg, rightReg, reg);
		return reg;
	default:
		ERROR("Symbol %c unknown\n", token);
		exit(EXIT_FAILURE);
	}
}

//Done
static void assign()
{
	/* YOUR CODE GOES HERE */
	// ASSIGN ::= VAR = EXPR
	if (!(is_identifier(token)))
	{
		ERROR("Expected indentifier \n");
		exit(EXIT_FAILURE);
	}

	int result;
	next_token();

	if (token != '=')
	{
		ERROR("Expected =\n");
		ERROR("Program error.  Current input symbol is %c\n", token);
		exit(EXIT_FAILURE);
	}

	next_token();
	result = expr();
	CodeGen(STORE, result, 0, EMPTY_FIELD);
}

//Done
static void read()
{
	/* YOUR CODE GOES HERE */
	//READ ::= % VAR
	if (token != '%')
	{
		ERROR("Expected &\n");
		exit(EXIT_FAILURE);
	}

	next_token();

	if (!is_identifier(token))
	{
		ERROR("Expected identifier\n");
		exit(EXIT_FAILURE);
	}

	CodeGen(READ, token, EMPTY_FIELD, EMPTY_FIELD);
	next_token();
}

//Done
static void print()
{
	/* YOUR CODE GOES HERE */
	//PRINT ::= $ VAR
	if (token != '$')
	{
		ERROR("Expected $\n");
		exit(EXIT_FAILURE);
	}

	next_token();

	if (!is_identifier(token))
	{
		ERROR("Expected identifier\n");
		exit(EXIT_FAILURE);
	}

	CodeGen(WRITE, token, EMPTY_FIELD, EMPTY_FIELD);
	next_token();
}

//Done
static void stmt()
{
	/* YOUR CODE GOES HERE */
	//STMT ::= ASSIGN | READ | PRINT
	switch (token)
	{
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
		assign();
		return;
	case '%':
		read();
		return;
	case '$':
		print();
		return;
	default:
		ERROR("Symbol %c unknown\n", token);
		exit(EXIT_FAILURE);
	}
}

//Done
static void morestmts()
{
	/* YOUR CODE GOES HERE */
	//MORESTMTS ::= ; STMTLIST | epsilon
	if (token == '!')
	{
		return;
	}
	else
	{
		if (token != ';')
		{
			ERROR("Program error.  Current input symbol is %c\n", token);
			ERROR("Expected ;\n");
			exit(EXIT_FAILURE);
		}
		else
		{
			next_token();
			stmtlist();
		}
	}
}

//Done
static void stmtlist()
{
	/* YOUR CODE GOES HERE */
	//STMTLIST ::= STMT MORESTMTS
	stmt();
	morestmts();
}

//Done
static void program()
{
	/* YOUR CODE GOES HERE */
	//PROGRAM ::= STMTLIST !
	stmtlist();
	if (token != '!')
	{
		ERROR("Program error: Current input symbol is %c \n", token);
		exit(EXIT_FAILURE);
	}
}

/*************************************************************************/
/* Utility definitions                                                   */
/*************************************************************************/
static void CodeGen(OpCode opcode, int field1, int field2, int field3)
{
	Instruction instr;

	if (!outfile)
	{
		ERROR("File error\n");
		exit(EXIT_FAILURE);
	}
	instr.opcode = opcode;
	instr.field1 = field1;
	instr.field2 = field2;
	instr.field3 = field3;
	PrintInstruction(outfile, &instr);
}

static inline void next_token()
{
	if (*buffer == '\0')
	{
		ERROR("End of program input\n");
		exit(EXIT_FAILURE);
	}
	printf("%c ", *buffer);
	if (*buffer == ';')
		printf("\n");
	buffer++;
	if (*buffer == '\0')
	{
		ERROR("End of program input\n");
		exit(EXIT_FAILURE);
	}
	if (*buffer == '!')
		printf("!\n");
}

static inline int next_register()
{
	return regnum++;
}

static inline int is_digit(char c)
{
	if (c >= '0' && c <= '9')
		return 1;
	return 0;
}

static inline int to_digit(char c)
{
	if (is_digit(c))
		return c - '0';
	WARNING("Non-digit passed to %s, returning zero\n", __func__);
	return 0;
}

static inline int is_identifier(char c)
{
	if (c >= 'a' && c <= 'f')
		return 1;
	return 0;
}

static char *read_input(FILE *f)
{
	size_t size, i;
	char *b;
	int c;

	for (b = NULL, size = 0, i = 0;;)
	{
		if (i >= size)
		{
			size = (size == 0) ? MAX_BUFFER_SIZE : size * 2;
			b = (char *)realloc(b, size * sizeof(char));
			if (!b)
			{
				ERROR("Realloc failed\n");
				exit(EXIT_FAILURE);
			}
		}
		c = fgetc(f);
		if (EOF == c)
		{
			b[i] = '\0';
			break;
		}
		if (isspace(c))
			continue;
		b[i] = c;
		i++;
	}
	return b;
}

/*************************************************************************/
/* Main function                                                         */
/*************************************************************************/

int main(int argc, char *argv[])
{
	const char *outfilename = "tinyL.out";
	char *input;
	FILE *infile;

	printf("------------------------------------------------\n");
	printf("CS314 compiler for tinyL\n");
	printf("------------------------------------------------\n");
	if (argc != 2)
	{
		ERROR("Use of command:\n  compile <tinyL file>\n");
		exit(EXIT_FAILURE);
	}
	infile = fopen(argv[1], "r");
	if (!infile)
	{
		ERROR("Cannot open input file \"%s\"\n", argv[1]);
		exit(EXIT_FAILURE);
	}
	outfile = fopen(outfilename, "w");
	if (!outfile)
	{
		ERROR("Cannot open output file \"%s\"\n", outfilename);
		exit(EXIT_FAILURE);
	}
	input = read_input(infile);
	buffer = input;
	program();
	printf("\nCode written to file \"%s\".\n\n", outfilename);
	free(input);
	fclose(infile);
	fclose(outfile);
	return EXIT_SUCCESS;
}
