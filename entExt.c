#include "head.h"

/********************************************
 This file was written by:
 # Ron Gabay     -> ID number 205592017
*********************************************/


/*this function gets an address of the ent file and edits it to the file.ent   */
void createEnt(FILE **fd, data* hd, int lenD)
{
	FILE *fdc = (FILE*)* fd;
	char *addr;
	int i = 0;
	
	for(; i < lenD; i++)
	{
		/*searching for symbols that was mentioned as entry*/
		if(strcmp(hd[i].attributes[0], "entry") == 0 || (hd[i].full == 2 && strcmp(hd[i].attributes[1], "entry") == 0))
		{
			addr = (char *)malloc(5 * sizeof(char));
			i_to_a((hd[i].value), &addr);
			fprintf(fdc, "%s", (hd[i].symbol));
			fprintf(fdc, "%c", '\t');
			fprintf(fdc, "%s", addr);
			fprintf(fdc, "%c", '\n');
			free(addr);
		}
	}
	
	*fd = fdc;
}

/*this function gets an address of the ext file and edits it to the file.ext */
void createExt(FILE **fd, code *hc, int lenC)
{
	FILE *fdc = (FILE*)* fd;
	int i = 0;
	
	for(; i< lenC; i++)
	{
		/*searching for code lines that extern symbol was mentioned in it*/
		if(hc[i].is_ext)
		{
			char *addr;
			addr = (char *)malloc(5 *sizeof(char));
			i_to_a((hc[i].address), &addr);
			fprintf(fdc, "%s", (hc[i].ext));
			fprintf(fdc, "%c", '\t');
			fprintf(fdc, "%s", addr);
			fprintf(fdc, "%c", '\n');
			free(addr);
		}
	}
	
	*fd = fdc;
}

