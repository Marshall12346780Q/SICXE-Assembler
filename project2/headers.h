#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void addSymbol(char *newSymbol, int newAddress);
void printTable();
int searchSymbol(char *query);
int isSymbol(char *testSymbol);
int isDirective(char *testSymbol);
char* getOpcode(char *instruction);
char* opcodes[60][2];
