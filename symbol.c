#include "head.h"

/********************************************
 This file was written by:
 # Ron Gabay     -> ID number 205592017
*********************************************/


/* function that read a symbol at beggining of a command line.
 * a symbol is declared with a first alphabetic character,
 * and if there's a character of ':' after the last character
 * if the symbol wasn't written correctly, an error occure */
int readSymbol(char **line, char **symbol)
{
	char *copy = (char *)*line;
	char *temp = (char *)*symbol;
	char *q;
	
	char c;
	int i=0;
	int count = 0;
	int tens = 2;
	
	/* Skip spaces/tabs */
	for( ;*copy==' ' || *copy=='\t'; copy++);
	
	while((c = *copy) != ':')
	{
		if(count == SYMBOL) /* if we need more space for the name of label */
		{
			count = 0;
			q = (char *)realloc(temp, tens++ * SYMBOL * sizeof(char));
			if(!q)
			{
				printf("\n%s\n",ERR_MEMORY); 
				return FALSELY;
			} 
			temp = q; 
		}
		if(SPACE(c))
			return FALSELY;  

		else
		{
			temp[i++] = *copy;
			count++;
		}

		copy++;	 
	}
	
	copy++;
	
	if(*copy != ' ' && *copy != '\t')
		return FALSELY;
	if(count == SYMBOL) /* if we need more space for the name of label */
	{
		q = (char *)realloc(temp, tens++ * SYMBOL * sizeof(char));
		if(!q)
		{
			printf("\n%s\n",ERR_MEMORY); 
			return FALSELY;
		} 
		temp = q;
	}
	temp[i] = '\0';
	*line = copy;
	*symbol = temp;
	
	return TRUELY;
}


/* function that check if the symbol is written correctly after its declaration.
 * this function check inside the command line, each command and values are written correctly.
 * the symbols are stored inside a list for later use, after the first read manipulations.
 * if the command line wasn't written correctly, an error occure. */
int checkSymbol(char* symbol, data *headData, int lenD, char **tokens)
{
	int j=0;
	int i = 0;
	int ans=TRUELY;
	
	for(;j<(lenD-1) ;j++)
	{
		if((strcmp(symbol, headData[j].symbol)==0) && strcmp(headData[j].attributes[0], "entry") != 0 )
			return FALSELY;
	}
	
	/*checking if the symbol is a code word*/
	if(searchAtList(symbol, R1F, JL) != NONE)/* search symbol through all the list of R,I,J */
		return 2;
	
	/*checking if the symbol is a directive word*/
	for( ;i<NUM_OF_TOKENS && ans; i++)
	{
		if(strcmp(symbol,tokens[i])==0)
			ans = FALSELY;
	}
	
	if(!ans)
		return 3;
	
	return TRUELY;
}

