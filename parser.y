%{
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "base.h"
#include "parser.h"
#include "ast.h"
#include "symbol_table.h"

#include "typecheck_visitor.h"
#include "graphprinter_visitor.h"



extern FILE *yyin;

static void yyerror( const char *msg );

static struct AstNode *ast;

%}

%defines
%locations
%pure-parser
%error-verbose

%union {
	char* lexeme;
	int integer;
	int boolean;
	char character;
	int type;
	struct AstNode *astnode;
};

%token <lexeme> IDENTIFIER
%token <type> TYPE_IDENTIFIER
%token <integer> INT_LITERAL
%token <boolean> BOOL_LITERAL
%token <character> CHAR_LITERAL

%token T_WHILE T_IF T_PROGRAM T_FUNCTION T_COLON T_PROC
%token T_COMMA T_SEMICOLON T_LBRACE T_RBRACE T_LPAR T_RPAR
%token T_DO T_DOT T_ASSIGN
%nonassoc IFX
%nonassoc T_ELSE

%left <lexeme> T_BITOR
%left <lexeme> T_LOGICOR
%left <lexeme> T_BITAND
%left <lexeme> T_LOGICAND
%left <lexeme> T_EQ T_NE
%left <lexeme> T_LT T_GT T_LE T_GE
%left <lexeme> T_PLUS T_MINUS
%left <lexeme> T_STAR T_SLASH
%nonassoc UMINUS

%type <astnode> program
%type <astnode> ident
%type <astnode> program_decl
%type <astnode> var_decl_list
%type <astnode> multi_var_decl
%type <astnode> var_decl
%type <astnode> identlist
%type <astnode> single_identifier
%type <astnode> multi_identifier

%type <astnode> proc_func_list
%type <astnode> multi_proc_func_decl
%type <astnode> proc_func_decl
%type <astnode> func_decl
%type <astnode> proc_decl

%type <astnode> param_list
%type <astnode> single_param
%type <astnode> multi_param

%type <type> type

%start program

%%

program:
	program_decl var_decl_list proc_func_list statement_block
	{
		struct AstNode *ast_node;
		ast_node = ast_node_new("Program", PROGRAM, VOID,
					yylloc.last_line, NULL);
		ast_node_add_child(ast_node, $1); // program_decl
		ast_node_add_child(ast_node, $2); // var_decl_list
		ast_node_add_child(ast_node, $3); // proc_func_list
		$$ = ast_node;

		ast = ast_node;
		//ast_node_add_child(ast_node, $3); // statement_block
	}
	;

var_decl_list:
	/* empty */ { $$ = NULL; }
	| multi_var_decl
	{
		struct AstNode *ast_node;
		ast_node = ast_node_new("VarDeclList", VARDECL_LIST, VOID,
					yylloc.last_line, NULL);
		ast_node_add_child(ast_node, $1);
		$$ = ast_node;
	}
	;

multi_var_decl:
	var_decl { $$ = $1; } 
	| var_decl multi_var_decl
	{
		ast_node_add_sibling($1, $2);
		$$ = $1;
	}
	;

var_decl:
	type identlist T_SEMICOLON
	{
	        struct AstNode *ast_node;
        	ast_node = ast_node_new("VarDecl", VARDECL, $1,
                                	yylloc.last_line, NULL);
	        ast_node_add_child(ast_node, $2);
	        $$ = ast_node;
	}
	;

proc_func_list:
	/* empty */ { $$ = NULL; }
	| proc_func_decl multi_proc_func_decl
	{
		struct AstNode *ast_node;
		ast_node = ast_node_new("ProcFuncList", PROCFUNC_LIST, VOID,
					yylloc.last_line, NULL);
		ast_node_add_sibling($1, $2);
		ast_node_add_child(ast_node, $1);

		$$ = ast_node;
	}
	;

multi_proc_func_decl:
	/* empty */ { $$ = NULL; }
	| proc_func_decl multi_proc_func_decl
	{
		ast_node_add_sibling($1, $2);
	}
	;

proc_func_decl:
	proc_decl { $$ = $1; }
	| func_decl { $$ = $1; }
	;

func_decl:
	T_FUNCTION ident T_LPAR param_list T_RPAR T_COLON type T_LBRACE var_decl_list statement_block T_RBRACE
	{	
		Symbol *symtab;
		struct AstNode *ast_node;
		
		ast_node = ast_node_new("FuncDecl", FUNCTION, VOID,
					yylloc.last_line, NULL);
	
		ast_node_add_child(ast_node, $2);	// Identifier
		ast_node_add_child(ast_node, $4);	// ParamList
		ast_node_add_child(ast_node, $9);	// VarDeclList
		//ast_node_add_child(ast_node, $9);	// Statements

		$2->symbol->type = $7;

		ast_node->symbol = symbol_new(NULL);
		
		$$ = ast_node;	
	}
	;

