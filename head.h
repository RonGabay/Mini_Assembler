#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <limits.h>

/********************************************
 This file was written by:
 # Ron Gabay        -> ID number 205592017
*********************************************/


/***** ENUMS *****/
/* enum of a boolean represantation */ 
typedef enum {false, true} bool;

/* enum of a status that represent in which case we are at the command line.
 * this enum is used in the files: utilities.c,   */	
enum status {SPACE_FIRST=100, SPACE_MIDDLE=101, DIGIT=102, COMMA=103, 
	     NAME=104, QUOT_MARKS_FIRST = 105, QUOT_MARKS_END=106, REG = 107, 
	     LABEL = 108, REG_FIRST = 10, REG_SECOND = 11, REG_THIRD = 12, IMMED = 13};
 

/***** STRUCTS *****/
/* sctruct that build lists for R,I,J */     
typedef struct {
	char name [5];
	int opcode;
	int func;
} item;

/* struct for Data */
typedef struct data{
	char *symbol;
	int value;
	char attributes[2][9];
	int full;
} data;

/* struct for Code */
typedef struct code {
	int address;
	char *sourceCode;
	int machineCode;
	int label_exist;
	int is_ext;
	char *ext;
	char *name;
	char group;
	int num_of_group;
} code;

/* struct for Directive */
typedef struct directive {
	int address;
	char *directive;
	long machineCode;
} directive;


/***** MACROS *****/
/* macro that checks white spaces (space or tab) */
#define SPACE(x) (((x) == ' ' || (x) == '\t')) ? true : false

/* macro that checks the range values */
#define RANGE(i, min, max) (i < min) || (i > max) ? true : false

/* macro that checks if the status that returned is DIGIT */
#define DIGIT_NUM(x) ((x) != 102) ? true : false

/***** CONTSTANTS *****/
/* creating a table list for R,I,J types.
 * this table list is filled in utillities.c file */
extern const item list[];

/* constant return values */
#define FALSELY 0
#define TRUELY 1
#define NONE -1

/* define lengths */
#define LENGTH_LINE 80
#define FIRST_ADDRESS 100
#define LENGTH_OF_NUM 10
#define SYMBOL 10
#define CODE 5
#define DIRECTIVE 7
#define NUM_OF_TOKENS 6
#define JUMPING 0
#define IC 4

/*define ranges for index at list
  F = first index      L = last index*/
#define R1F 0
#define R1L 4
#define R2F 5
#define R2L 7

#define I1F 8
#define I1L 12
#define I2F 13
#define I2L 16
#define I3F 17
#define I3L 22

#define JF 23
#define JL 26


/* define type of errors */
#define WARNING "can't write label before .exteren or .entry, in line "
#define ERR_MEMORY "Memory allocation failed"
#define ERR_SYNTAX "Syntax error occure, in line "
#define ERR_DUPLICATE " is already existed, in line "
#define ERR_SYMBOL "The symbol was not written correctly, in line "
#define ERR_SYMBOL_CODE " is already existed as a name of code, in line "
#define ERR_SYMBOL_EXIST " is already configured at the code, in line "
#define ERR_SYMBOL_DIRECT " is already existed as a name of directive, in line "
#define ERR_DIRECTIVE "unknown directive name, in line "
#define ERR_RANGE "is not in the range of that directive, in line "
#define ERR_RANGE_CODE "the number of the register is not the range, in line "
#define ERR_CODE "invalid name of code, in line "
#define ERR_LABEL "Unknown label name, in line "

/* define Type */
#define sizeR 8
#define sizeI 15
#define sizeJ 4

/* define Func */
#define addF 1
#define subF 2 
#define andF 3
#define orF 4
#define norF 5

#define moveF 1
#define mvhiF 2
#define mvloF 3

/* define Opcode */
#define add 0
#define sub 0 
#define and 0
#define or 0
#define nor 0

#define move 1
#define mvhi 1
#define mvlo 1

#define addi 10
#define subi 11
#define andi 12
#define ori 13
#define nori 14
#define bne 15
#define beq 16
#define blt 17
#define bgt 18
#define lb 19
#define sb 20
#define lw 21
#define sw 22
#define lh 23
#define sh 24

#define jmp 30
#define la 31
#define call 32
#define stop 63


/**** FUNCTIONS DECLARATIONS ****/
/* utilities.c */
void free_all_images();
int searchAtList(char *, int , int);
bool isWhiteSpaces(char *);
int checkRange(char *, long, int **);
int num_of_digits(int);
void i_to_a(int, char **);
void whatStatus(int *, char);
int immedSituation(char **, int, int ***);
int spaceFirstCase(char, int, char);
int spaceMiddleCase(char);
int commaCase(char *, char);
int digitCase(char *, char, int *, char **, int *, int);
int regCase(char);

/* symbol.c */
int readSymbol(char**, char**);
int checkSymbol(char*, data *, int,  char **);

/* storage.c */
void createNodeForData(data **, int);
void createNodeForCode(code **, int);
void createNodeForDirective(directive **, int);
bool addToData(char *,int , char *, data **, int **);
void addToDirective(int , char *, directive **, void * , int** );
void addToCode(int , char *, int , char *, char , int , code **, int **, int);

/* directive.c */
int synDhDwDb(char *, char *, int **, long **);
int synExtEnt(char *, char *, char **);	
int synAsciz(char *, char *, int **, char **);
int readDirective(char **, char **, char **);
int checkDirectiveLine(char *, char *, int *, long **, char **);

/* code.c */
char whichType(char *);
int smCode(char ,char * ,char);
int labelCode(char);
int commaCode(int ,char , int, char **, int ** );
char checkCode(char **, char **);
int validCode(char *, char, int, int *);

/* firstRead.c */
void changeAddress(data **, directive **, int, int, int);
int firstRead(FILE *, data **, directive **, code **, int *, int *, int *, int *, int *);

/* secondRead.c */
void buildToken(code *, char ***);
int binaryR(int, int, int, int ,int);
int binaryI(int, int, int ,int);
int binaryJ(int, int, int);
int findReg(char *);
int checkLabel(char *, data *, int);
bool codeBinaryChange(code *, data *, int);
bool secondRead(data *, code **, int, int);

/* ob.c */
void createFile(FILE **, directive *, code *, int, int);
void createOB(FILE **, directive *, code *, int, int);

/* entExt.c */
void createEnt(FILE **, data *, int);
void createExt(FILE **, code *, int);

/* main.c */
int main(int, char **);
/*********************************************************/

