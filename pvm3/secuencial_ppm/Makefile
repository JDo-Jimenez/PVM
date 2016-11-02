all:Mandel

Mandel:Mandel.c Mandel.h funciones.o
	cc -O0 -g -W -Wall -o Mandel Mandel.c funciones.o

funciones.o:funciones.c
	cc -O4 -g -W -Wall -c funciones.c
Mandel.h:
	
clean:
	rm funciones.o Mandel
