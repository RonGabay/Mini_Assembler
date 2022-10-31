#include "head.h"

/********************************************
 This file was written by:
 # Ron Gabay     -> ID number 205592017
*********************************************/


/* this function gets an address of array of strings with 3 cells
 * (each cell has diffrent word)
 * the function fills the array with words while each cell is a diffrent part
 * of the code line*/
void buildToken(code *line, char ***tok)
{
	char **token = (char **)*tok;
	char *str;
	int limit;
	int i, ind = 0;
	
	str = (char *)malloc((strlen(line -> sourceCode)+1) * sizeof(char)); 
	if(!str)
		printf("\n%s\n", ERR_MEMORY);
		
	strcpy(str,line -> sourceCode);
	
	/*we need to choose how namy cells will be filled*/
	if(line -> group == 'I' || (line -> group == 'R' && line -> num_of_group == 1))
	{
		limit = 3;
	}
	else
	{
		if(line -> group == 'J')
			limit = 1;
		else
			limit = 2;
	}
	
	for( i = 0; i<limit; i++, ind = 0)
	{
		token[i] = (char *)malloc(LENGTH_LINE * sizeof(char));
		if(token[i] == NULL)
		{
			printf("\n%s\n", ERR_MEMORY);
			break;
		}
		/*read until we've got ',' and copy it to the cell*/
		while(*str != ',' && (*str))
		{
			token[i][ind++] = *str;
			str++;
		}
		str++;
		token[i][ind] = '\0';
	}
	*tok = token;
}

/*build the machine code for R group*/
int binaryR(int opcode, int rs, int rt, int rd, int funct)
{
        int opcodeC = opcode<<26;
        int rsC = rs<<21;
        int rtC = rt<<16;
        int rdC = rd<<11;
        int functC = funct<<6;
        
        return opcodeC | rsC | rtC | rdC | functC; 
}

/*build the machine code for I group*/
int binaryI(int opcode, int rs, int rt, int immed)
{
	   int opcode_copy = opcode << 26;
	   int rs_copy = rs << 21;
	   int rt_copy = rt << 16;
	   int immed_copy;
	   
	   if(immed<0)
	      immed_copy = ((~(abs(immed))) + 1)&0XFFFF;/*complitementary to 2*/
	   else
	      immed_copy = immed;
	      
	   return (opcode_copy | rs_copy | rt_copy | immed_copy);
}

/*build the machine code for J group*/
int binaryJ(int opcode, int reg, int address)
{
        int opcodeC = opcode<<26;
        int regC = reg<<25;
        return opcodeC | regC | address;
}

/*find the number of the range*/
int findReg(char *token)
{
	char cnum[3];
	int num;
	int ind = 0;
	
	for(;*token != '$'; token++);
	token++;
	
	while((*token) && isdigit(*token))
	{
		cnum[ind++] = *token;
		token++;
	}
	
	cnum[ind] = '\0';
	num = atoi(cnum);
	return num;
}

/*check id the label that is inside of line is valid and mentioned at the symbol table*/
int checkLabel(char * label, data *headData, int lenD)
{
	int ind=0;
	
	for(; ind < lenD; ind++)
	{
		if((strcmp(label, headData[ind].symbol) == 0))
			return ind;
		
		
	}
	return NONE;
}

/* took each code line and transfer it to machine code
 * if there is a label at the source code we will check if this is
 * a valid label
 * if this is isn't a valud label the function returns false*/
