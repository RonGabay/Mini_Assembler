#include "head.h"

/********************************************
 This file was written by:
 # Ron Gabay     -> ID number 205592017
*********************************************/


/* function that helps for the use of readFirst() function.
 * this function receive directive or data values,
 * and manipulate the address of each value for later use. */
void changeAddress(data **headData, directive **headDirective, int IC_add, int lenD, int lenDir)
{
	data *hd = (data*)*headData;
	directive *hdir = (directive *)*headDirective;
	
	int i = 0;
	int j = 0;
	int num;
	
	for(; i < lenD; i++)
	{
		if((hd[i].full ==2 && (strcmp(hd[i].attributes[1], "data") == 0)) || (strcmp(hd[i].attributes[0], "data") == 0))
		{
			num = (hd[i].value) + IC_add;
			hd[i].value = num;
		}
	}
	
	for(; j < lenDir; j++)
	{
		num = (hdir[j].address) + IC_add;
		hdir[j].address = num;
	}
	
	*headData = hd;
	*headDirective = hdir;
}

/* function that read the first run of the file.
 * this function stores the data, directive and code values for later use on the second read.
 * this function knows how receive entry or extern command and stores them for late use in different files.
 * this function stores and manages the address in accordance to values. 
 * this function check if the values are correct according to the program,
 * if not correct than throws an error in specified line and what's wrong with it. */
