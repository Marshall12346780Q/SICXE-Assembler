#include "headers.h"
/*A simple linked list implementation with structures with an add and search method. Note that the parameters are passed by reference and so there would be a data leak if
 * variables were changed here*/
char* opcodes[][2] = {
	{"ADD", "18"},
	{"ADDF", "58"},
	{"ADDR", "90"},
	{"AND", "40"},
	{"CLEAR", "B4"},
	{"COMP", "28"},
	{"COMPF", "88"},
	{"COMPR", "A0"},
	{"DIV", "24"},
	{"DIVF", "64"},
	{"DIVR", "9C"},
	{"FIX", "C4"},
	{"FLOAT", "C0"},
	{"HIO", "F4"},
	{"J", "3C"},
	{"JEQ", "30"},
	{"JGT", "34"},
	{"JLT", "38"},
	{"JSUB","48"},
	{"LDA", "00"},
	{"LDB", "68"},
	{"LDCH", "50"},
	{"LDF", "70"},
	{"LDL","08"},
	{"LDS", "6C"},
	{"LDT", "74"},
	{"LDX", "04"},
	{"LPS", "D0"},
	{"MUL", "20"},
	{"MULF", "60"},
	{"MULR", "98"},
	{"NORM", "C8"},
	{"OR","44"},
	{"RD","D8"},
	{"RMO", "AC"},
	{"RSUB", "4C"},
	{"SHIFTL", "A4"},
	{"SHIFTR","A8"},
	{"SIO","FO"},
	{"SSK", "EC"},
	{"STA","OC"},
	{"STB","78"},
	{"STCH","54"},
	{"STF","80"},
	{"STI","D4"},
	{"STL","14"},
	{"STS","7C"},
	{"STSW", "E8"},
	{"STT", "84"},
	{"STX", "10"},
	{"SUB","1C"},
	{"SUBF","5C"},
	{"SUBR","94"},
	{"SVC", "B0"},
	{"TD", "E0"},
	{"TIO","F8"},
	{"TIX", "2C"},
	{"TIXR", "B8"},
	{"WD", "DC"},/*below here is XE (format 3/4)*/
	{"ADDF","58"},
	{"COMPF", "28"},
	{"DIVF", "64"},
	{"LDB", "68"},
	{"LDF", "70"},
	{"LDS", "6C"},
	{"LDT", "74"},
	{"LPS", "D0"},
	{"MULF", "60"},
	{"SSK", "EC"},
	{"STB", "78"},
	{"STF", "80"},
	{"STI", "D4"},
	{"STS", "7C"},
	{"STT", "84"},
	{"SUBF", "5C"},
	{"\0","\0"}
};	



void TACalc(int add)/*Target Address calculator from address in assembly*/
{
	if (add <= 4095) /*'c' case*/
	{
		return add; /*displacement is difference between current and target, I'm pretty sure, maybe?*/
	}
	else/*'m' case*/
	{
		/*if disp is between -2048 and 2047 inclusive, PC relative is used*/

		/*if disp is between 0 and 4095 inclusive, base relative is used*/
	}
}
void removeFirstChar(char c[])/*Removes the first character of a char array in it's calling function (pass by ref)*/
{
	int i = 0;
	while (c[i] != '\0') 
	{
		c[i] = c[i + 1];
		i++;
	}
}
int getRegisterCode(char* r) /*returns the int value corresponding to the register name, or -1 if not found*/
{/*I thought about simply using the first char since this is sufficient to distinguish, but I need to be able to check for invalid register names*/
	int result = -1;
	if (r == NULL) { result = 0; } /*this seems a bit weird but it follows example. Potential error uncaught:when leaving out second register of a two-register instruction*/
	else if (strcmp("A", r) == 0) { result = 0; }
	else if (strcmp("X", r) == 0) { result = 1; }
	else if (strcmp("L", r) == 0) { result = 2; }
	else if (strcmp("B", r) == 0) { result = 3; }
	else if (strcmp("S", r) == 0) { result = 4; }
	else if (strcmp("T", r) == 0) { result = 5; }
	else if (strcmp("F", r) == 0) { result = 6; }
	else if (strcmp("PC", r) == 0) { result = 8; }
	else if (strcmp("SW", r) == 0) { result = 9; }
	return result;
}
char* getF1Opcode(char* instruction) {/*If instruction is of format 1, returns opcode, otherwise 0*/
	/*it will have a newline character?*/
	char* result = "00";
	/*printf("\nchecking value for f1: |%d|", instruction[3]); a little ghost hunting, son of a bitch*/
	if (strcmp("FIX", instruction) == 0){result = "C4"; }
	else if (strcmp("FLOAT", instruction) == 0) { result = "C0"; }
	else if (strcmp("HIO", instruction) == 0) { result = "F4"; }
	else if (strcmp("NORM", instruction) == 0) { result = "C8"; }
	else if (strcmp("SIO", instruction) == 0) { result = "FO"; }
	else if (strcmp("TIO", instruction) == 0) { result = "F8"; }
	return result;
}
char* getF2Opcode(char* instruction) {/*If instruction is of format 2, returns opcode, otherwise 0*/
	char* result = "00";
	if (strcmp("ADDR", instruction) == 0) { result = "90"; }
	else if (strcmp("CLEAR", instruction) == 0) { result = "B4"; }
	else if (strcmp("COMPR", instruction) == 0) { result = "A0"; }
	else if (strcmp("DIVR", instruction) == 0) { result = "9C"; }
	else if (strcmp("MULR", instruction) == 0) { result = "98"; }
	else if (strcmp("RMO", instruction) == 0) { result = "AC"; }
	else if (strcmp("SHIFTL", instruction) == 0) { result = "A4"; }
	else if (strcmp("SHIFTR", instruction) == 0) { result = "A8"; }
	else if (strcmp("SUBR", instruction) == 0) { result = "94"; }
	else if (strcmp("SVC", instruction) == 0) { result = "B0"; }
	else if (strcmp("TIXR", instruction) == 0) { result = "B8"; }
	return result;
}

