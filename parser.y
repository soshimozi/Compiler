%{
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "compiler.h"

/* prototypes */
nodeType *opr(int oper, int nops, ...);
nodeType *id(int id);
nodeType *con(int value);

void freeNode(nodeType *p);

int ex(nodeType *p);

int yylex(void);

void yyerror(char *s);
int sym[26];		/* symbol table */

%}

%error-verbose

%union {
	int iValue;	
	char sIndex;
	nodeType *nPtr;
};


%token <iValue> INTVAL
%token <sIndex> IDENTIFIER
%type <nPtr> stmt expr stmt_list
%token WHILE IF PRINT T_PROGRAM VAR T_COMMA
%token TYPE_INT TYPE_CHAR
%nonassoc IFX
%nonassoc ELSE

%left GE LE EQ NE '>' '<'
%left '+' '-'
%left '*' '/'
%nonassoc UMINUS


%%

program:
	T_PROGRAM ident ';'  var_decl statement_block { exit(0); }
	;

var_decl:
	/* empty */
	|
	type identlist ';' var_decl
	;

identlist:
	ident
	| ident T_COMMA identlist
	;

ident:
	IDENTIFIER
	;

statement_block:
	statement_block stmt
	| /* NULL */
	;
	
type:
	TYPE_INT 
	| TYPE_CHAR
	;
	
stmt:
	';'			{ $$ = opr(';', 2, NULL, NULL); }
	| expr ';'		{ $$= $1; }
	| PRINT expr ';'	{ $$ = opr(PRINT, 1, $2); }
	| IDENTIFIER '=' expr ';' { $$ = opr('=', 2, id($1), $3); }
	| WHILE '(' expr ')' stmt 
				{ $$ = opr(WHILE, 2, $3, $5); }
	| IF '(' expr ')' stmt %prec IFX 
				{ $$ = opr(IF, 2, $3, $5); printf("Processing If statement.\n"); }
	| IF '(' expr ')' stmt ELSE stmt
				{ $$ = opr(IF, 3, $3, $5, $7); printf("Processing if/else statement.\n"); }
	| '{' stmt_list '}'	{ $$ = $2; }
	;

stmt_list:
	stmt	{ $$ = $1; }
	| stmt_list stmt { $$ = opr(';', 2, $1, $2); }
	;

expr:
	INTVAL			{ $$ = con($1); }
	| IDENTIFIER		{ $$ = id($1); }
	| '-' expr %prec UMINUS { $$ = opr(UMINUS, 1, $2); }
	| expr '+' expr		{ $$ = opr('+', 2, $1, $3); }
	| expr '-' expr		{ $$ = opr('-', 2, $1, $3); }
	| expr '*' expr		{ $$ = opr('*', 2, $1, $3); }
	| expr '/' expr		{ $$ = opr('/', 2, $1, $3); }
	| expr '<' expr		{ $$ = opr('<', 2, $1, $3); }
	| expr '>' expr		{ $$ = opr('>', 2, $1, $3); }
	| expr GE expr		{ $$ = opr(GE, 2, $1, $3); }
	| expr LE expr		{ $$ = opr(LE, 2, $1, $3); }
	| expr NE expr		{ $$ = opr(NE, 2, $1, $3); }
	| expr EQ expr		{ $$ = opr(EQ, 2, $1, $3); }
	| '(' expr ')'		{ $$ = $2; }
	;
%%

#define SIZEOF_NODETYPE ((char *)&p->con - (char *)p)

nodeType *con(int value) {
	nodeType *p;
	size_t nodeSize;

	/* allocate node */
	nodeSize = SIZEOF_NODETYPE + sizeof(conNodeType);
	if ((p = malloc(nodeSize)) == NULL)
		yyerror("out of memory");

	/* copy information */
	p->type = typeCon;
	p->con.value = value;

	return p;
}

nodeType *id(int i) {
	nodeType *p;
	size_t nodeSize;

	/* allocate node */
	nodeSize = SIZEOF_NODETYPE + sizeof(idNodeType);
	if ((p = malloc(nodeSize)) == NULL)
		yyerror("out of memory");

	/* copy information */
	p->type = typeId;
	p->id.i = i;

	return p;
}


nodeType *opr(int oper, int nops, ...) {
	va_list ap;
	nodeType *p;
	size_t nodeSize;
	int i;

	/* allocate node */
	nodeSize = SIZEOF_NODETYPE + sizeof(oprNodeType) +
		(nops - 1) * sizeof(nodeType*);
	if ((p = malloc(nodeSize)) == NULL)
		yyerror("out of memory");

	/* copy information */
	p->type = typeOpr;
	p->opr.oper = oper;
	p->opr.nops = nops;
	va_start(ap, nops);
	for (i = 0; i < nops; i++)
		p->opr.op[i] = va_arg(ap, nodeType*);
	va_end(ap);
	return p;
}

void freeNode(nodeType *p) {
	int i;

	if (!p) return;
	if (p->type == typeOpr) {
		for (i=0; i<p->opr.nops; i++)
			freeNode(p->opr.op[i]);
	}

	free(p);
}

void yyerror(char *s) {
	fprintf(stderr, "%s\n", s);
}

int main(void) {
	yyparse();
	return 0;
}

int ex(nodeType *p) {
	printf("ex called!\n");
}
