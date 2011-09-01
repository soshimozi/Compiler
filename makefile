LEX=flex
LEXFLAGS=
YACC=bison
YACCFLAGS=-v -t -d
CC=gcc
CFLAGS=-ggdb
LIBS=
PARSER=parser
SCANNER=scanner
OBJS=$(SCANNER).o $(PARSER).o
PROGRAM=compiler

all: $(OBJS)
	$(CC) $(LIBS) $(OBJS) -o $(PROGRAM)

$(PARSER).o: $(PARSER).c $(PARSER).h
	$(CC) $(CFLAGS) $(PARSER).c -c

$(SCANNER).o: $(SCANNER).c $(PARSER).h
	$(CC) $(CFLAGS) $(SCANNER).c -c

$(SCANNER).c: $(SCANNER).l
	$(LEX) $(LEXFLAGS) -o$(SCANNER).c $(SCANNER).l

$(PARSER).h: $(PARSER).y
	$(YACC) $(YACCFLAGS) $(PARSER).y -o $(PARSER).c

clean:
	rm -rf $(SCANNER).c $(PARSER).c $(PARSER).h *.o $(PROGRAM)