char* getOpcode(char* instruction){ /*Accepts instruction, returns opcode or "0" if not found*/
	int i = 0;/*can be improved by converting string to a number value so it only has to compare once and no strcmp, use binarySearch etc etc*/
	/*I think this generated a segFault once under current implementation and then never again.. watch out for this*/
	while(!(strcmp(opcodes[i][0],instruction) == 0))
	{
		i++;
		if(strcmp(opcodes[i][0], "\0") == 0)
		{
			return "0";
		}
	}
	return opcodes[i][1];
}

struct symNode {
	char* symbol;
	int address;
	struct symNode* next;
};
struct symNode* first = NULL;

void addSymbol(char *newSymbol, int newAddress)/*insert new node at beginning, a stack*/
{
	struct symNode* newNode = (struct symNode*)malloc(sizeof(struct symNode));/*only stores the space for one addition*/
	newNode->symbol = malloc(strlen(newSymbol)); //+1? i guess?
	strcpy(newNode->symbol, newSymbol);
	newNode->address = newAddress;
	newNode->next = first;
	first = newNode;
	/*printf("\nsucessfully added %s %X\n", first->symbol,first->address);
	if(first->next !=NULL)
	{
		printf("previous entry is %s %X\n", first->next->symbol, first->next->address);
	}*/
	return;
}

void printTable()/*print symbol table in reverse order*/
{
	struct symNode* iterator = first;
	while (iterator)
	{
		printf("%s %X\n", iterator->symbol, iterator->address);
		iterator = iterator->next;
	}
	return;

}

int searchSymbol(char *query)/*Searches for a given symbol by name and returns the address, or -1 if it's not present*/
{
	int offset = 0;
	if(query == NULL)
	{
		return 0;
	}
	strcat(query, "\0");
	for(int i = 0; i <10; i++)
	{
		if(query[i] == ',')
		{
			offset = 32768;
			query[i] = '\0';
		}
	}
	/*support for ,X at the end, which adds 8k hex to address*/
	/*printf("\nsearching for symbol %s ", query);*/
	int result = -1;
	struct symNode* current = first;
	while(current)
	{
		if(strcmp(current->symbol, query) == 0)
		{
			return current->address+offset;
		}
		current = current->next;
	}	
	return result;
}

int isSymbol(char *testSymbol) /*Returns 0 if invalid Symbol, 1 if valid*/
{
	int i = 0;
	int result = 0;
	int done = 0;
	/*Segmentation fault means you accessed memory that wasn't yours, this usually is caused by trying to read a string that is not properly null terminated*/	
	while(done != 1)
	{
		if (testSymbol[i] == '\0' || testSymbol[i] == 0 || testSymbol[i] == '\t')/*if its the end, i think*/
		{
			result = 1;
			done = 1;
		}
		else if ((testSymbol[i] < 65) || (testSymbol[i] > 90))/*if its not a capital letter*/
		{
			printf("char '%c' is not capital\n", testSymbol[0]);
			done = 1;
		}
		else if (i > 5)
		{
			printf("more than 6 letters\n");
			done = 1;
		}
		else
		{
			i++;
		}
	}
	return result;
}
int isDirective(char *testString) /*0 if not directive, 1-8 if corresponding directive*/
{
	int result = 0;
	if(strcmp(testString, "START")==0){result = 1;}
	else if(strcmp(testString, "END")==0){result = 2;}
	else if(strcmp(testString, "BYTE")==0){result = 3;}
	else if(strcmp(testString, "WORD")==0){result = 4;}
	else if(strcmp(testString, "RESB")==0){result = 5;}
	else if(strcmp(testString, "RESW")==0){result = 6;}
	else if(strcmp(testString, "RESR")==0){result = 7;}
	else if(strcmp(testString, "EXPORTS")==0){result = 8;}
	return result;
}
