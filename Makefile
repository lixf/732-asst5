#--------------------------------------------------------------------------------
#
# Makefile for the calculator
# Programmer: Leonidas Fegaras
# Date: 12/18/97
#
#--------------------------------------------------------------------------------


GCC = gcc
CFLAGS = -g
YACC = bison -v -d
LEX = flex

simple:	main.c simple.o ast.o tables.o eval.o
	$(GCC) $(CFLAGS)  main.c simple.o ast.o tables.o eval.o -o simple

ast.o:  ast.c ast.h
	$(GCC) $(CFLAGS) -c ast.c

tables.o:  tables.c tables.h
	$(GCC) $(CFLAGS) -c tables.c

eval.o:  eval.c eval.h
	$(GCC) $(CFLAGS) -c eval.c

# eval.o:  eval.c eval.h ast.h
# 	 $(GCC) $(CFLAGS) -I$(GCDIR)/include -c eval.c

simple.o: simple.c simple.h simple.yy.c ast.h #eval.h
	$(GCC) $(CFLAGS)  -c simple.c 

%.h %.c: %.y
	$(YACC) simple.y
	mv simple.tab.c simple.c
	mv simple.tab.h simple.h

simple.yy.c: simple.lex simple.y
	   $(LEX) simple.lex
	   mv lex.yy.c simple.yy.c


clean: 
	/bin/rm -f *.o *~ simple.yy.c simple.c simple.h simple.output simple core*
	/bin/rm -rf simple.dSYM