int firstRead(FILE *fd, data **hd, directive **hdir, code **hc, int *lenDir, int *lenC, int *lenD, int *flag_ent, int *flag_ext)
{
	/* copy pointers */	
	data *headData = (data *)*hd;
	code *headCode = (code *)*hc;	
	directive *headDir = (directive *)*hdir;
	
	/* strings array of directives tokens */
	char *tokens[7] = {"db", "dh", "dw", "asciz","entry", "extern"};
	
	/* variables */
	char codeChecker;
	int status;
	int DC = JUMPING;
	int jump = JUMPING;
	int check1 = FALSELY;
	int check2 = FALSELY;
	int label_exist = 0;
	int countLine = 0;
	int countErr = 0;
	int IC_address = FIRST_ADDRESS;
	

	while(true)
	{
		char *line;
		char *symbol;
		char *lineToDelete;
		
		line = (char *)malloc(LENGTH_LINE * sizeof(char));
		fgets(line, LENGTH_LINE, fd);
		countLine++;
		
		if(feof(fd))
			break;

		line[strcspn(line, "\n")] = '\0';
			
		lineToDelete = line;
		
		/* if there is an empty line */
		if(isWhiteSpaces(line)) 
			continue;
		
		/* Skip spaces/tabs */			
		for(; SPACE(*line); line++);
		
		/* if there is a ';' at the beginning of a line */
		if(*line == ';')
			continue;
			
		
		/* check if there is a symbol in the line */
		if(strchr(line, ':'))
		{
			symbol = (char *) malloc (SYMBOL * sizeof(char));
			check1 = readSymbol(&line, &symbol);
			if(!check1) /* there is a syntax error with writing the name of the symbol */
			{
				printf("\n %s%d \n", ERR_SYMBOL, countLine);
				countErr++;
				/*free(symbol);1*/ 
			}
				
			else
			{
				check2 = checkSymbol(symbol,headData, *lenD, tokens);
				switch(check2)
				{
					case 0: /* in case of symbol is already written once in the file */
						printf("\n %s%s%d \n", symbol, ERR_DUPLICATE, countLine);
						countErr++;
					break;
					
					case 2: /* in case the name of the symbol is a name of a code */
						printf("\n %s%s%d \n", symbol, ERR_SYMBOL_CODE,countLine);
						countErr++;
					break;
						
					case 3: /* in case the name of the symbol is a name of a directive line */
						printf("\n %s%s%d \n", symbol, ERR_SYMBOL_DIRECT,countLine);
						countErr++;
					break;
				}
			}
		}
		
		/*checking if there is a directive value at the line*/
		if(strchr(line, '.'))
		{
		  char *directive;
		  directive = (char *)malloc(DIRECTIVE * sizeof(char));
		  
		  if(readDirective(&line, &directive, tokens) == FALSELY)
		  {
		  	printf("\n%s%d\n",ERR_DIRECTIVE,countLine);
		  	countErr++;
		  }
		  
		  else
		  {
		  	char *nameAsciz = NULL;
			long *bitsArray = NULL;
			if(check1 && check2 && (strcmp(directive, "extern") == 0 || strcmp(directive, "entry") == 0))
			{
				printf("\n%s%d\n",WARNING, countLine);
				check1 = FALSELY;	
			}
			
		  	status = checkDirectiveLine(line, directive, &jump, &bitsArray, &nameAsciz);
		  	if(status == FALSELY)
		  	{
		  		printf("\n%s%d\n",ERR_SYNTAX ,countLine);
		  		countErr++;
		  	}
		  	else{
		  	if(status == 2)
		  	{
		  		printf("\n%s%d\n",ERR_RANGE ,countLine);
		  		countErr++;
		  	}
		  	
		  	else /*status is correctly*/
		  	{
		  		/*checking if there is a symbol in line*/
		  		if(check1 && check2) /*if there is a label in line*/
		  		{
		  				if(!addToData(symbol, DC, "data", &headData, &lenD))
	  					{
	  						printf("\n%s%s%d\n", symbol, ERR_SYMBOL_EXIST, countLine);
	  						countErr++;	
	  					}
		  		}
		  		
		  		if(strcmp(directive, "extern") == 0)
	  			{
	  				(*flag_ext) = TRUELY;
	  				if(!addToData(nameAsciz, 0, "extern", &headData, &lenD))
	  				{
	  					printf("\n%s%s%d\n", symbol,ERR_SYMBOL_EXIST, countLine);
	  					countErr++;
	  				}
	  			}
	  			
	  			else
	  			{
	  				if(strcmp(directive, "entry") == 0)
	  				{
	  					(*flag_ent) = TRUELY;
	  					if(!addToData(nameAsciz, DC, "entry",&headData, &lenD))
	  					{
	  						printf("\n%s%s%d\n", symbol, ERR_SYMBOL_EXIST, countLine);
	  						countErr++;	
	  					}
	  				}
	  				else
	  				{
	  					if(strcmp(directive, "asciz") == 0)
	  					{
							addToDirective(DC, directive, &headDir, (void *)nameAsciz, &lenDir);
						}	
						else
						{
							addToDirective(DC, directive, &headDir, (void *)bitsArray, &lenDir);
						}
	  			
						DC+=jump;
	  				}
	  			}
		  	}
		  	}
		  	jump = 0;
		  	if(nameAsciz != NULL)
		  		free(nameAsciz);
		  	else
		  		free(bitsArray);
		  }
		  
		  free(directive);
		}
		
		/*this case happens if there is a code word at the line*/
		else
		{
			int group;
			int validCheck;
			int place;
			
			char *code;			
			code = (char *) malloc (CODE * sizeof(char));
			
			codeChecker = checkCode(&line, &code);
			
			switch(codeChecker)
			{
				case 'F': 
					printf("\n%s%d\n", ERR_CODE, countLine);
					
				case 'R':
					place = searchAtList(code, R1F, R2L);
					if(place>= R1F && place <=R1L)
					{
						validCheck = validCode(line, codeChecker, 1, &label_exist);
						group = 1;
					}
					else
					{
						validCheck = validCode(line, codeChecker, 2, &label_exist);
						group = 2;
					}
				break;
				
				case 'I':
					place = searchAtList(code, I1F, I3L);
					if(place>= I1F && place <=I1L)
					{
						validCheck = validCode(line, codeChecker, 1, &label_exist);
						group = 1;
					}
					
					else
					{
						if(place>= I2F && place <=I2L)
						{
							validCheck = validCode(line, codeChecker, 2, &label_exist);
							group = 2;
						}
						else
						{
							validCheck = validCode(line, codeChecker, 3, &label_exist);
							group = 3;
						}
					}
				break;
				
				case 'J': 
					if(strcmp(code, "stop") == 0)
					{	
						if(!isWhiteSpaces(line))
						{
							printf("\n%s%d\n", ERR_SYNTAX, countLine);
							countErr++;
						}
					}
					else
					{
						validCheck = validCode(line, codeChecker, 1, &label_exist);
						group = 1;
					}
				break;
			}
			
			if(validCheck == 2)
			{
				printf("\n%s%d\n",ERR_RANGE_CODE, countLine);
				countErr++;
			}
			
			if(!validCheck)
			{
				printf("\n%s%d\n", ERR_SYNTAX, countLine);
				countErr++;
			}
			
			else /*the code line is written correctly*/
			{
				if(check1 && check2) /*if line has label in it*/
					addToData(symbol, IC_address, "code", &headData, &lenD);
				
				addToCode(IC_address, line, label_exist, code, codeChecker, group, &headCode, &lenC, FALSELY);
				IC_address += IC;
			}
			free(code);
		}
		if(strchr(line, ':'))
			free(symbol);
			
		check1 = FALSELY;
		check2 = FALSELY;
			
		label_exist = 0;
		free(lineToDelete);
	}
	
	changeAddress(&headData, &headDir, IC_address, *lenD, *lenDir);
	*hd = headData;
	*hdir = headDir;
	*hc = headCode;
	
	if(countErr != 0)
		return FALSELY;
	
	return TRUELY;
}

