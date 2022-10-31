#include "head.h"

/********************************************
 This file was written by:
 # Ron Gabay     -> ID number 205592017
*********************************************/


/* The main file. 
 * This file manages the assembling process. 
 * It calls the first and second read methods, and then creates the output files. */
int main(int argc, char * argv[])
{
	int length;
	int j=0;
	int flag_ent = FALSELY;
	int flag_ext = FALSELY;
	char *nameOfFile;
	
	/*building pointers for each table*/
	data *headData = NULL;
	directive *headDirective = NULL;
	code *headCode = NULL;
	
	
	FILE *fd;
	FILE *fdOB;
	FILE *fdENT;
	FILE *fdEXT;
	
	int i=1, lenC = 0, lenDir = 0, lenD=0;
	for(; i<argc; i++)
	{
		fd = fopen(argv[i], "r");
		if(!fd)
		{
			printf("\n file %s doesn't exist", argv[i]);
			break;
		}
		length =strlen(argv[i]); /*this variable include the length of the the file name +3 */
		nameOfFile = (char *)malloc((length) * sizeof(char));
		strncpy(nameOfFile, argv[i], length-3);
		nameOfFile[length-3] = '.';
		nameOfFile[length-2] = '\0';
		if(firstRead(fd, &headData, &headDirective, &headCode, &lenDir, &lenC, &lenD, &flag_ent, &flag_ext))
		{
			if(secondRead(headData, &headCode, lenC, lenD))
			{
				char *nOB;
				char *nENT;
				char *nEXT;
				nOB = (char *)malloc((length+3) * sizeof(char));
				nENT = (char *)malloc((length+3) * sizeof(char));
				nEXT = (char *)malloc((length+3) * sizeof(char));
				
				strcpy(nOB, nameOfFile);
				strcpy(nENT, nameOfFile);
				strcpy(nEXT, nameOfFile);
				
				strcat(nOB,"ob");
				strcat(nENT,"ent");
				strcat(nEXT,"ext");
				
				/*create the OB file*/
				if(!(fdOB = fopen(nOB, "w+t")))
				{
					fprintf(stderr,"\ncan't create OB file \n");
					break;
				}
				createOB(&fdOB, headDirective, headCode, lenDir, lenC);
				fclose(fdOB);
				
				
				if(flag_ent)
				{
					/*create the ENT file*/
					if(!(fdENT = fopen(nENT, "w+t")))
					{
						fprintf(stderr,"\ncan't create ENT file \n");
						break;
					}
					createEnt(&fdENT, headData, lenD);
					fclose(fdENT);
				}
				if(flag_ext)
				{
					/*create the EXT file*/
					if(!(fdEXT = fopen(nEXT, "w+t")))
					{
						fprintf(stderr,"\ncan't create EXT file \n");
						break;
					}
					createExt(&fdEXT, headCode, lenC);
					fclose(fdEXT);
				}
				
				
				free(nOB);
				free(nENT);
				free(nEXT);
				
				
				
				
			}
		}
		free(nameOfFile);
	}
	for(; j < lenD; j++)
	{
		free(headData[j].symbol);
	}
	free(headData);
	
	j=0;
	for(; j < lenDir; j++)
	{
		free(headDirective[j].directive);
	}
	free(headDirective);
	
	j=0;
	for(; j < lenC; j++)
	{
		free(headCode[j].sourceCode);
		free(headCode[j].name);
		if(headCode[j].is_ext)
		{
			free(headCode[j].ext);
		}
	}
	free(headCode);
	
	return 1;
}

