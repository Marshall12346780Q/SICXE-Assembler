#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int TACalc(int add);
void removeFirstChar(char c[]);
void addSymbol(char *newSymbol, int newAddress);
void printTable();
int searchSymbol(char *query);
int isSymbol(char *testSymbol);
int isDirective(char *testSymbol);
int getRegisterCode(char* r);
char* getF1Opcode(char* instruction);
char* getF2Opcode(char* instruction);
char* getOpcode(char *instruction);
char* opcodes[80][2];
