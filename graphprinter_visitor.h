#ifndef GRAPHPRINTER_VISITOR_H
#define GRAPHPRINTER_VISITOR_H

#include "ast.h"

#define COLOR_FILL_GLOBAL  "\"#EEEEEE\""
#define COLOR_FILL_LOCAL   "\"#CAE7FF\""
#define COLOR_FILL_COMMON  "\"#EEFFEE\""
#define COLOR_FILL_LITERAL "\"#FFFFCC\""
#define COLOR_FILL_SYMBOL  "\"#CCFF99\""
#define COLOR_FILL_ERROR   "\"#FFEEEE\""

#define COLOR_EDGE_GROUP   "\"#22DDAA\""
#define COLOR_EDGE_FUNCT   "\"#EEEEFF\""
#define COLOR_EDGE_ERROR   "\"#FF0000\""

Visitor *graphprinter_new();

void graphprinter_visit_program (struct _Visitor *, struct AstNode *);
void graphprinter_visit_programdecl (struct _Visitor *, struct AstNode *);
void graphprinter_visit_vardecl_list (struct _Visitor *, struct AstNode *);
void graphprinter_visit_identifier_list (struct _Visitor *, struct AstNode *);
void graphprinter_visit_procfunc_list (struct _Visitor *, struct AstNode *);
void graphprinter_visit_procfunc (struct _Visitor *, struct AstNode *);
void graphprinter_visit_param_list (struct _Visitor *, struct AstNode *);
void graphprinter_visit_parameter (struct _Visitor *, struct AstNode *);
void graphprinter_visit_statement_list (struct _Visitor *, struct AstNode *);
void graphprinter_visit_binary_expr (struct _Visitor *, struct AstNode *);
void graphprinter_visit_call (struct _Visitor *, struct AstNode *);
void graphprinter_visit_callparam_list (struct _Visitor *, struct AstNode *);
void graphprinter_visit_callparam (struct _Visitor *, struct AstNode *);
void graphprinter_visit_identifier (struct _Visitor *, struct AstNode *);
void graphprinter_visit_literal (struct _Visitor *, struct AstNode *);

void graphprinter_visit_simplenode (struct _Visitor *, struct AstNode *);

#endif // GRAPHPRINTER_VISITOR_H
