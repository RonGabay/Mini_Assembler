#include "head.h"

/********************************************
 This file was written by:
 # Ron Gabay     -> ID number 205592017
*********************************************/


/* function that sort each name to his type */     
char whichType(char *str)
{
	if(
		(strcmp(str, "add")==0)||
		(strcmp(str, "sub")==0)||
		(strcmp(str, "and")==0)||
		(strcmp(str, "or")==0)||
		(strcmp(str, "nor")==0)||
		(strcmp(str, "move")==0)||
		(strcmp(str, "mvhi")==0)||
		(strcmp(str, "mvlo")==0))
		{
		
			return 'R';
		}
		
	if(
		(strcmp(str, "addi")==0)||
		(strcmp(str, "subi")==0)||
		(strcmp(str, "andi")==0)||
		(strcmp(str, "ori")==0)||
		(strcmp(str, "nori")==0)||
		(strcmp(str, "bne")==0)||
		(strcmp(str, "beq")==0)||
		(strcmp(str, "blt")==0)||
		(strcmp(str, "bgt")==0)||
		(strcmp(str, "lb")==0)||
		(strcmp(str, "sb")==0)||
		(strcmp(str, "lw")==0)||
		(strcmp(str, "sw")==0)||
		(strcmp(str, "lh")==0)||
		(strcmp(str, "sh")==0))
		{
			return 'I';
		}
	if(
		
		(strcmp(str, "jmp")==0)||
		(strcmp(str, "la")==0)||
		(strcmp(str, "call")==0)||
		(strcmp(str, "stop")==0))
		{
			return 'J';
		}
		
		return 'F';
}

/**** Inner codes for validCode function ****/
/* function that represent for what type of codes, there's a need for space middle in a command line */
int smCode(char c ,char *line ,char type)
{
	if(type == 'J')
	{
		/* avoid white spaces */
		if(isWhiteSpaces(line))
			return TRUELY;
			
		return FALSELY;
	}
	
	return spaceMiddleCase(c);
}

/* function that represent for what type of codes, there's a need for label  in a command line */
int labelCode(char c)
{
	if(SPACE(c)) 
		return SPACE_MIDDLE;
	
	else
	{	
		if(!isalpha(c) && !isdigit(c))
			return FALSELY;
		else	
			return LABEL;
	}	
}


/* function that represent for what type of codes, there's a need for comma  in a command line */
int commaCode(int countComma,char type, int numOfGroup, char **line, int **label_exist)
{
	int status;
	char *linec = (char *)*line;
	
	/* avoid white spaces */
	if(isWhiteSpaces(linec))
		return FALSELY;
	
	switch(type)
	{
		case 'R':
			if(numOfGroup == 1)
			{
				if(countComma <= 2)
					return SPACE_FIRST;
				
				else
					return FALSELY;
			}
			
			else
			{
				if(countComma == 1)
					return SPACE_FIRST;
				
				else
					return FALSELY;
			}
		break;
	
		case 'I':
			if(countComma == 1)
			{
				if(numOfGroup == 1 || numOfGroup == 3)
				{
					status = immedSituation(&linec,numOfGroup, &label_exist);
					*line = linec;
					if(status== TRUELY || status == FALSELY)
						return FALSELY;
					else
						return status;
				}
				else
					return SPACE_FIRST;
			}
			
			else
			{
				if(countComma == 2)
				{
					if(numOfGroup == 1 || numOfGroup == 3)
						return SPACE_FIRST;
					
					else
					{
						status = immedSituation(&linec, numOfGroup, &label_exist);
						*line = linec;
						return status;	
					}
				}
				
				else /* the number of commas is bigger then 2 */
					return FALSELY;
			}
		break;
	
		case 'J':
			return FALSELY;
		break;
	}
	
	return TRUELY;
}

/* function that check the syntax of code in a command line */
char checkCode(char **line, char **code)
{
	char *temp = (char *)*code;
	char *copy = (char *)*line;
	char c = *copy;
	int i=0;
	
	/* Skip spaces/tabs */
	for(; SPACE(c); copy++, c = *copy); /* for(; c == ' ' || c == '\t'; copy++) */

	while(c && c != ' ' && c != '\t') /* while(c != ' ' && c != '\t') */
	{
		if(!islower(c))
			return 'F';
		
		temp[i++] = c;
		copy++;
		c = *copy;
	}
	
	*line = copy;
	temp[i] = '\0';
	*code = temp;
	return whichType(temp);
}

/* function that check the validation of the code */
int validCode(char *line, char type, int numOfGroup, int *label_exist)
{
	char *dirNum;
	int status = SPACE_FIRST;
	int c;
	int indNum = 0;
	int countJ = 0;
	int countComma = 0;
	int num;
	
	dirNum = (char *)malloc(LENGTH_OF_NUM * sizeof(char));
	
	/* avoid white spaces */
	if(isWhiteSpaces(line))
		return FALSELY;
	
	/* Skip spaces/tabs */
	for(; SPACE(*line); line++); /* for(; *line = ' ' || *line = '\t'; line++); */
	
	while((c = *line))
	{
		switch(status)
		{
			case SPACE_FIRST:
				status = spaceFirstCase(c, CODE, type);
				line++;
			break;
			
			case REG:
				status = regCase(c);
				
				if(DIGIT_NUM(status))
					line++;
					
			break;
			
			case DIGIT:
				status = digitCase(" ", c, &indNum, &dirNum, &countJ, CODE); /* no need for directive name */
				
				if(status == 2)
					return 2;
					
				if(type == 'J')
				{
					status = SPACE_MIDDLE;
					/*free(dirNum);*/
				}
				
				else
				{
					if(status != DIGIT)
					{
						free(dirNum);
			   			dirNum = (char *)malloc(LENGTH_OF_NUM * sizeof(char));
			   			indNum = 0;
					}
				}
			line++;
			break;
			
			case SPACE_MIDDLE:
				status = smCode(c ,line ,type);
				line++;
			break;
			
			case LABEL: /* will be used only with getting line with 'J' code word */
				status = labelCode(c);
				(*label_exist) = 1;
				line++;
			break;
			
			case COMMA:
				countComma++;
				status = commaCode(countComma,type, numOfGroup, &line, &label_exist);
			break;
				
		}
		
		if(status == FALSELY)
			return FALSELY;

		if(status == TRUELY)
			return TRUELY;	
	}
	
	if(status == LABEL)
		(*label_exist) = 1;
		
	/* if there are more than 1 comma */
	if((type == 'I' || (type == 'R' && numOfGroup == 1)) && countComma < 2)
		return FALSELY;
	
	/* if last number is not in range (between 0-31)*/
	if(status == DIGIT)
	{
		dirNum[indNum] = '\0';
		num = atoi(dirNum);
		
		if(RANGE(num, 0, 31))
			return 2;
	}
	free(dirNum);
			
	return TRUELY;
}

