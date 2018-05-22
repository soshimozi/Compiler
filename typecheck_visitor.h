#ifndef TYPECHECK_VISITOR_H
#define TYPECHECK_VISITOR_H

#include "ast.h"
#include "base.h"
#include "symbol_table.h"

//static bool is_vardecl = FALSE;
static Type declared_type = VOID;
static Symbol *symtab;
static Symbol *global_symtab;

Visitor *typecheck_new();

void typecheck_visit_program (struct _Visitor *, struct AstNode *);
void typecheck_visit_programdecl (struct _Visitor *, struct AstNode *);
void typecheck_visit_procfunc_list (struct _Visitor *, struct AstNode *);
void typecheck_visit_procfunc(struct _Visitor *, struct AstNode *);
void typecheck_visit_vardecl_list (struct _Visitor *, struct AstNode *);
void typecheck_visit_vardecl (struct _Visitor *, struct AstNode *);
void typecheck_visit_param_list(struct _Visitor *, struct AstNode *);
void typecheck_visit_parameter (struct _Visitor *, struct AstNode *);
void typecheck_visit_statement_list(struct _Visitor *, struct AstNode *);
void typecheck_visit_printint_stmt (struct _Visitor *, struct AstNode *);
void typecheck_visit_printchar_stmt (struct _Visitor *, struct AstNode *);
void typecheck_visit_printbool_stmt (struct _Visitor *, struct AstNode *);
void typecheck_visit_printline_stmt (struct _Visitor *, struct AstNode *);
void typecheck_visit_assignment_stmt (struct _Visitor *, struct AstNode *);
void typecheck_visit_if_stmt (struct _Visitor *, struct AstNode *);
void typecheck_visit_while_stmt (struct _Visitor *, struct AstNode *);
void typecheck_visit_for_stmt (struct _Visitor *, struct AstNode *);
void typecheck_visit_binary_expr (struct _Visitor *, struct AstNode *);
void typecheck_visit_notfactor (struct _Visitor *, struct AstNode *);
void typecheck_visit_call (struct _Visitor *, struct AstNode *);
void typecheck_visit_callparam_list (struct _Visitor *, struct AstNode *);
void typecheck_visit_callparam (struct _Visitor *, struct AstNode *);
void typecheck_visit_identifier_list(struct _Visitor *, struct AstNode *);
void typecheck_visit_identifier (struct _Visitor *, struct AstNode *);

#endif // TYPECHECK_VISITOR_H
