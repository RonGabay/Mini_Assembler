#include "head.h"

/********************************************
 This file was written by:
 # Ron Gabay     -> ID number 205592017
*********************************************/


/* a constant list table with R,I,J values (name,opcode,func) */
const item list[] = {
/** list 'R1' **/
	{"add", add, addF},
	{"sub", sub, subF},
	{"and", and, andF},
	{"or", or, orF},
	{"nor", nor, norF},
/** list 'R2' **/
	{"move", move, moveF},
	{"mvhi", mvhi, mvhiF},
	{"mvlo", mvlo, mvloF},
	
/** list 'I1' **/
	{"addi", addi, NONE},
	{"subi", subi, NONE},
	{"andi", andi, NONE},
	{"ori", ori, NONE},
	{"nori", nori, NONE},
/** list 'I2' **/
	{"bne", bne, NONE},
	{"beq", beq, NONE},
	{"blt", blt, NONE},
	{"bgt", bgt, NONE},
/** list 'I3' **/
	{"lb", lb, NONE},
	{"sb", sb, NONE},
	{"lw", lw, NONE},
	{"sw", sw, NONE},
	{"lh", lh, NONE},
	{"sh", sh, NONE},
	
/** list 'J' **/
	{"jmp", jmp, NONE},
	{"la", la, NONE},
	{"call", call, NONE},
	{"stop", stop, NONE}, 
};



/* function that run through all the lists of R,I,J.
 * check whether there is a valid command list. */
int searchAtList(char *code, int firstIndex, int lastIndex)
{
	int i;
	for(i = firstIndex; i <= lastIndex; i++)
	{
		if(strcmp(list[i].name,code)==0)
			return i;
	}
	return NONE;
}

/* returns if line contains only white spaces. */
bool isWhiteSpaces(char *line)
{
	while (*line)
	{
		if (!isspace(*line++))
		{
			return false;
		}
	}
	return true;
}

/* check when the directive values is inside the range and their address jump.
 * returns an error if directive are not in the range */
int checkRange(char *directive, long num, int **countJ)
{
	int check;
	/*check if the number is not in range of the number that can be absorted*/
	if(strcmp(directive, "db")==0) /* db = 1 bytes range = -128 to 127 */
	{
		if(RANGE(num, -128, 127))
			return 2;
		
		(**countJ)++; 
		
	}
	if(strcmp(directive, "dh")==0) /* dh = 2 bytes range = -32768 to 32767 */
	{
		if(RANGE(num, -32768, 32767))
			return 2;
		
		(**countJ)+=2;
		 
	}
	if(strcmp(directive, "dw")==0) /* dw = 4 bytes range = -2147483648 to 2147483647 */
	{
		if(num < INT_MIN || num > INT_MAX)
			return 2;
		
		(**countJ)+=4;
	}
	
	
	return TRUELY;
}

/* function that helps to *i_to_a() function.
 * counts how many digits. */
int num_of_digits(int num)
{
    int digit_count = 0;
    
    while(num > 0)
    {
        digit_count++;
        num /= 10;
    }

    return digit_count;
}

/* function that parse an integer to ascii string.
 * address is represented by 4 digit (if there are 3 digits add '0' to the beggining). */
void i_to_a(int num, char **word)
{
    char *str = (char*)*word;
    int digit_count = 0;

    digit_count += num_of_digits(num);

    if(digit_count == 3) {
        digit_count++;
    }

    str[digit_count] = '\0';

    while(num > 0)
    {
        str[digit_count-1] = num%10 + '0';
        num = num/10;
        digit_count--;
    }

    if(digit_count == 1)
        str[0] = '0';
    *word = str;

}

/* function that helps the immedSituation() function.
 * check if the value is a digit. */
void whatStatus(int *status, char temp)
{
	if(isdigit(temp))
		(*status) = DIGIT;
		
	else
	{
		if(temp == '-' || isdigit(temp))
			(*status) = DIGIT;
		else
		{
			if(!isalpha(temp))
				(*status) = FALSELY;
		}
	}
}

/* function that check when immed is a label or digit.
 * the function check in which case the immed value can apear correctly with operator included.
 * returns errors if immed was not written correctly. */
