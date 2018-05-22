#include <stdlib.h>
#include <stdio.h>
#include "typecheck_visitor.h"

static struct AstNode *_inside_procfunc = NULL;
static void _typecheck_print_stmt(struct AstNode *node, Type type, const char *ptype_str);
static Symbol *_complete_symbol_lookup(Symbol *sym);

Visitor *
typecheck_new()
{
    Visitor *visitor = (Visitor *) malloc (sizeof(Visitor));

    visitor->visit_program = &typecheck_visit_program;
    visitor->visit_programdecl = &typecheck_visit_programdecl;
    visitor->visit_vardecl_list = &typecheck_visit_vardecl_list;
    visitor->visit_vardecl = &typecheck_visit_vardecl;
    visitor->visit_procfunc_list = &typecheck_visit_procfunc_list;
    visitor->visit_procedure = &typecheck_visit_procfunc;
    visitor->visit_function = &typecheck_visit_procfunc;
    visitor->visit_param_list = &typecheck_visit_param_list;
    visitor->visit_parameter = &typecheck_visit_parameter;
    visitor->visit_statement_list = &typecheck_visit_statement_list;
    visitor->visit_printint_stmt = &typecheck_visit_printint_stmt;
    visitor->visit_printchar_stmt = &typecheck_visit_printchar_stmt;
    visitor->visit_printbool_stmt = &typecheck_visit_printbool_stmt;
    visitor->visit_printline_stmt = NULL;
    visitor->visit_assignment_stmt = &typecheck_visit_assignment_stmt;
    visitor->visit_if_stmt = &typecheck_visit_if_stmt;
    visitor->visit_while_stmt = &typecheck_visit_while_stmt;
    visitor->visit_for_stmt = &typecheck_visit_for_stmt;
    visitor->visit_rel_expr = &typecheck_visit_binary_expr;
    visitor->visit_add_expr = &typecheck_visit_binary_expr;
    visitor->visit_mul_expr = &typecheck_visit_binary_expr;
    visitor->visit_notfactor = &typecheck_visit_notfactor;
    visitor->visit_call = &typecheck_visit_call;
    visitor->visit_callparam_list = &typecheck_visit_callparam_list;
    visitor->visit_callparam = &typecheck_visit_callparam;
    visitor->visit_identifier_list = &typecheck_visit_identifier_list;
    visitor->visit_identifier = &typecheck_visit_identifier;
    visitor->visit_literal = NULL;
    visitor->visit_add_op = NULL;
    visitor->visit_mul_op = NULL;
    visitor->visit_rel_op = NULL;
    visitor->visit_not_op = NULL;

    return visitor;
}

void
typecheck_visit_program(struct _Visitor *visitor, struct AstNode *node)
{
    node->symbol = symbol_new(NULL);
    global_symtab = node->symbol;
    symtab = global_symtab;
    _inside_procfunc = NULL;
    ast_node_accept_children(node->children, visitor);
}

void
typecheck_visit_programdecl (struct _Visitor *visitor, struct AstNode *node)
{
    node->children->symbol->decl_linenum = node->linenum;

    //struct AstNode *child;
    //for (child = node->children; child != NULL; child = child->sibling) {
    //    ast_node_accept(child, visitor);
    
   ast_node_accept(node->children, visitor);
}

void
typecheck_visit_procfunc_list (struct _Visitor *visitor, struct AstNode *node)
{
    symtab = global_symtab;

    ast_node_accept_children(node->children, visitor);

    symtab = global_symtab;
    _inside_procfunc = NULL;
}

void
typecheck_visit_vardecl_list (struct _Visitor *visitor, struct AstNode *node)
{
    struct AstNode *child;

    for (child = node->children; child != NULL; child = child->sibling)
        ast_node_accept(child, visitor);
}

void
typecheck_visit_vardecl (struct _Visitor *visitor, struct AstNode *node)
{
    node->children->type = node->type;
    ast_node_accept(node->children, visitor);
}

void
typecheck_visit_procfunc (struct _Visitor *visitor, struct AstNode *node)
{
    struct AstNode *ident;
    struct AstNode *child;

    _inside_procfunc = node;

    // Identifier
    symtab = global_symtab;
    ident = node->children;
    ident->type = node->type;

    // params > -1 == identificador eh funcao
    ident->symbol->params = 0;

    ident->symbol->decl_linenum = node->linenum;

    ast_node_accept(ident, visitor);

    // ParamList, VarDeclList, Statements
    symtab = node->symbol;
    for (child = ident->sibling; child != NULL; child = child->sibling) {
        if (child->kind == PARAM_LIST)
            child->symbol = ident->symbol;
        ast_node_accept(child, visitor);
    }
}

