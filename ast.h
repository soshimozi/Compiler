#ifndef AST_H
#define AST_H

#include "compiler.h"
#include "base.h"
#include "parser.h"
#include "symbol_table.h"

struct AstNode {
    char *name;
    int kind;
    Type type;
    Value value;
    Symbol *symbol;
    int linenum;
    int child_counter;
    struct AstNode* parent;
    struct AstNode* children;
    struct AstNode* sibling;
};

typedef struct _Visitor {
    void (*visit_program) (struct _Visitor *, struct AstNode *);
    void (*visit_programdecl) (struct _Visitor *, struct AstNode *);
    void (*visit_vardecl_list) (struct _Visitor *, struct AstNode *);
    void (*visit_vardecl) (struct _Visitor *, struct AstNode *);
    void (*visit_identifier_list) (struct _Visitor *, struct AstNode *);
    void (*visit_identifier) (struct _Visitor *, struct AstNode *);
    void (*visit_procfunc_list) (struct _Visitor *, struct AstNode *);
    void (*visit_procedure) (struct _Visitor *, struct AstNode *);
    void (*visit_function) (struct _Visitor *, struct AstNode *);
    void (*visit_param_list) (struct _Visitor *, struct AstNode *);
    void (*visit_parameter) (struct _Visitor *, struct AstNode *);
    void (*visit_statement_list) (struct _Visitor *, struct AstNode *);
    void (*visit_printint_stmt) (struct _Visitor *, struct AstNode *);
    void (*visit_printchar_stmt) (struct _Visitor *, struct AstNode *);
    void (*visit_printbool_stmt) (struct _Visitor *, struct AstNode *);
    void (*visit_printline_stmt) (struct _Visitor *, struct AstNode *);
    void (*visit_assignment_stmt) (struct _Visitor *, struct AstNode *);
    void (*visit_if_stmt) (struct _Visitor *, struct AstNode *);
    void (*visit_while_stmt) (struct _Visitor *, struct AstNode *);
    void (*visit_for_stmt) (struct _Visitor *, struct AstNode *);
    void (*visit_rel_expr) (struct _Visitor *, struct AstNode *);
    void (*visit_add_expr) (struct _Visitor *, struct AstNode *);
    void (*visit_mul_expr) (struct _Visitor *, struct AstNode *);
    void (*visit_notfactor) (struct _Visitor *, struct AstNode *);
    void (*visit_call) (struct _Visitor *, struct AstNode *);
    void (*visit_callparam_list) (struct _Visitor *, struct AstNode *);
    void (*visit_callparam) (struct _Visitor *, struct AstNode *);
    void (*visit_literal) (struct _Visitor *, struct AstNode *);
    void (*visit_add_op) (struct _Visitor *, struct AstNode *);
    void (*visit_mul_op) (struct _Visitor *, struct AstNode *);
    void (*visit_rel_op) (struct _Visitor *, struct AstNode *);
    void (*visit_not_op) (struct _Visitor *, struct AstNode *);
} Visitor;

typedef void (*VisitFunc)(struct _Visitor *, struct AstNode *);

struct AstNode *ast_node_new(const char *name, int kind, int type,
                             int linenum, Symbol *symbol);
void ast_node_destroy(struct AstNode *);

Value ast_node_get_value(struct AstNode *);
int ast_node_get_value_as_int(struct AstNode *);
int ast_node_get_child_counter(struct AstNode *);
bool ast_node_check_errors(struct AstNode *);
void ast_node_add_child(struct AstNode *, struct AstNode *);
void ast_node_add_sibling(struct AstNode *, struct AstNode *);
void ast_node_accept(struct AstNode *, Visitor *);
void ast_node_accept_children(struct AstNode *, Visitor *);

#endif // AST_H
