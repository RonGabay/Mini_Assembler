#include "head.h"

/********************************************
 This file was written by:
 # Ron Gabay     -> ID number 205592017
*********************************************/


/*the function creates the binary file as a machine code*/
void createFile(FILE **bin, directive *headDirective, code *headCode, int lenC, int lenDir)
{
	
	
	FILE *fd = (FILE *)*bin;
	long numd;
	int numc;
	int num;
	int i=0, j=0;
	
	for(; i < lenC; i++)
	{
		numc = headCode[i].machineCode;
		num = fwrite(&numc, 4, 1, fd);
	}
	
	for(; j < lenDir; j++)
	{
		numd = headDirective[j].machineCode;
		
		if(strcmp(headDirective[j].directive, "db") == 0)
			num = fwrite(&numd,1,1,fd);
			
		if(strcmp(headDirective[j].directive, "dh") == 0)
			num = fwrite(&numd,2,1,fd);
			
		if(strcmp(headDirective[j].directive, "dw") == 0)
			num = fwrite(&numd,4,1,fd);
			
		if(strcmp(headDirective[j].directive, "asciz") == 0)
			num = fwrite(&numd,sizeof(char),1,fd);
	}
	*bin = fd;
}

/*the function creates binary file for the assembler input file and create the file.ob */
void createOB(FILE **fdOB, directive *headDirective, code *headCode, int lenDir, int lenC)
{
	FILE *fd = (FILE *)*fdOB;
	FILE *bin = fopen("binary.txt","w");
	
	int address = FIRST_ADDRESS;
	int length;
	int i;
	int num;
	
	createFile(&bin, headDirective, headCode, lenC, lenDir);
	fclose(bin);
	if(!(bin = fopen("binary.txt","r")))
	{
		fprintf(stderr, "can't create file");
		exit(0);
	}	
	
	/*calculate the code section size*/
	num = (headCode[lenC-1].address) +4 -FIRST_ADDRESS;
	fprintf(fd, "\t%d\t", num);
	
	/*Calculate the directive section size*/
	num = (headDirective[lenDir -1].address) - (headCode[lenC-1].address);
	fprintf(fd, "%d\n", num);
	
	while(true)
	{
		char *IC_address;
		char *s;
		
		s = (char *)malloc(5 *sizeof(char));
		IC_address = (char *)malloc(5*sizeof(char));
		
		if(feof(bin))
			break;
		
		i_to_a(address, &IC_address);
		address += IC;
		
		/* puts in the file, the address after managed correctly */
		fputs(IC_address, fd);
		length = fread(s,sizeof(char),4,bin);
		
		fprintf(fd , "\t");
		/*we took each 32 bits and translate each 8 bits (fron array of chars) to hexadecimal number*/
		for(i = 0; i < length ; i++)
		{
			fprintf(fd, "%02X", s[i]&0XFF);
			fprintf(fd, "\t");
		}
		fprintf(fd, "\n");
		
		free(IC_address);
		free(s);
	}
	fclose(bin);
	
	*fdOB = fd;
}