void
typecheck_visit_param_list(struct _Visitor *visitor, struct AstNode *node)
{
	fprintf(stderr, "About to visit param_list\n");
	fprintf(stderr, "node: %x, node->symbol: %x\n", (unsigned)node, (unsigned)node->symbol);

    int i;
    struct AstNode *child;

    node->child_counter = 0;
    for (child = node->children; child != NULL; child = child->sibling) {
        ast_node_accept(child, visitor);
        node->child_counter++;
    }

    symbol_create_params(node->symbol, node->child_counter);

    i = 0;
    for (child = node->children; child != NULL; child = child->sibling) {
        node->symbol->param_types[i] = child->type;
        i++;
    }
}

void
typecheck_visit_parameter (struct _Visitor *visitor, struct AstNode *node)
{
    node->children->type = node->type;
    node->children->symbol->decl_linenum = node->linenum;
    ast_node_accept(node->children, visitor);
}

void
typecheck_visit_statement_list(struct _Visitor *visitor, struct AstNode *node)
{
    ast_node_accept_children(node->children, visitor);
}

void
typecheck_visit_printint_stmt (struct _Visitor *visitor, struct AstNode *node)
{
    ast_node_accept(node->children, visitor);
    _typecheck_print_stmt(node, INTEGER, "Int");
}

void
typecheck_visit_printchar_stmt (struct _Visitor *visitor, struct AstNode *node)
{
    ast_node_accept(node->children, visitor);
    _typecheck_print_stmt(node, CHAR, "Char");
}

void
typecheck_visit_printbool_stmt (struct _Visitor *visitor, struct AstNode *node)
{
    ast_node_accept(node->children, visitor);
    _typecheck_print_stmt(node, BOOLEAN, "Bool");
}

void
typecheck_visit_assignment_stmt (struct _Visitor *visitor, struct AstNode *node)
{
    struct AstNode *lnode = node->children;
    struct AstNode *rnode = lnode->sibling;

    ast_node_accept(lnode, visitor);
    ast_node_accept(rnode, visitor);

    if (symbol_is_procfunc(lnode->symbol) && (_inside_procfunc == NULL ||
        strcmp(_inside_procfunc->children->symbol->name,
               lnode->symbol->name))) {
        node->type = ERROR;
        fprintf(stderr,
                "Error: Symbol '%s' is a function identifier, you cannot "
                "assign a value to it from outside the said function. "
                "Check line %d.\n", lnode->symbol->name, node->linenum);

    } else if (lnode->type != ERROR && rnode->type != ERROR &&
               lnode->type != rnode->type) {
        node->type = ERROR;
        fprintf(stderr,
                "Error: Incompatible types on assignment "
                "operation in line %d.\n", node->linenum);
    }
}

void
typecheck_visit_if_stmt (struct _Visitor *visitor, struct AstNode *node)
{
    struct AstNode *expr = node->children;
    struct AstNode *stmt = expr->sibling;

    ast_node_accept(expr, visitor);
    ast_node_accept(stmt, visitor);

    if (expr->type != BOOLEAN) {
        node->type = ERROR;
        fprintf(stderr,
                "Error: Condition for if statement must be of Boolean type. "
                "Check line %d.\n", node->linenum);
    }
}

void
typecheck_visit_while_stmt (struct _Visitor *visitor, struct AstNode *node)
{
    struct AstNode *expr = node->children;
    struct AstNode *stmt = expr->sibling;

    ast_node_accept(expr, visitor);
    ast_node_accept(stmt, visitor);

    if (expr->type != BOOLEAN) {
        node->type = ERROR;
        fprintf(stderr,
                "Error: Expression in While statement must be of "
                "Boolean type. Check line %d.\n", expr->linenum);
    }
}

void
typecheck_visit_for_stmt (struct _Visitor *visitor, struct AstNode *node)
{
    struct AstNode *asgn = node->children;
    struct AstNode *expr = asgn->sibling;
    struct AstNode *stmt = expr->sibling;
    struct AstNode *id_node = asgn->children;

    ast_node_accept(asgn, visitor);
    ast_node_accept(expr, visitor);

    if (id_node->type != INTEGER) {
        node->type = ERROR;
        fprintf(stderr,
                "Error: Identifier '%s' is of %s type; it must be Integer. "
                "Check line %d.\n", id_node->symbol->name,
                type_get_lexeme(id_node->type), id_node->linenum);
    }

    if (expr->type != INTEGER) {
        node->type = ERROR;
        fprintf(stderr,
                "Error: Value of stop condition is not of Integer type. "
                "Check line %d.\n", expr->linenum);
    }

    ast_node_accept(stmt, visitor);

}

void
typecheck_visit_binary_expr (struct _Visitor *visitor, struct AstNode *node)
{
    struct AstNode *lnode = node->children;
    struct AstNode *op = lnode->sibling;
    struct AstNode *rnode = op->sibling;

    ast_node_accept(lnode, visitor);
    ast_node_accept(rnode, visitor);

    if (lnode->type != ERROR && rnode->type != ERROR &&
        lnode->type != rnode->type) {
        node->type = ERROR;
        fprintf(stderr,
                "Error: Operation '%s' over incompatible types on line %d.\n",
                op->name, op->linenum);
    }
}