bool codeBinaryChange(code *line, data *headData, int lenD)
{
	char **tok;
	
	int j=0;
	int num;
	int rs, rt, rd, immed, reg, address, opcode, func;
	int ind, status;
	
	/*in order to find the opcode*/
	if(line -> group == 'R')
	{
		ind = searchAtList((line -> name),R1F,R2L);
		func = list[ind].func;
	}
	else
		ind = searchAtList((line -> name),I1F, JL);
		
	opcode = list[ind].opcode;
	
	/*if we got a code line with 'stop'*/
	if(isWhiteSpaces(line -> sourceCode))
	{
		reg = 0;
		address = 0;
		line->machineCode = binaryJ(opcode, reg, address);
		
		return true;
	}
	
	tok = (char **)malloc(3*sizeof(char *));
	if(!tok)
		printf("\n%s\n", ERR_MEMORY);
		
	buildToken(line, &tok);
	

	if(line -> group == 'J' && line -> label_exist)
		status = IMMED;
	else
		status = REG_FIRST;
		
	while (tok[j])
	{
		char *token;
		token = (char *)malloc((strlen(tok[j])+1)*sizeof(char));
		strcpy(token, tok[j]);	
		switch(status)
		{
			case REG_FIRST:
				num = findReg(token);
				if(line -> group == 'I')
				{
					rs = num;
					if(line -> num_of_group == 1 || line -> num_of_group == 3)
						status = IMMED;
					else /*num of group is 2*/
						status = REG_SECOND;
				}
				else
				{
					if(line -> group == 'R')
					{
						status = REG_SECOND;
						if(line -> num_of_group == 1)
							rs = num;
						else
							rs = num;
					}
					else /*line group is J*/
					{
						reg = 1;
						address = num;
						line -> machineCode = binaryJ(opcode, reg, address);
						return true;
					}
				}
			break;
			
			case REG_SECOND:
				num = findReg(token);
				if(line -> group == 'I' || (line -> group == 'R' && line -> num_of_group == 1))
					rt = num;
					
				else/*we are at group R2*/
				{
					rd = num;
					rt = 0;
					line -> machineCode = binaryR(opcode, rs, rt, rd, func);
					return true;
				}
				
				if(line -> group == 'I' && (line -> num_of_group == 1 || line -> num_of_group == 3))
				{
					line -> machineCode = binaryI(opcode, rs, rt, immed);
					return true;	
				}
				
				if(line -> group == 'I' && line -> num_of_group == 2)
					status = IMMED;
					
				if(line -> group == 'R' && line -> num_of_group == 1)
					status = REG_THIRD;
				
			break;
			
			case REG_THIRD:
				rd = findReg(token);
				line -> machineCode = binaryR(opcode, rs, rt, rd, func);
				return true;
			break;
			
			case IMMED:
				if(!(line -> label_exist))
				{
					if(line -> group == 'J')
					{
						for(;*token != '$'; token++);
						
						token++;						
					}
					num = atoi(token);
				}
				
				else /*there is a label at the line*/
				{
					char *label;
					int k;
					int sizeOfToken;
					char *token2;
					/* avoid spaces/tabs */
					for(;SPACE(*token); token++);
					sizeOfToken = strlen(token);
					token2 = token;
					
					/*reading label and saves it at label value*/
					label = (char*)malloc(LENGTH_LINE * sizeof(char));
					if(!label)
					{
						printf("\n%s\n",ERR_MEMORY);
						return false;
					}
					
					
					for(k=0; k<sizeOfToken && !SPACE(*token2) && (*token2 != ','); k++)
					{
						label[k] = *token2;
						token2++;
					}
					label[k] = '\0';
					num = checkLabel(label, headData, lenD);
					
					if(num == NONE)
						return false;
							
					if(strcmp(headData[num].attributes[0],"extern")==0 || 
					  (headData[num].full == 2 && strcmp(headData[num].attributes[1],"extern")==0))
					{
						(*line).is_ext = TRUELY;
						(*line).ext = (char *)malloc((strlen(label)+1)*sizeof(char));
						if(!(*line).ext)
						{
							printf("\n%s\n", ERR_MEMORY);
						}
						strcpy(line -> ext,label);		
					}
					
					free(label);
					num = headData[num].value;
				}
				
				if(line -> group == 'J')
				{
					address = num;
					reg = 0;
					line -> machineCode = binaryJ(opcode, reg, address);
					return true;
				}
				if(line -> group == 'I' && (line -> num_of_group == 1 || line -> num_of_group == 3))
				{
					immed = num;
					status = REG_SECOND;
				}
				else /*we are at I2*/
				{
					immed = num - (line -> address);
					line -> machineCode = binaryI(opcode, rs, rt, immed);
					return true;
				}
			break;
		}
		
		j++;
		free(token);
	}
	free(tok);
	
	return true;
}

/* this function is the function that store the storage into a table after the second run. */
bool secondRead(data *headData, code **headCode, int lenC, int lenD)
{
	code *copy = (code *)*headCode;
	int i = 0;
	
	for(; i < lenC; i++)
	{
		
		if(!codeBinaryChange(&copy[i], headData, lenD))
			return false;
	}
	*headCode = copy;
	
	return true;
}

