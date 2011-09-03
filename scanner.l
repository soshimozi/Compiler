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
"program"	return PROGRAM;
","		return T_COMMA;
"var"		return VAR;
"int"		return TYPE_INT;
"char"		return TYPE_CHAR;
[0-9]+                      { return INTEGER;
                            }
[A-Za-z][A-Za-z0-9_]*       { /*uploc; yylval->lexeme=strdup(yytext);*/
                              return IDENTIFIER;
                            }
[ \t\n]+	; /* ignore whitespace */
.		yyerror("Unknown characters");

%%
int yywrap(void) {
	return 1;
}
