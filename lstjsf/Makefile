CC = gcc 
OBJECTS = lstjsf.c  utils.c
## CFLAGS=-g -m32 $(OPTFLAGS) -Wall -Wimplicit -Wimplicit-int -Wimplicit-function-declaration -W -Wstrict-prototypes -Wnested-externs  
CFLAGS=-g $(OPTFLAGS) -Wall -Wimplicit -Wimplicit-int -Wimplicit-function-declaration -W -Wstrict-prototypes -Wnested-externs  
LIBS = -lm


lstjsf:$(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) $(LIBS) -o lstjsf


