#include "headers.h" 

int main(int argc, char* argv[]) {
	FILE* fp;
	int adds[1000];
	char line[1024];
	char* symbol;
	char starter[7];
	long startAddress = 0;
	long address = 0;/*stored as decimal, printed as hex*/
	int lineCount = 0;
	char startFlag = 0;
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
		if (address > 32768)
		{
			printf("ERROR: MEMORY OVERFLOW- SIC supports up to 32K of memory, or 8000 in hex");
			ERRORflag = 1;
		}
		/*printf("line:%d address: %X\n", lineCount, address);*/
		adds[lineCount] = address;
		symbol = strtok(line, " \t\n");
		if (line[0] == 35)/*If its a comment, don't do anything*/
		{
			/*printf("Comment detected:%s\n", line);*/
		}
		else {/*if it's not a comment */
			lineCount++; 
			if (!(line[0] == ' ' || line[0] == '\t')) /*if it's not blank, strtok should parse*/
			{
				if (isSymbol(symbol) == 0 || isDirective(symbol) > 0) /*Check for invalid symbol names*/
				{
					printf("ASSEMBLY ERROR:\nLine:%d Invalid Symbol Name. Must be no more than 6 letters, all capitals, and not the name of a directive.\n", lineCount);
					ERRORflag = 1;
					fclose(fp);
				}
				if (searchSymbol(symbol) != -1) /*Check if symbol has been used before*/
				{
					printf("Error: Duplicate Symbol %s on line %d", symbol, lineCount);
					ERRORflag = 1;
				}
				if (startFlag == 0) 
				{
					strcpy(starter, symbol);  
					startFlag = 1; /*there is a need for a seperate variable because of comments*/
				}
				else
				{
					/*printf("%s\t%X\n",symbol,address);*/
					addSymbol(symbol, address);
				}
				symbol = strtok(NULL, " \t\n");/*grab the next token*/
			}
			int dStat = isDirective(symbol);/*Directive Check Gang*/
			if (dStat >= 1)
			{
				/*printf("Directive Detected:'%s' ", symbol);*/
				if (dStat == 1)/*START*/
				{
					if (!(lineCount == 1))
					{
						printf("Error: START directive must be used on first line only");
					}
					char* useless;
					address = strtol(strtok(NULL, " \t\n"), &useless, 16); /*read in the address as base 16 and store it in address*/
					addSymbol(starter, address);
					startAddress = address;
					adds[lineCount] = 0;
					printf("START found at line: %d address set to %d", lineCount, adds[lineCount]);
				}
				else if (dStat == 2)
				{

				}
				else if (dStat == 3)
				{/*generate char or hex constant using as many bytes as needed*/
					symbol = strtok(NULL, " \t\n");
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
								printf("ERROR invalid hex character %d on line %d", symbol[i], lineCount);
								ERRORflag = 1;
							}
							i++;
						}
						address = address + (int)((i - 2.0) / 2.0) + .99;
					}
					else
					{
						printf("Error line %d: Invalid BYTE parameter; constant should be preceded by 'C' or 'X'", lineCount);
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

			}
			else if (getOpcode(symbol) == "0")/*Neither directive nor instruction*/
			{
				printf("Invalid Instruction:'%s' on line: %d\n", symbol, lineCount);
				ERRORflag = 1;
			}
			else /*Valid instruction, this logic is a bit cringe and should be reworked to have valid case first*/
			{
				address = address + 3;
			}
			/*printf("line:%d address: %X\n", lineCount, address);*/
			/*memset(line, '\0', 1024*sizeof(char));
			memset(symbol, '\0', 1024*sizeof(char));*/
		}
		while (symbol != NULL)
		{
			symbol = strtok(NULL, " \t\n"); /*flush strtok of the entire line*/
		}
		/*wipe line and symbol before running again. This might not be necessary idk*/
		/*memset(line, '\0', 1024 * sizeof(char));
		memset(symbol, '\0', 7 * sizeof(char));*/
	}
	fclose(fp); /*PASS 1 COMPLETE Make sure all error cases close properly*/

	printTable();
	if (ERRORflag == 1) { return -1; }
	int lCount = 0;
	fp = fopen(argv[1], "r"); 
	FILE* p2 = fopen(strcat(argv[1], ".objt"), "w"); /*open file for writing*/
	int recordLength;
	memset(line, '\0', 1024 * sizeof(char)); 
	char* field3; 
	char charBuffer[30];
	char wordBuffer[30];
	while (fgets(line, 1024, fp) != NULL) 
	{
		recordLength = 3;
		if (line[0] == 35) /*If its a comment, don't do anything*/
		{
			continue; 
		}
		lCount++; /*only increment if not a comment*/
		symbol = strtok(line, " \t\n");
		if (!((line[0] == ' ') || (line[0] == '\t'))) /*First column is not empty*/
		{
			symbol = strtok(NULL, " \t\n");
		} 
		int dResult = isDirective(symbol);
		if (dResult > 0) /*Directive*/
		{
			/*printf("Directive: %s", symbol);*/
			symbol = strtok(NULL, " \t\n");
			if (dResult == 3) /*BYTE*/
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
				sprintf(wordBuffer, "%.6X", atoi(symbol));
				recordLength = recordLength + 3;
				adds[lCount] = -adds[lCount]; /*invert value to indicate it's a directive in need of printing*/
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
			field3 = strtok(NULL, " \t\n");
			if (strtok(NULL," \t\n") == NULL)/*line isn't being memset so it contains random garbage as another delimeter following the last delim that was read in by fgets*/
			{/*this might create errors in some circumstances if that random garbage contains a delimeter such as from a previous line idk*/
				field3 = NULL;/*absence of 3rd field detected; lookup address is 0000*/
			}
		}
		/*The line with the START directive is the special H record, this testing logic may be improved but is simply but in one place for now*/
		if (lCount == 1) /*START line*/
		{
			printf("\nH%s  %.6X%.6X", starter, startAddress, address - startAddress);
		}
		else if (adds[lCount] > 1) /*if it's an instruction*/
		{
			printf("\nT%.6X%.2d%s%.4X", adds[lCount], recordLength, getOpcode(symbol), searchSymbol(field3));
			if (searchSymbol(field3) == 0)
			{
				adds[lCount] = 0; /*print T record but not an M record for RSUB*/
			}
		}	
		else if(adds[lCount] < 0)/*if its a directive in need of printing; BYTE and WORD*/
		{
			printf("\nT%.6X%.2d%s", -adds[lCount], recordLength, wordBuffer); /*this is convention for not combining lines*/
			memset(wordBuffer, '\0', 30 * sizeof(char)); /*clear wordBuffer*/
		}
	}
	/*M records, skip lines flagged as uneccessary by -1*/
	for (int k = 1; k < lCount; k++)
	{
		if (adds[k] > 1)
		{
			printf("\nM%.6X04+%s", adds[k] + 1, starter);
			/*printf(" for line %d", k);*/
		}
	}
	/*fprintf(p2,"T%X %X %c %s%X", adds[i],3, getOpcode("JLT"),searchSymbol("SYMNAME"));*/
	fclose(fp);
	fclose(p2);
}
	