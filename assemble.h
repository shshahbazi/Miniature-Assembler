#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define NORTYPE 5
#define NOITYPE 8
#define NOJTYPE 2
#define RTYPE 0
#define ITYPE 1
#define JTYPE 2
struct symbolTable
{
	int value;
	char* symbol;
};
struct instruction
{
	size_t instType; /*0 -> R-format  1 -> I-format  2 -> J-format*/
	size_t intInst;
	char* mnemonic;
	char inst[9];
	int rs;
	int rt;
	int rd;
	int imm;
	int pc;
};
int findSymTabLen(FILE*);
int fillSymTab(struct symbolTable*, FILE*);
int hex2int(char*);
void int2hex16(char*, int);