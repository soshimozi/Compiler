%{
#include <stdlib.h>
#include "compiler.h"
#include "parser.h"

void yyerror(char *);

%}

%%

0	{
		yylval.iValue = atoi(yytext);
		return INTEGER;
	}

[1-9][0-9]* 	{
			yylval.iValue = atoi(yytext);
			return INTEGER;
		}

"program"	return PROGRAM;
"var"		return T_VAR;
"while"		return WHILE;
"if"		return IF;
"else"		return ELSE;
"print"		return PRINT;
","		return T_COMMA;
";"		return T_SEMICOLON;
"."		return T_DOT;
"("		return T_LPAREN;
")"		return T_RPAREN;
"["		return T_LBRACE;
"]"		return T_RBRACE;
"}"		return T_RBRACKET;
"{"		return T_LBRACKET;
">"		return GT;
"<"		return LT;
">="		return GE;
"<="		return LE;
"=="		return EQ;
"!="		return NE;
"+"		return PLUS;
"-"		return MINUS;
"*"		return MUL;
"/"		return DIV;
"="		return T_ASSIGN;

[A-Za-z][A-Za-z0-9_]*
	{
		yylval->lexeme=strdup(yytext);
		return VARIABLE;
	}

[ \t\n]+	; /* ignore whitespace */
.		yyerror("Unknown characters");

%%
int yywrap(void) {
	return 1;
}
