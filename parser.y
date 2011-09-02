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
	char *lexeme;
	int iValue;	/* integer value */
	char sIndex;	/* symbol table index */
	nodeType *nPtr;	/* node pointer */
};

%token <iValue> INTEGER
%token <lexeme> VARIABLE
%token WHILE IF PRINT PROGRAM T_VAR T_COMMA T_SEMICOLON
%token T_LBRACE T_RBRACE T_LBRACKET T_RBRACKET T_LPAREN T_RPAREN T_ASSIGN
%token T_DOT
%nonassoc IFX
%nonassoc ELSE


%left GE LE EQ NE GT LT
%left PLUS MINUS
%left MUL DIV
%nonassoc UMINUS

%type <nPtr> stmt expr stmt_list

%%

program:
	function { exit(0); }
	;
function:
	function stmt	{ ex($2); freeNode($2); }
	| /* NULL */
	;

stmt:
	T_SEMICOLON			{ $$ = opr(';', 2, NULL, NULL); }
	| expr T_SEMICOLON		{ $$= $1; }
	| PRINT expr T_SEMICOLON	{ $$ = opr(PRINT, 1, $2); }
	| VARIABLE T_ASSIGN expr T_SEMICOLON { $$ = NULL; /*opr('=', 2, id($1), $3);*/ }
	| WHILE T_LPAREN expr T_RPAREN stmt 
				{ $$ = opr(WHILE, 2, $3, $5); }
	| IF T_LPAREN expr T_RPAREN stmt %prec IFX 
				{ $$ = opr(IF, 2, $3, $5); }
	| IF T_LPAREN expr T_RPAREN stmt ELSE stmt
				{ $$ = opr(IF, 3, $3, $5, $7); }
	| T_LBRACKET  stmt_list T_RBRACKET	{ $$ = $2; }
	;

stmt_list:
	stmt	{ $$ = $1; }
	| stmt_list stmt { $$ = opr(';', 2, $1, $2); }
	;

expr:
	INTEGER			{ $$ = con($1); }
	| VARIABLE		{ $$ = NULL /* id($1);*/ }
	| MINUS expr %prec UMINUS { $$ = opr(UMINUS, 1, $2); }
	| expr PLUS expr		{ $$ = opr('+', 2, $1, $3); }
	| expr MINUS expr		{ $$ = opr('-', 2, $1, $3); }
	| expr MUL expr		{ $$ = opr('*', 2, $1, $3); }
	| expr DIV expr		{ $$ = opr('/', 2, $1, $3); }
	| expr LT  expr		{ $$ = opr('<', 2, $1, $3); }
	| expr GT expr		{ $$ = opr('>', 2, $1, $3); }
	| expr GE expr		{ $$ = opr(GE, 2, $1, $3); }
	| expr LE expr		{ $$ = opr(LE, 2, $1, $3); }
	| expr NE expr		{ $$ = opr(NE, 2, $1, $3); }
	| expr EQ expr		{ $$ = opr(EQ, 2, $1, $3); }
	| T_LPAREN  expr T_RPAREN		{ $$ = $2; }
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