int immedSituation(char **line, int numOfGroup, int ***label_exist)
{
	char c;
	int status = LABEL;
	char *temp = (char *)*line;
	
	/* Skip spaces/tabs */
	for(;SPACE(*temp);temp++);
	
	whatStatus(&status, *temp);
	
	if(!status)
		return FALSELY;
		
	temp++;
	if(status == LABEL)
		(***label_exist) = 1;
	
	/*read the label\ digit until the end of the immed*/
	while((c = *temp) && c != ' ' && c != '\t' && c != ',')
	{
		switch(status)
		{
			case DIGIT:
				if(!isdigit(c))
					return FALSELY;
			break;
			
			case LABEL:
				if(!isdigit(c) && !isalpha(c))
					return FALSELY;
			break;
		}
		temp++;
	}
	/*if we got to the end of file*/
	if(!c)
	{
		if(numOfGroup == 2)
			return TRUELY;
		
			
		else
			return FALSELY;
	}
	temp++;
	*line = temp;
	
	if(SPACE(c))
		return SPACE_MIDDLE;
		
	if(c == ',')
	{
		if(numOfGroup == 1 || numOfGroup == 3)
			return COMMA;
			
		else
			return FALSELY;
	}
	temp++;
	
	return TRUELY;
}

/* function that checks inside a command line.
 * if there's a situation where a space is obtained after a code/directive word.
 * returns an error if the command line was not written correctly. */
int spaceFirstCase(char c, int type, char group)
{
	if(SPACE(c))
		return SPACE_FIRST;
		
	if(type == DIRECTIVE)
	{
		if(isdigit(c) || c == '+' || c == '-')
			return DIGIT;
		
		else
			return FALSELY;
	}
	else /*the type is code*/
	{
		if(c == '$')
			return REG;
		
		else
		{	
			if(group == 'J')
			{
				if(isalpha(c))
					return LABEL;
			}
		}			
	}
	return FALSELY;
}

/* function that checks inside a command line.
 * if there's a situation where a space is obtained after digit/immed value or after a comma.
 * returns an error if the command line was not written correctly. */
int spaceMiddleCase(char c)
{
   	if(SPACE(c))
   		return SPACE_MIDDLE;
   	
   	else
   	{
   		if(c == ',')
   			return COMMA;
   		
   		else
   			return FALSELY;
   	}
}

/* function that checks inside a command line.
 * if there's a situation where a comma is obtained after or before a digit.
 * returns an error if the command line was not written correctly. */
int commaCase(char * line, char c)
{
	/* line is only white spaces */ 
	if(isWhiteSpaces(line)) 
   		return FALSELY;
   		
   	if(SPACE(c)) /* if(c == ' ' || c == '\t') */
   		return SPACE_FIRST;
   	
   	else
   	{
   		if(isdigit(c)|| c == '-' || c== '+')
   			return DIGIT;
   		
   		else
   			return FALSELY;
   	}
}

/* function that checks inside a command line.
 * if there's a situation where a digit is obtained after a directive/immed word, comma, digit, or operator.
 * returns an error if the command line was not written correctly.
 * if there's a range error, the file continue to run the first read. */ 
int digitCase(char *directive, char c, int *indNum, char **dirNum, int *countJ, int type)
{
	char *temp = (char *)*dirNum;
	
	if(isdigit(c) || c == '+' || c == '-')
   	{
   		temp[(*indNum)] = c;
   		(*indNum)++;
   		*dirNum = temp;
   		return DIGIT;
   	}
   	
   	else
   	{
   		temp[(*indNum)] = '\0';
   		int num = atoi(temp);
   		
   		if(type == DIRECTIVE)
   		{
   			/*the number is not in range of the number that can be absorted*/
   			if(checkRange(directive, num, &countJ) == 2)
   				return 2;
	   	}
	   	
	   	else /*the type is code*/
	   	{
	   		/*checking if the register is in the range of the valid register's addresses*/
	   		if(RANGE(num,0,31)) /* (num < 0) || (num > 31) */
	   			return 2;  
	   	}
	   	
   		if(SPACE(c))
   			return SPACE_MIDDLE;
   		
   		if(c == ',')
   			return COMMA;
   			
   		else
   			return FALSELY;
   	}
}

/* function that checks inside a command line.
 * if there's a situation where a register is obtained after directive/immed word.
 * returns an error if the command line was not written correctly. */
int regCase(char c)
{
	if(isdigit(c))
		return DIGIT;
	
	else
		return FALSELY;
}

