#include "head.h"

/********************************************
 This file was written by:
 # Ron Gabay     -> ID number 205592017
*********************************************/


/* node for data structure */
void createNodeForData(data **hd, int lenD)
{
	data *headData = (data *)*hd;
	if(lenD == 0)
	{
		headData = (data *) malloc (sizeof(data));
		
	}
	else
	{
		headData = (data *) realloc (headData, ++lenD * sizeof(data));
		if(!headData)
		{
			printf("\n %s\n", ERR_MEMORY);
		}
	}
	*hd = headData;
}

/* node for directive structure */
void createNodeForCode(code **hc, int lenC)
{
	code *headCode = (code *)*hc;
	if(lenC == 0)
	{
		headCode = (code *) malloc (sizeof(code));
	}
	else
	{
		headCode = (code *) realloc (headCode, ++lenC * sizeof(code));
		if(!headCode)
		{
		
			printf("\n%s\n", ERR_MEMORY);
		}
	}
	*hc = headCode;
}

/* node for code structure */
void createNodeForDirective(directive **hdir, int lenDir)
{
	directive *headDirective = (directive *)*hdir;
	if(lenDir == 0)
	{
		headDirective = (directive *) malloc (sizeof(directive));
	}
	else
	{
		headDirective = (directive *) realloc (headDirective, ++lenDir * sizeof(directive));
		if(!headDirective)
		{	
		
			printf("\n%s\n", ERR_MEMORY);
		}
	}
	*hdir = headDirective;
}

/* collects all the data symbols that are written to the command line.
 * stores the data symbols into a table that is collected on the first read  of the program for later use. */
bool addToData(char *symbol,int address, char *type, data **headData, int **lenData)
{	
	data *hd = (data *)*headData;
	int *lenD = (int *)*lenData;
	
	/*check if there is cells at the array of data table*/
	if((*lenD) >= 1)
	{
		int i=0;
		for(;i<((*lenD)) ;i++)
		{
			/*check if we found the symbol that we wants to add the data table*/
			if(strcmp(symbol, hd[i].symbol) == 0)
			{	
				if(hd[i].full == 2)
					return false;
				if(strcmp(type, "extern") == 0)
					return false;
				if(strcmp(type, "entry") == 0)
				{
					if(strcmp(hd[i].attributes[0], "entry")==0 || strcmp(hd[i].attributes[0], "extern")==0)
						return false;
						
					else
					{
						strcpy(hd[i].attributes[1], type);
						if(strcmp(type, "extern")==0)
							hd[i].value = address;
						
						hd[i].full = 2;
						*headData = hd;
						return true;
					}
				}
				
				else
				{
					if(strcmp(hd[i].attributes[0], "entry")==0)
					{
						strcpy(hd[i].attributes[1], type);
						if(strcmp(type, "extern")!=0)
							hd[i].value = address;
						
						hd[i].full = 2;	
						*headData = hd;
						return true;
					}
					/*the attribute might be data, code or external*/
					else 
						return false;
				}
			}
		}
	}
	
	createNodeForData(&hd, (*lenD));
	(*lenD)++;
	hd[(*lenD)-1].symbol = (char *)malloc((strlen(symbol)+1)*sizeof(char));	
	strcpy(hd[(*lenD)-1].symbol, symbol);
	strcpy(hd[(*lenD)-1].attributes[0], type);
	
	if(strcmp(type, "entry") != 0)
		hd[(*lenD)-1].value = address;
	
	hd[(*lenD)-1].full = 1;
	*headData = hd;
	*lenData = lenD;
	
	return true;
}

/* collects all the directive values that are written to the command line.
  stores the directives into a table that is collected on the first read  of the program for later use. */
void addToDirective(int address, char *dir, directive **headDirective, void *dirArray, int** lengthDirective)
{
	directive *hdir = (directive *)*headDirective;
	int *lenDir = (int *)*lengthDirective;
	
	char *asciz;
	long *dwdhdb;
	
	int length = 0;
	int i = 0;
	int jump;
	bool flag_Asciz = true;
	
	
	if(strcmp(dir, "asciz") == 0)
	{
		asciz = (char *)dirArray; /*casting the array to his type*/
		length = strlen(asciz)+1;
		jump = 1;
	}
		
	else
	{
		flag_Asciz = false;
		dwdhdb = (long *)dirArray;
		for(; dwdhdb[length]; length++);
		/*check the jump of the addresses*/
		if(strcmp(dir, "db") == 0)
			jump = 1;
			
		if(strcmp(dir, "dh") == 0)
			jump = 2;
		
		if(strcmp(dir, "dw") == 0)
			jump = 4;
	}
	
	
	for(; i < length; i++)
	{
		createNodeForDirective(&hdir, (*lenDir));
		(*lenDir)++;
		hdir[(*lenDir)-1].address = (address + (i * jump));
		hdir[(*lenDir)-1].directive = (char *) malloc ((strlen(dir)+1) * length);
		strcpy(hdir[(*lenDir)-1].directive, dir);
		/*each num\char will be conver to long type which will be saved at the machine code value*/	
		if(flag_Asciz)
			hdir[(*lenDir)-1].machineCode = (long)asciz[i];
		else
			hdir[(*lenDir)-1].machineCode = dwdhdb[i];
	}
	
	*headDirective = hdir;
	*lengthDirective = lenDir;
}

/* collects all the code values that are written to the command line.
 * stores the codes into a table that is collected on the first read  of the program for later use. */
void addToCode(int address, char *line, int label_exist, char *code_name, char group, int num_of_group, code **headCode, int **lengthCode, int is_ext)
{
	int *lenC = (int *)*lengthCode;
	code *hc = (code *)*headCode;
	createNodeForCode(&hc, (*lenC));
	(*lenC)++;
	
	/*the source code is filled with the text that is written after the code name*/
	hc[(*lenC)-1].sourceCode = (char *)malloc((strlen(line)+1)*sizeof(char));
	/*the name is filled with the name of code*/
	hc[(*lenC)-1].name = (char *)malloc((strlen(code_name)+1)*sizeof(char));
	hc[(*lenC)-1].address = address;
	strcpy(hc[(*lenC)-1].sourceCode, line);
	strcpy(hc[(*lenC)-1].name, code_name);
	/*the group is filled with the group (I\J\R) that the code name came from*/
	hc[(*lenC)-1].group = group;
	hc[(*lenC)-1].num_of_group = num_of_group;
	/*label exist is a flag that shows if there is at I and J line codes a label*/
	hc[(*lenC)-1].label_exist = label_exist;
	hc[(*lenC)-1].is_ext = is_ext;
	
	*headCode = hc;
	*lengthCode = lenC;
}

