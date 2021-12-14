#include "headers.h" 


int b = 0; /*storing the contents of the base register is necessary for base-relative addressing*/
int main(int argc, char* argv[]) {
	FILE* fp;
	int adds[1000];
	char formats[1000]; /*flag formats of instructions*/
	memset(formats, 0, 1000 * sizeof(char));
	char line[1024];
	char* symbol;
	char name[7];
	int startLine = 0;
	long address = 0;/*stored as decimal, printed as hex*/
	int lCount = 0;
	int ERRORflag = 0; /*Switched to 1 if there is an error. Program will display no output other than error message*/
	if (argc != 2) {
		printf("ERROR: Usage: %s filename\n", argv[0]);
		return 0;
	}
	fp = fopen(argv[1], "r");/*fopen returns NULL if it cant find the file or if you don't have permission*/
	if (fp == NULL) {
		printf("ERROR: %s could not be opened for reading\n", argv[1]);
		return 0;
	}
	while (fgets(line, 1024, fp) != NULL)/*read line by line until end of file*/
	{
		lCount++; /*detect more errors? check against test files*/
		if (address > 1048576)
		{
			printf("ERROR: MEMORY OVERFLOW- SICXE supports up to 1MB of memory, or 100,000 in hex, %d", address);
			ERRORflag = 1;
		}
		adds[lCount] = address;
		symbol = strtok(line, " \t\n\r");
		if (line[0] == 35)/*If its a comment, don't do anything*/
		{
			/*printf("Comment detected:%s\n", line);*/
		}
		else {/*if it's not a comment */ 
			if (!(line[0] == ' ' || line[0] == '\t')) /*if it's not blank, there's a token in field 1 and strtok should parse*/
			{
				if (isSymbol(symbol) == 0 || isDirective(symbol) > 0) /*Check for invalid symbol names*/
				{
					printf("ASSEMBLY ERROR:\nLine:%d Invalid Symbol Name. Must be no more than 6 letters, all capitals, and not the name of a directive.\n", lCount);
					ERRORflag = 1;
					fclose(fp);
				}
				if (searchSymbol(symbol) != -1) /*Check if symbol has been used before*/
				{
					printf("Error: Duplicate Symbol %s on line %d", symbol, lCount);
					ERRORflag = 1;
				}
				if (startLine == 0) /*program hasnt started yet*/
				{
					strcpy(name, symbol);  /*name may temporarily hold values other than the actual name, but it remains as such when it encounters START directive*/
				}
				else
				{
					addSymbol(symbol, address);
				}
				symbol = strtok(NULL, " \t\n\r");/*grab the next token, advancing to field 2*/
			}
			int dStat = isDirective(symbol);
			if (dStat >= 1)
			{
				if (dStat == 1)/*START*/
				{
					/*if (!(lCount == 1))
					{
						printf("Error: START directive must be used on first line only");
					}*/

					char* useless;
					address = strtol(strtok(NULL, " \t\n\r"), &useless, 16); /*read in the address as base 16 and store it in address*/
					addSymbol(name, address); 
					startLine = lCount;
					adds[startLine] = address;
				}
				else if (dStat == 2)
				{

				}
				else if (dStat == 3)
				{/*generate char or hex constant using as many bytes as needed*/
					symbol = strtok(NULL, " \t\n\r");
					if (symbol[0] == 'C')
					{
						int i = 2;
						while (symbol[i] != 39)
						{
							i++;
						}
						address = address + (i - 2);
					}
					else if (symbol[0] == 'X')
					{/*every char adds half a byte round up*/
						int i = 2;
						while (symbol[i] != 39)
						{
							if (!((symbol[i] > 64 && symbol[i] < 71) || (symbol[i] > 47 && symbol[i] < 58)))
							{
								printf("ERROR invalid hex character %d on line %d", symbol[i], lCount);
								ERRORflag = 1;
							}
							i++;
						}
						address = address + (int)((i - 2.0) / 2.0) + .99;
					}
					else
					{
						printf("Error line %d: Invalid BYTE parameter; constant should be preceded by 'C' or 'X'", lCount);
						ERRORflag = 1;
					}
				}
				else if (dStat == 4)
				{
					address = address + 3;
				}
				else if (dStat == 5)
				{
					address = address + atoi(strtok(NULL, " '\t\n"));
				}
				else if (dStat == 6)
				{
					address = address + (3 * (atoi(strtok(NULL, " '\t\n"))));
				}
				else if (dStat == 7)
				{
					address = address + 3;
				}
				else if (dStat == 8)
				{
					address = address + 3;
				}
				else if (dStat == 9)/*the BASE directive takes up no space, but assigns a value to register B that should be noted for base addressing*/
				{
					
				}

			}
			else if (!((strcmp(getF1Opcode(symbol), "00")) == 0))/*Format 1*/
			{
				/*printf("\nFormat 1 instruction: %s Opcode: %s", symbol, getF1Opcode(symbol));*/
				address = address + 1;
				formats[lCount] = 1; 
			}
			else if (!((strcmp(getF2Opcode(symbol), "00")) == 0))/*Format 2*/
			{
				/*printf("\nFormat 2 instruction: %s Opcode: %s", symbol, getF2Opcode(symbol));*/
				address = address + 2;
				formats[lCount] = 2;
			}
			else if (symbol[0] == 43)/*A plus sign is detected, indicating this is a format 4*/
			{
				int i = 1;
				do  /*remove the + from the symbol, could probably be done more cleanly*/
				{
					/*printf("\n%c replaced with %c\n", symbol[i - 1], symbol[i]);*/
					symbol[i - 1] = symbol[i];
					i++;
				} while (symbol[i-1] != '\0');
				symbol[i] = '\0';
				if (getOpcode(symbol) == "0")
				{
					printf("Invalid Instruction:'%s' on line: %d\n", symbol, lCount);
					ERRORflag = 1;
				}
				/*printf("\nFormat 4 instruction: %s Opcode: %s", symbol, getOpcode(symbol));*/
				address = address + 4;
				formats[lCount] = 4;
			}
			else if (!(getOpcode(symbol) == "0"))/*Valid format 3*/
			{
				/*printf("\nFormat 3 instruction: %s Opcode: %s", symbol, getOpcode(symbol));*/
				address = address + 3;
				formats[lCount] = 3;
			}
			else /*Invalid Instruction*/
			{
				printf("Invalid Instruction:'%s' on line: %d\n", symbol, lCount);
				ERRORflag = 1;
			}
			/*printf("line:%d address: %X\n", lCount, address);*/
			/*memset(line, '\0', 1024*sizeof(char));
			memset(symbol, '\0', 1024*sizeof(char));*/
		}
		while (symbol != NULL) /*this isn't actually necessary*/
		{
			symbol = strtok(NULL, " \t\n\r"); /*flush strtok of the entire line*/
		}
		/*wipe line and symbol before running again. /
		/*memset(line, '\0', 1024 * sizeof(char));
		memset(symbol, '\0', 7 * sizeof(char));*/
	}
	fclose(fp); /*PASS 1 COMPLETE Make sure all error cases close properly*/
	if (ERRORflag == 1) { return -1; }/*maybe I shouldn't do this; errors should just quit program immediately?*/
	lCount = 0;
	fp = fopen(argv[1], "r"); 
	char filename[100];
	strcpy(filename, argv[1]); /*record the filename before .obj is added*/
	/*Test for SIC or SICXE. If SIC then it needs to do old pass 2*/
	int fLength = strlen(filename);
	if (filename[fLength - 3] == 's' && filename[fLength - 2] == 'i' && filename[fLength - 1] == 'c')
	{/*detect if non-xe .sic file*/
		printf("normie sic detected\n");
		fLength = -1; /*flag to indicate basic .sic*/
	}
	FILE* p2 = fopen(strcat(argv[1], ".obj"), "w"); /*open file for writing*/ 
	int recordLength;
	memset(line, '\0', 1024 * sizeof(char)); 
	char* field3; 
	char charBuffer[30];
	char wordBuffer[30];
	while (fgets(line, 1024, fp) != NULL)
	{
		lCount++;
		recordLength = 3;
		if (line[0] == 35) /*If its a comment, don't do anything*/
		{
			continue;
		}
		symbol = strtok(line, " \t\n\r");
		if (!((line[0] == ' ') || (line[0] == '\t'))) /*If field 1 is not empty, we'll need to advance to the next token to be sure we're getting field 2*/
		{
			symbol = strtok(NULL, " \t\n\r");
		}
		int dResult = isDirective(symbol);
		/*printf("line: %d format %d\n", lCount, formats[lCount]);*/
		if (dResult > 0)
		{
			symbol = strtok(NULL, " \t\n\r");/*advance to field 3*/
			if (dResult == 1) /*START*/
			{
				/*do nothing, this is simply used to prevent adds index from becoming 0*/
			}
			else if (dResult == 3) /*BYTE*/
			{
				int val = 0; /*hex ASCII number*/
				int i = 2;
				if (symbol[0] == 'C')
				{
					while (symbol[i] != 39)/*hex ASCII value of each character*/
					{
						sprintf(charBuffer, "%.2X", symbol[i++]);
						strcat(wordBuffer, charBuffer);
						recordLength++;
					}
				}
				else if (symbol[0] == 'X')/*hex directly*/
				{
					while (symbol[i] != 39)
					{
						sprintf(charBuffer, "%C", symbol[i++]);
						strcat(wordBuffer, charBuffer);
						if (i % 2 == 0)
						{
							recordLength++;
						}
					}
				}
				adds[lCount] = -adds[lCount]; /*invert value to indicate it's a directive in need of printing*/
			}
			else if (dResult == 4) /*WORD*/
			{
				sprintf(wordBuffer, "%.6X", atoi(symbol));/*should be replaced with strtol, apparently*/
				recordLength = recordLength + 3;
				adds[lCount] = -adds[lCount]; /*invert value to indicate it's a directive in need of printing*/
			}
			else if (dResult == 9)/*BASE*/
			{/*sets register b to the address of the symbol*/
				b = searchSymbol(symbol);
				/*printf("BASE recognized, stored: %X", b);*/
			}
			else /*one of the RES commands which needs to be flagged as a no print*/
			{
				adds[lCount] = 0;
			}
		}
		else /*Instruction*/
		{
			/*printf("\nT%.6X ", adds[lCount]);
			printf("  %s",getOpcode(symbol));*/
			field3 = strtok(NULL, " ,\t\n\r");
			if (formats[lCount] == 1)/*Format1 just prints it's opcode, which is tested and found with a method from symbols*/
			{
				fprintf(p2, "\nT%X1", adds[lCount]);
				fprintf(p2, "%2s", getF1Opcode(symbol));
			}
			else if (formats[lCount] == 2)/*Format 2 instructions simply print their opcode followed by the register number of each register*/
			{
				fprintf(p2, "\nT%X2", adds[lCount]);
				int r1Code = getRegisterCode(field3);
				char* field4 = strtok(NULL, " ,\t\n\r"); /*must be stored for error checking reasons. These checks should really be in pass1 but w/e for now*/
				int r2Code = getRegisterCode(field4); /*hopefully this can pass null and be fine*/
				if (r1Code == -1) { printf("Error: Invalid register name '%s', on line %d", field3, lCount); }
				if (r2Code == -1) { printf("Error: Invalid register name '%s', on line %d", field4, lCount); }
				/*should print the corresponding register numbers according to the specification. no r2 means its 0 according to example*/
				fprintf(p2, "%s%d%d", getF2Opcode(symbol), r1Code, r2Code);
			}
			else if (formats[lCount] == 3)/*Format 3 does some stuff based on size of operand, the flags, and the decision the assembler must make between PC and Base relative*/
			{/*Note that format 3 always prints the displacement in its final 12 bit field as opposed to 4's 20 bit*/
				fprintf(p2, "\nT%X3", adds[lCount]);
				int pOp = strtol(getOpcode(symbol), NULL, 16); /*print Opcode, as an int for flagging*/
				if (field3 == NULL) /*catch the Null case; an instruction with no operands*/
				{
					printf("Instruction %s with no operand %d\n", symbol, lCount);
					fprintf(p2, "%.2X%.4X", pOp, 0);
					/*return 0;*/
				}
				else
				{
				/*check if base .sic*/
					char* indexed = strtok(NULL, " ,\t\n\r");
					if (indexed != NULL) /*Check for Null first to prevent a segFault. Note that there are no two operand format 3's. A comma will always indicate indexed here*/
					{
					if (strcmp("X", indexed) == 0)/*INDEXED: look up symbol address from table, flip n&i x&e bits*/
					{
						int pAdd;
						if (fLength == -1)
						{
							printf("INDEXED SIC instrution line %d\n", lCount);
							pAdd = searchSymbol(field3) - adds[lCount];
						}
						else 
						{
							TACalc(field3, adds[lCount]);
							pOp += 3;
							pAdd = strtol(field3, NULL, 16);
						}
						pAdd ^= 0x8000;
						fprintf(p2, "%X%.4X", pOp, pAdd);
					}
					}
					else if (fLength == -1)/*SIC instruction: flip no bits just print as in pass2*/
					{
						printf("SIC instruction line %d\n", lCount);
						fprintf(p2, "%.2X%X",pOp,searchSymbol(field3)-adds[lCount]);
					}
					else if (field3[0] == 64)/*INDIRECT: remove @, lookup symbol address from table, TA Calc, flag n*/
				{
					removeFirstChar(field3);
					TACalc(field3, adds[lCount]);
					fprintf(p2, "%.2X%s", pOp + 2, field3);
				}
					else if (field3[0] == 35)/*IMMEDIATE: remove #, read symbol as address in decimal,*/
				{
					removeFirstChar(field3);
					TACalc(field3, adds[lCount]);
					fprintf(p2, "%.2X%s", pOp + 1, field3);
				}
					else/*SIMPLE: look up symbol address from table, flip n&i.*/
				{
					TACalc(field3, adds[lCount]);/*can also return as int for standard flagging process*/
					fprintf(p2, "%.2X%s", pOp + 3, field3);
				}
				}
			}
			else if (formats[lCount] == 4)/*Format 4 is always indicated by a plus, and then no flag, trailing X, @, or #*/
			{
				removeFirstChar(symbol);
				fprintf(p2, "\nT%X4", adds[lCount]);
				int pOp = strtol(getOpcode(symbol), NULL, 16);
				char* indexed = strtok(NULL, " ,\t\n\r");
				if (indexed != NULL) /*Check for Null first to prevent a segFault. Note that there are no two operand format 4's. A comma will always indicate indexed here*/
				{/*does this create logic errors with the following cases? probably not.. but it may exclude cases where the next symbol is uhhh idk*/
					if (strcmp("X", indexed) == 0)/*INDEXED: look up symbol address from table, flip n&i x&e bits*/
					{
						int pAdd = searchSymbol(field3);
						pAdd ^= 0x800000; /*x*/
						pAdd ^= 0x100000; /*e*/
						fprintf(p2, "%X%.6X", pOp + 3, pAdd); /*adding 3 to first byte flips n and i*/
					}
				}
				else if (field3[0] == 64)/*INDIRECT: remove @, lookup symbol address from table, flip n&e*/
				{
					removeFirstChar(field3);
					int pAdd = searchSymbol(field3);
					pAdd ^= 0x100000;
					fprintf(p2, "%X%.6X", pOp + 1, pAdd);

				}
				else if (field3[0] == 35)/*IMMEDIATE: remove #, read symbol as address in decimal, flip i&e bit*/
				{
					removeFirstChar(field3);
					int pAdd = atoi(field3);/*returns 0 if it's not an int, but might actually want constant 0*/
					pAdd ^= 0x100000; /*getOpcode is a string in hex, field3 is a string in decimal*/
					fprintf(p2, "%X%.6X", pOp + 1, pAdd); /*adding 1 flips the i bit in 1st byte, eMask flips e in remaining 3bytes which contains address*/
					/*printf("address BE: %X", atoi(field3));*/
				}
				else/*SIMPLE: look up symbol address from table, flip n&i&e*/
				{
					int pAdd = searchSymbol(field3);
					pAdd ^= 0x100000;
					fprintf(p2, "%X%.6X", pOp + 3, pAdd);/*adding 3 flips both n and i appropriate, eMask flips e*/
				}/*these options could definitely have a method for less repeated code*/
			}
		}
		if (lCount == startLine) /*START line*/
		{
			fprintf(p2, "H%s  %.6X%.6X", name, adds[startLine], address - adds[startLine]);
		}
		else if (adds[lCount] < 0)/*if its a directive in need of printing; BYTE and WORD*/
		{
			fprintf(p2, "\nT%.6X%.2d%s", -adds[lCount], recordLength, wordBuffer); /*this is convention when not combining lines*/
			memset(wordBuffer, '\0', 30 * sizeof(char)); /*clear wordBuffer*/
		}
	}
	/*M records, skip lines flagged as uneccessary by -1*/
	/*M records are now only needed by format 4 instructions*/
	for (int k = startLine+1; k < lCount; k++)
	{
		if (formats[k] == 4)
		{
			fprintf(p2,"\nM%.6X04+%s", adds[k] + 1, name);
			/*printf(" for line %d", k);*/
		}
	}
	/*E Record*/
	fprintf(p2,"\nE%.6X\n",adds[startLine]);
	/*fprintf(p2,"T%X %X %c %s%X", adds[i],3, getOpcode("JLT"),searchSymbol("SYMNAME"));*/
	fclose(fp);
	fclose(p2);
}
int TACalc(char* f3, int currentAddress)/*Target Address calculator from field3, changes f3 in calling function*/
{
	int iConst = atoi(f3);/*atoi returns 0 if f3 is not an int at all (is a symbol, which is quite common)*/
	int add;
	if (iConst <= 4095 && iConst > 0) /*the c case, where field3 is a constant less than or eq 4095*/
	{
		sprintf(f3,"%4X", iConst);
	}
	else /*the m case where field3 is a symbol or a number greater than 4095*/
	{
		if (iConst == 0){add = searchSymbol(f3);}
		else{add = iConst;}
		int PCdisp = add - (currentAddress + 3);/*we know its a format 3 instruction so we can add 3 bytes to get PC, example supports this even for directives on next line*/
		int baseDisp = add - b;
		if (PCdisp >= -2048 && PCdisp <= 2047)
		{
			/*printf("PC-relative case. PC value: %X address: %X disp: %X", currentAddress+3, add, PCdisp);*/
			PCdisp = PCdisp & 0xFFF;
			PCdisp ^= 0x2000;
			sprintf(f3, "%4X", PCdisp);/*AND operation with FFF = 1111 1111 1111 to zero out unneeded bits*/
		}
		else if (baseDisp >= 0 && baseDisp <= 4095)/*else if BASE+4095 can reach the address, use BASE*/
		{
			baseDisp = baseDisp & 0xFFF;
			baseDisp ^= 0x4000;
			sprintf(f3, "%4X", baseDisp);
		}
		else /*Can't be reached*/
		{
			printf("Memory address %X can't be reached. Use a format 4 instruction or a different BASE register value",add);
			return -1;
		}
	}
}