void
typecheck_visit_notfactor (struct _Visitor *visitor, struct AstNode *node)
{
    ast_node_accept(node->children, visitor);

    if (node->children->type != BOOLEAN) {
        node->type = ERROR;
        fprintf(stderr,
                "Error: Operation 'not' over non-boolean "
                "operand on line %d.\n", node->linenum);
    }
}

void
typecheck_visit_call (struct _Visitor *visitor, struct AstNode *node)
{
    int params;
    struct AstNode *ident = node->children;
    struct AstNode *plist = ident->sibling;

    ast_node_accept(ident, visitor);

    if (ident->type == ERROR)
        return;

    node->type = ident->symbol->type;

    if (plist != NULL) {
        // CallParamLists apontam para o simbolo da funcao chamada
        // para obter a quantidade e os tipos esperados dos parametros.
        plist->symbol = ident->symbol;

        ast_node_accept(plist, visitor);
        params = plist->child_counter;

    } else
        params = 0;

    if (params != ident->symbol->params) {
        node->type = ERROR;
        fprintf(stderr, "Error: Expecting %d parameters, received %d. "
                "Check line %d\n",
                ident->symbol->params, params, node->linenum);
    }
}

void
typecheck_visit_callparam_list (struct _Visitor *visitor, struct AstNode *node)
{
    int i;
    struct AstNode *child;

    node->child_counter = 0;
    for (child = node->children; child != NULL; child = child->sibling)
        node->child_counter++;

    if (node->symbol->params != node->child_counter) {
        node->type = ERROR;
        return;
    }

    i = 0;
    for (child = node->children; child != NULL; child = child->sibling) {
        ast_node_accept(child, visitor);

        if (child->type != ERROR &&
            child->type != node->symbol->param_types[i]) {
            node->type = ERROR;
            child->type = node->symbol->param_types[i];

            fprintf(stderr, "Error: Call '%s' on line %d, expecting %s "
                            "on parameter %d (",
                    node->symbol->name, node->linenum,
                    type_get_lexeme(node->symbol->param_types[i]),
                    i + 1);

            if (child->children->kind == IDENTIFIER)
                fprintf(stderr, "'%s'", child->children->symbol->name);
            else
                value_print(stderr, &child->value, child->type);

            fprintf(stderr, "), received %s.\n", type_get_lexeme(child->type));
        }
        i++;
    }
}

void
typecheck_visit_callparam (struct _Visitor *visitor, struct AstNode *node)
{
    ast_node_accept(node->children, visitor);
    node->type = node->children->type;
}

void
typecheck_visit_identifier_list (struct _Visitor *visitor, struct AstNode *node)
{
    struct AstNode *child;

    for (child = node->children; child != NULL; child = child->sibling) {
        child->type = node->type;
        child->symbol->decl_linenum = node->linenum;
        ast_node_accept(child, visitor);
    }
}

void
typecheck_visit_identifier (struct _Visitor *visitor, struct AstNode *node)
{
    Symbol *sym = symbol_lookup(symtab, node->symbol->name);
    Symbol *_sym = sym;

    // O atributo 'decl_linenum' > 0 indica que o identificador referencia
    // a declaracao de uma variavel/procedimento/funcao.

    void __fetch_symbol(struct AstNode *node, Symbol *sym) {
        symbol_table_destroy(node->symbol);
        node->symbol = sym;
        node->type = sym->type;
    }

    if (sym == NULL) {

        // Simbolo possui linha de declaracao: Insira na tabela de simbolos.
        if (node->symbol->decl_linenum > 0) {
            node->symbol->type = node->type;
            node->symbol->is_global = (symtab == global_symtab);

            node->symbol = symbol_insert(symtab, node->symbol);

        } else if ((sym = symbol_lookup(global_symtab, node->symbol->name))
                   != NULL) {
            __fetch_symbol(node, sym);

        // Sem linha de declaracao == eh fetch de variavel nao declarada.
        } else {
            node->symbol->type = node->type = ERROR;
            fprintf(stderr, "Error: Undeclared symbol '%s' in line %d\n",
                    node->symbol->name, node->linenum);
        }

    // Simbolo encontrado na tabela e eh fetch: OK. Ou pode ser global.
    } else if (node->symbol->decl_linenum == 0) {
        __fetch_symbol(node, sym);

    // Simbolo possui linha de declaracao mas foi  encontrado na tabela:
    // tentativa de redefinicao.
    } else {
        node->symbol->type = node->type = ERROR;
        fprintf(stderr, "Error: Symbol '%s' already defined in line %d. "
                "Check line %d.\n",
                _sym->name, _sym->decl_linenum, node->linenum);
    }

}

// Helper functions ----------------------------------------------------------

static void
_typecheck_print_stmt(struct AstNode *node, Type type, const char *ptype_str)
{
    if (node->children->type != type) {
        node->type = ERROR;
        fprintf(stderr,
                "Error: Expression Print%s statement must be of "
                "%s type. Check line %d.\n",
                ptype_str, type_get_lexeme(type), node->linenum);
    }
}