proc_decl:
	T_PROC ident T_LPAR param_list T_RPAR T_LBRACE var_decl_list statement_block T_RBRACE
	{
		Symbol *symtab;
		struct AstNode *ast_node;
		
		ast_node = ast_node_new("ProcDecl", PROCEDURE, VOID,
					yylloc.last_line, NULL);
	
		ast_node_add_child(ast_node, $2);	// Identifier
		//ast_node_add_child(ast_node, $4);	// ParamList
		ast_node_add_child(ast_node, $7);	// VarDeclList
		//ast_node_add_child(ast_node, $9);	// Statements

		ast_node->symbol = symbol_new(NULL);
		
		$$ = ast_node;
	}
	;
	
program_decl:
	T_PROGRAM ident T_SEMICOLON
	{  
		struct AstNode *ast_node;
		
		ast_node = ast_node_new("ProgramDecl", PROGRAM_DECL, VOID,
					yylloc.last_line, NULL);
		ast_node_add_child(ast_node, $2); /* identifier */
		$$ = ast_node;
	}
	;

param_list:
	/* empty */ { $$ = NULL; }
	| single_param multi_param
	{
		struct AstNode *ast_node;
		ast_node = ast_node_new("ParamList", PARAM_LIST, VOID,
					yylloc.last_line, NULL);
		ast_node_add_sibling($1, $2);
		ast_node_add_child(ast_node, $1);
		$$ = ast_node;
	}
	;

multi_param:
	/* empty */ { $$ = NULL; }
	| T_COMMA single_param multi_param
	{
		ast_node_add_sibling($2, $3);
		$$ = $2;
	}
	;
	

single_param:
	type ident
	{
		struct AstNode *ast_node;
		ast_node = ast_node_new("Parameter", PARAMETER, $1,
					yylloc.last_line, NULL);
		ast_node_add_child(ast_node, $2);	// Identifier
		$$ = ast_node;
	}
	;

identlist:
	single_identifier multi_identifier
	{
		struct AstNode *ast_node;

		ast_node = ast_node_new("IdentifierList", IDENT_LIST, VOID,
					yylloc.last_line, NULL);
		ast_node_add_sibling($1, $2);
		ast_node_add_child(ast_node, $1);
		$$ = ast_node;
	}
	;

multi_identifier:
	/* empty */ { $$ = NULL; }
	| T_COMMA single_identifier multi_identifier
	{
		ast_node_add_sibling($2, $3);
		$$ = $2;
	}
	;

single_identifier:
	ident { $$ = $1; }
	;

ident:
	IDENTIFIER 
	{
		struct AstNode *ast_node;
	
		ast_node = ast_node_new("Identifier", IDENTIFIER, VOID,	
					yylloc.last_line, NULL);
		ast_node->symbol = symbol_new($1);
		// ast-node->symbol->decl_linenum = yylloc.last_line;
		$$ = ast_node;	 
	}
	;


statement_block:
	statement_block stmt
	|
	/* empty */
	;
	
type:	TYPE_IDENTIFIER 
	;
	
stmt:
	T_SEMICOLON			{ }
	| expr T_SEMICOLON		{ }
	| IDENTIFIER T_ASSIGN expr T_SEMICOLON { }
	| T_WHILE T_LPAR expr T_RPAR stmt 
				{ }
	| T_DO stmt T_WHILE T_LPAR expr T_RPAR T_SEMICOLON { }
	| T_IF T_LPAR expr T_RPAR stmt %prec IFX 
				{ }
	| T_IF T_LPAR expr T_RPAR stmt T_ELSE stmt
				{ }
	| T_LBRACE stmt_list T_RBRACE	{ }
	;

stmt_list:
	stmt	{ }
	| stmt_list stmt { }
	;

expr:
	INT_LITERAL		{ }
	| CHAR_LITERAL 		{ }
	| BOOL_LITERAL		{ }
	| IDENTIFIER		{ }
	| T_MINUS expr %prec UMINUS { }
	| expr T_PLUS expr		{ }
	| expr T_MINUS expr		{ }
	| expr T_STAR expr		{ }
	| expr T_SLASH expr		{ }
	| expr T_LT expr		{ }
	| expr T_GT expr		{ }
	| expr T_GE expr		{ }
	| expr T_LE expr		{ }
	| expr T_NE expr		{ }
	| expr T_EQ expr		{ }
	| T_LPAR expr T_RPAR		{ }
	;
%%

static void yyerror(const char *msg)
{
	fprintf(stderr, "Error(%d): %s\n", yyget_lineno(),  msg);
}

int main(int argc, char **argv)
{
	if (argc > 1)
		yyin = fopen(argv[1], "r");
	else
		yyin = stdin;

	/*yylloc.first_line = yylloc.last_line = 1;
	yylloc.first_column = yylloc.last_column = 0;*/

	yyparse();

	Visitor *visitor;

	visitor = typecheck_new();
	ast_node_accept(ast, visitor);
	
	if(ast_node_check_errors(ast)) {
		fprintf(stderr, "Too many errors to compile.\n");
	}

	visitor = graphprinter_new();

	ast_node_accept(ast, visitor);

	return 0;
}
