#include "headers.h" 

int main(int argc, char* argv[]) {
	FILE* fp;
	int adds[1000];
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
		if (address > 32768)
		{
			printf("ERROR: MEMORY OVERFLOW- SIC supports up to 32K of memory, or 8000 in hex");
			ERRORflag = 1;
		}
		adds[lCount] = address;
		symbol = strtok(line, " \t\n");
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
				symbol = strtok(NULL, " \t\n");/*grab the next token, advancing to field 2*/
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
					address = strtol(strtok(NULL, " \t\n"), &useless, 16); /*read in the address as base 16 and store it in address*/
					addSymbol(name, address); 
					startLine = lCount;
					adds[startLine] = address;
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

			}
			else if (getOpcode(symbol) == "0")/*Neither directive nor instruction*/
			{
				printf("Invalid Instruction:'%s' on line: %d\n", symbol, lCount);
				ERRORflag = 1;
			}
			else /*Valid instruction; address offset will probably need to be changed in XE*/
			{
				address = address + 3;
			}
			/*printf("line:%d address: %X\n", lCount, address);*/
			/*memset(line, '\0', 1024*sizeof(char));
			memset(symbol, '\0', 1024*sizeof(char));*/
		}
		while (symbol != NULL) /*this isn't actually necessary*/
		{
			symbol = strtok(NULL, " \t\n"); /*flush strtok of the entire line*/
		}
		/*wipe line and symbol before running again. This might not be necessary idk*/
		/*memset(line, '\0', 1024 * sizeof(char));
		memset(symbol, '\0', 7 * sizeof(char));*/
	}
	fclose(fp); /*PASS 1 COMPLETE Make sure all error cases close properly*/

	if (ERRORflag == 1) { return -1; }/*maybe I shouldn't do this; errors should just quit program immediately?*/
	lCount = 0;
	fp = fopen(argv[1], "r"); 
	FILE* p2 = fopen(strcat(argv[1], ".objt"), "w"); /*open file for writing*/
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
		symbol = strtok(line, " \t\n");
		if (!((line[0] == ' ') || (line[0] == '\t'))) /*If field 1 is not empty, we'll need to advance to the next token to be sure we're getting field 2*/
		{
			symbol = strtok(NULL, " \t\n");
		} 
		int dResult = isDirective(symbol);
		if (dResult > 0) 
		{
			symbol = strtok(NULL, " \t\n");/*advance to field 3*/
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
		/*The line with the START directive is the special H record, this testing logic may be improved but is simply put in one place for now*/
		/*Currently, there is an issue regarding the fact that adds[startLine] is used to store the address of the program*/
		/*But the printing logic relies on non-printing T-records to be 0*/
		if (lCount == startLine) /*START line*/
		{
			printf("\nH%s  %.6X%.6X", name, adds[startLine], address - adds[startLine]);
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
			printf("\nT%.6X%.2d%s", -adds[lCount], recordLength, wordBuffer); /*this is convention when not combining lines*/
			memset(wordBuffer, '\0', 30 * sizeof(char)); /*clear wordBuffer*/
		}
	}
	/*M records, skip lines flagged as uneccessary by -1*/
	int checkedRecords = 0;
	for (int k = startLine+1; k < lCount; k++)
	{
		if (adds[k] > 1)
		{
			printf("\nM%.6X04+%s", adds[k] + 1, name);
			/*printf(" for line %d", k);*/
		}
		checkedRecords = k;
	}
	/*E Record*/
	printf("\nE%.6X\n",adds[startLine]);
	/*fprintf(p2,"T%X %X %c %s%X", adds[i],3, getOpcode("JLT"),searchSymbol("SYMNAME"));*/
	fclose(fp);
	fclose(p2);
}
	