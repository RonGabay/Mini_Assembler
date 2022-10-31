assembler: utilities.o  symbol.o storage.o directive.o code.o firstRead.o secondRead.o ob.o entExt.o main.o
	gcc   -Wall -ansi -pedantic -g firstRead.o secondRead.o symbol.o directive.o code.o utilities.o storage.o ob.o entExt.o main.o -o assembler

utilities.o: utilities.c head.h
	gcc  -c -ansi -Wall -pedantic utilities.c -o utilities.o

symbol.o: symbol.c  head.h 
	gcc  -c -ansi -Wall -pedantic symbol.c -o symbol.o

storage.o: storage.c  head.h
	gcc  -c -ansi -Wall -pedantic storage.c -o storage.o

directive.o: directive.c  head.h
	gcc  -c -ansi -Wall -pedantic directive.c -o directive.o

code.o: code.c  head.h
	gcc  -c -ansi -Wall -pedantic code.c -o code.o


firstRead.o: firstRead.c  head.h
	gcc  -c -ansi -Wall -pedantic firstRead.c -o firstRead.o

secondRead.o: secondRead.c  head.h
	gcc  -c -ansi -Wall -pedantic secondRead.c -o secondRead.o

ob.o: ob.c  head.h
	gcc  -c -ansi -Wall -pedantic ob.c -o ob.o
	
entExt.o: entExt.c  head.h
	gcc  -c -ansi -Wall -pedantic entExt.c -o entExt.o


main.o: main.c  head.h 
	gcc -c -ansi -Wall -pedantic main.c -o main.o
