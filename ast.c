#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
//#include "typecheck_visitor.h"

struct AstNode *
ast_node_new(const char* name, int kind, int type,
             int linenum, Symbol *symbol)
{
    int i;
    struct AstNode *node;

    node = (struct AstNode *) malloc (sizeof(struct AstNode));

    if (name != NULL){
        node->name = strdup(name);
    } else
        node->name = NULL;

    node->kind = kind;
    node->type = type;
    node->linenum = linenum;
    node->child_counter = 0;
    node->symbol = symbol;
    node->parent = NULL;
    node->children = NULL;
    node->sibling = NULL;

    return node;
}

void
ast_node_destroy(struct AstNode *self)
{
    if (self != NULL) {
        ast_node_destroy(self->children);
        ast_node_destroy(self->sibling);
        free(self);
    }
}

Value
ast_node_get_value(struct AstNode *self)
{
    if (self->kind == IDENTIFIER)
        return self->symbol->value;

    return self->value;
}

int
ast_node_get_value_as_int(struct AstNode *self)
{
    if (self->kind == IDENTIFIER)
        return self->symbol->value.integer;

    return self->value.integer;
}

int
ast_node_get_child_counter(struct AstNode *self)
{
    return self->child_counter++;
}

bool
ast_node_check_errors(struct AstNode *self)
{
    struct AstNode *child;

    if (self == NULL || self->type == ERROR)
        return TRUE;

    for (child = self->children; child != NULL; child = child->sibling) {
        if (ast_node_check_errors(child) == TRUE)
            return TRUE;
    }

    return FALSE;
}

void
ast_node_add_child(struct AstNode *self, struct AstNode *child)
{
    struct AstNode *temp;

    if (child == NULL)
        return;

    if (self->children == NULL) {
        child->parent = self;
        self->children = child;
    } else {
        ast_node_add_sibling(self->children, child);
    }
    for (temp = child; temp != NULL; temp = temp->sibling)
        temp->parent = self;
}

void
ast_node_add_sibling(struct AstNode *self, struct AstNode *sibling)
{
    struct AstNode *temp;

    if (sibling == NULL)
        return;

    if (self->sibling == NULL) {
        self->sibling = sibling;
    } else {
        for (temp = self->sibling; temp->sibling != NULL; temp = temp->sibling)
            ;
        temp->sibling = sibling;
    }
}

/*
void
ast_node_accept(struct AstNode *self, Visitor *visitor)
{
    VisitFunc visit;

    if (self == NULL)
        return;

    self->child_counter = 1;

    switch (self->kind) {
        case PROGRAM:
            visit = visitor->visit_program;
            break;
        case PROGRAM_DECL:
            visit = visitor->visit_programdecl;
            break;
        case VARDECL_LIST:
            visit = visitor->visit_vardecl_list;
            break;
        case VARDECL:
            visit = visitor->visit_vardecl;
            break;
        case IDENT_LIST:
            visit = visitor->visit_identifier_list;
            break;
        case PROCFUNC_LIST:
            visit = visitor->visit_procfunc_list;
            break;
        case PROCEDURE:
            visit = visitor->visit_procedure;
            break;
        case FUNCTION:
            visit = visitor->visit_function;
            break;
        case PARAM_LIST:
            visit = visitor->visit_param_list;
            break;
        case PARAMETER:
            visit = visitor->visit_parameter;
            break;
        case STATEMENT_LIST:
            visit = visitor->visit_statement_list;
            break;
        case PRINTINT_STMT:
            visit = visitor->visit_printint_stmt;
            break;
        case PRINTCHAR_STMT:
            visit = visitor->visit_printchar_stmt;
            break;
        case PRINTBOOL_STMT:
            visit = visitor->visit_printbool_stmt;
            break;
        case PRINTLINE_STMT:
            visit = visitor->visit_printline_stmt;
            break;
        case ASSIGNMENT_STMT:
            visit = visitor->visit_assignment_stmt;
            break;
        case IF_STMT:
            visit = visitor->visit_if_stmt;
            break;
        case WHILE_STMT:
            visit = visitor->visit_while_stmt;
            break;
        case FOR_STMT:
            visit = visitor->visit_for_stmt;
            break;
        case REL_EXPR:
            visit = visitor->visit_rel_expr;
            break;
        case ADD_EXPR:
            visit = visitor->visit_add_expr;
            break;
        case MUL_EXPR:
            visit = visitor->visit_mul_expr;
            break;
        case NOTFACTOR:
            visit = visitor->visit_notfactor;
            break;
        case CALL:
            visit = visitor->visit_call;
            break;
        case CALLPARAM_LIST:
            visit = visitor->visit_callparam_list;
            break;
        case CALLPARAM:
            visit = visitor->visit_callparam;
            break;
        case IDENTIFIER:
            visit = visitor->visit_identifier;
            break;
        case INT_LITERAL:
        case BOOL_LITERAL:
        case CHAR_LITERAL:
            visit = visitor->visit_literal;
            break;
        case T_PLUS:
        case T_MINUS:
        case T_OR:
            visit = visitor->visit_add_op;
            break;
        case T_STAR:
        case T_SLASH:
        case T_AND:
            visit = visitor->visit_mul_op;
            break;
        case T_EQUAL:
        case T_NOTEQUAL:
        case T_LESSER:
        case T_GREATER:
        case T_LESSEREQUAL:
        case T_GREATEREQUAL:
            visit = visitor->visit_rel_op;
            break;
        case T_NOT:
            visit = visitor->visit_not_op;
            break;
        default:
            visit = NULL;
    }

    if (visit != NULL)
        visit(visitor, self);
}

void
ast_node_accept_children(struct AstNode *self, Visitor *visitor)
{
    struct AstNode *temp;
    for (temp = self; temp != NULL; temp = temp->sibling)
        ast_node_accept(temp, visitor);
}
*/
