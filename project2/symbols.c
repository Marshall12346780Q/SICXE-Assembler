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
	{"WD", "DC"},
	{"\0","\0"}
};	

char* getOpcode(char* instruction){ /*Accepts instruction, returns opcode or "0" if not found*/
	int i = 0;/*can be improved by converting string to a number value so it only has to compare once and no strcmp, use binarySearch etc etc*/
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

void addSymbol(char *newSymbol, int newAddress)/*insert new node at beginning, we stackin*/
{
	struct symNode* newNode = (struct symNode*)malloc(sizeof(struct symNode));/*only stores the space for one addition, damnit*/
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

void printTable()/*currently prints the stuff in reverse order but who actually cares mate*/
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
