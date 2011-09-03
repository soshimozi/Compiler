%{
#include <stdlib.h>
#include "compiler.h"
#include "parser.h"

void yyerror(char *);

%}

%%

[-()<>=+*/;{}.] {
		return *yytext;
		}

">="		return GE;
"<="		return LE;
"=="		return EQ;
"!="		return NE;
"while"		return WHILE;
"if"		return IF;
"else"		return ELSE;
"print"		return PRINT;
"program"	return T_PROGRAM;
","		return T_COMMA;
"var"		return VAR;
"int"		return TYPE_INT;
"char"		return TYPE_CHAR;
[A-Za-z]			       	{ /*uploc; yylval->lexeme=strdup(yytext);*/
                              return IDENTIFIER;
                            }
[0-9]+                      { return INTVAL;
                            }
[ \t\n]+	; /* ignore whitespace */
.		yyerror("Unknown characters");

%%
int yywrap(void) {
	return 1;
}
