#include "head.h"

/********************************************
 This file was written by:
 # Ron Gabay     -> ID number 205592017
*********************************************/


/* function that synchronize the directive operands of dw, dh, db.
 * this function check if there's a valid directive operand and store the values.
 * this function check wheter the directive oparnds are written correctly.
 * in case the oprands are not written corretly, an error occure. */
int synDhDwDb(char * line, char *directive, int **DC, long **bitsArray)
{
	long *copy = (long*)*bitsArray;
	int *temp = (int *)*DC;
	char *dirNum;
	int *cJ;
	
	int ind =0;
	char c;
	int indNum = 0;
	int num;
	int countJ = 0;
	int status = SPACE_FIRST;
	
	dirNum = (char *) malloc (LENGTH_LINE * sizeof(char));
	if(!dirNum)
	{
		printf("\n%s\n", ERR_MEMORY);
		return FALSELY;
	}
	while((c= *line))
		{
			switch(status)
			{
			   /*in case we are at the space after the directive word of we've got spaces after comma*/
			   case SPACE_FIRST:
			   	status = spaceFirstCase(c, DIRECTIVE, ' ' );
			   	if(status != DIGIT)
			   		line++;
			   	break;
			   /*in case we've got digits*/
			   case DIGIT:
			   	status = digitCase(directive ,c, &indNum, &dirNum, &countJ, DIRECTIVE);
			   	if(status == 2)
			   		return 2;
			   	if(status != DIGIT)
			   	{
			   		dirNum[indNum]='\0';
			   		num = atoi(dirNum);
			   		copy[ind++] = (long)num;
			   		free(dirNum);
			   		dirNum = (char *)malloc(LENGTH_LINE * sizeof(char));
			   		indNum = 0;
			   	}
			   	line++;
			   	break;
			   /*in case we've got spaces after a digit*/	
			   case SPACE_MIDDLE:
			   	status = spaceMiddleCase(c);
			   	if(status !=DIGIT)
			   		line++;
			   	break;
			   
			   case COMMA: 
			   	status = commaCase(line,c);
			   	if(status != DIGIT)
			   		line++;
			   	break;
			   
			}
			if(status == FALSELY)
				return FALSELY;
			
		}
		cJ = &countJ;
		dirNum[indNum] = '\0';
		num = atoi(dirNum);
		if(status == DIGIT)
		{
			status = checkRange(directive, num, &cJ);
			if(status == 2)
				return 2;
			copy[ind++] = num;
		}
		(*temp) += countJ;
		free(dirNum);
		copy[ind] = '\0';
		*bitsArray = copy;
		*DC = temp;
		
		return TRUELY;
}

/* function that synchronize the entry/extren operands.
 * this function check if there's a valid entry/extren operand and store the values.
 * this function check wheter the entry/extren oparnds are written correctly.
 * in case the oprands are not written corretly, an error occure. */
int synExtEnt(char *line, char *directive, char **name)
{
	char *copy= (char *)*name;
	int c = *line;
	char *q;
	int tens = 2;
	int ind = 0;
	int count = 0;
	int status = SPACE_FIRST;
	
	while(c != 0)
	{
		switch(status)
		{
		
			case SPACE_FIRST:
				if(SPACE(c))
				{
					status = SPACE_FIRST;
					line++;
					c = *line;
				}
				else
				{
					if(!isalpha(c))
						return FALSELY;
					else
						status = NAME;
				}
			break;
				
			case NAME:
				if(SPACE(c))
					return isWhiteSpaces(line);
					
				else
				{
					if(!isalpha(c) && !isdigit(c))
						return FALSELY;
						
					if(count == SYMBOL)
					{
						q = (char *)realloc(copy,tens++ * SYMBOL * sizeof(char));
						if(!q)
						{
							printf("\n%s\n", ERR_MEMORY);
							return FALSELY;
						}	
						copy = q;
					}
					copy[ind++] = c;
					count++;
				}
				line++;	
				c = *line;
			break;
		}
	}
	if(count == SYMBOL)
	{
		q = (char *)realloc(copy,tens++ * SYMBOL * sizeof(char));
		if(!q)
		{
			printf("\n%s\n", ERR_MEMORY);
			return FALSELY;
		}	
		copy = q;
	}
	copy[ind] = '\0';
	*name = copy;
	
	return TRUELY;
}

/* function that synchronize the asciz operand.
 * this function check if there's a valid asciz operand and store the values.
 * this function check wheter the asciz oparnd are written correctly.
 * in case the oprands are not written corretly, an error occure. */
int synAsciz(char *line, char *directive, int **DC, char **ascizArray)
{
	int *temp = (int *)*DC;
	char *copy = (char*)*ascizArray;
	int ind =0;
	
	for(;*line!='\"' && *line != EOF; line++)
	{
		if(*line != ' ' && *line != '\t')
			return FALSELY;
	}
	if(!(*line))/*if there isn't mark questions at the line*/
		return FALSELY; 
	line++;
	for(;*line!='\"' || !(*line); line++) /*read the sentence between the question marks*/
	{
		copy[ind++] = *line;
		(*temp)++;
	}
	(*temp)++;
	if(!(*line)) /*if there isn't mark questions at the end of the line*/
		return FALSELY;
		
	line++;
	
	if(!isWhiteSpaces(line))
		return FALSELY;
		
	*DC = temp;
	copy[ind] = '\0';
	*ascizArray = copy;
	
	return TRUELY;	
}

/* function that read a valid directive operand after the character '.' */
int readDirective(char **line,char **directive, char **tokens)
{
	char *temp = (char *)*directive;
	char *copy = (char *)*line;
	int i=0;
	int j=0;
	int k=0;
	char c;
	
	for(; (c = copy[i]) != '.'; i++)
	{
		if(c != ' ' && c != '\t')
			return FALSELY;
	}
	i++;
	c = copy[i];
	while(c != ' ' && c !='\t')
	{
		/* check if the line is only lower case letters */
		if(!islower(c))
			return FALSELY;
		else
			temp[j++] = c;
		c = copy[++i];
	}
	temp[j] = '\0';
	copy += i;
	*directive = temp;
	*line = copy;
	
	for(;k < NUM_OF_TOKENS; k++)
	{
		if(strcmp(*directive, tokens[k]) == 0)
			return TRUELY;
	}
	return FALSELY;
}

/* function that check the name of a valid directive in a command line */
int checkDirectiveLine(char *line, char * directive, int *DC, long ** bitsArray, char **name)
{
	int num;
	if(strcmp(directive,"dh")==0 || strcmp(directive,"db")==0 || strcmp(directive,"dw")==0)
	{
		long *bits = (long *)*bitsArray;
		bits = (long *)malloc(LENGTH_LINE * sizeof(long));
		if(!bits)
			printf("\n%s \n", ERR_MEMORY);
		num = synDhDwDb(line, directive, &DC, &bits);
		*bitsArray = bits;
		return num;
	}
	else
	{
		if(strcmp(directive,"extern")==0 || strcmp(directive,"entry")==0)
		{
			char *cName = (char *)*name;
			cName = (char *)malloc(SYMBOL * sizeof(char));
			if(!cName)
				printf("\n%s \n", ERR_MEMORY);
			num = synExtEnt(line, directive, &cName);
			*name = cName;
			return num;
		}
		else
		{
			char *asciz = (char *)*name;
			asciz = (char *)malloc(LENGTH_LINE * sizeof(char));
			
			if(!asciz)
				printf("\n%s \n", ERR_MEMORY);
				
			num = synAsciz(line, directive, &DC, &asciz);
			*name = asciz;
			return num;
		}
	}	  
}

