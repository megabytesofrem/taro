#ifndef TARO_NODE
#define TARO_NODE

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "util/error.h"

enum TrNodeKind
{
    NK_IDENTIFIER,
    NK_INT,
    NK_FLOAT,
    NK_STRING,
    NK_ARRAY,

    NK_BINARY_OP,
    NK_UNARY_OP,
    NK_ASSIGN,
    NK_FUNCTION_CALL,
    NK_STRUCT_CTOR,

    // Statements
    NK_STMT_EXPR,
    NK_STMT_IF,
    NK_STMT_FOR,
    NK_STMT_WHILE,
    NK_STMT_VARIABLE,
    NK_STMT_RETURN,
    NK_STMT_FUNCTION_DECL,
};

typedef struct TrASTNode
{
    enum TrNodeKind kind;

    union NodeData {
        char *string_value;
        int int_value;
        float float_value;
    } data;

    // Do note arrays are represented internally via NK_ARRAY_TYPE and s_children.
    // The AST consists of a tree of nodes, with each node having a left and right child
    // and optionally a list of children (s_children)
    struct TrASTNode **s_children;
    int s_children_count;
    int s_children_capacity;

    struct TrASTNode *left, *right;
} TrASTNode;

typedef struct
{
    TrASTNode *root;

    char **comments;
    int comment_count;
} TrAST;

TrASTNode *ast_node_create(enum TrNodeKind kind, TrASTNode *left, TrASTNode *right)
{
    TrASTNode *node = (TrASTNode *)(sizeof(TrASTNode));
    node->kind = kind;
    node->left = left;
    node->right = right;

    return node;
}

void ast_node_create_child(TrASTNode *parent, TrASTNode *child)
{
    if (parent == NULL || child == NULL) {
        log_error("cannot create a child when parent or child is NULL\n");
        return;
    }

    parent->s_children = (TrASTNode **)realloc(
        parent->s_children, sizeof(TrASTNode *) * (parent->s_children_count + 1));
    parent->s_children[parent->s_children_count] = child;
    parent->s_children_count++;
}

TrASTNode *ast_node_create_int(int value)
{
    TrASTNode *node = ast_node_create(NK_INT, NULL, NULL);
    node->data.int_value = value;

    return node;
}

TrASTNode *ast_node_create_float(float value)
{
    TrASTNode *node = ast_node_create(NK_FLOAT, NULL, NULL);
    node->data.float_value = value;

    return node;
}

TrASTNode *ast_node_create_string(char *value)
{
    TrASTNode *node = ast_node_create(NK_STRING, NULL, NULL);
    node->data.string_value = strdup(value);

    return node;
}

TrASTNode *ast_node_create_ident(char *value)
{
    TrASTNode *node = ast_node_create(NK_IDENTIFIER, NULL, NULL);
    node->data.string_value = strdup(value);

    return node;
}

TrASTNode *ast_node_create_operator(enum TrNodeKind kind, TrASTNode *left,
                                    TrASTNode *right)
{
    if (kind != NK_BINARY_OP || kind != NK_UNARY_OP) {
        log_error("invalid node kind (expected NK_BINARY_OP or NK_UNARY_OP)\n");
        return NULL;
    }

    return ast_node_create(kind, left, right);
}

TrASTNode *ast_node_create_array(TrASTNode **nodes, int node_count, int capacity)
{
    TrASTNode *node = ast_node_create(NK_ARRAY, NULL, NULL);
    node->s_children = nodes;
    node->s_children_count = node_count;
    node->s_children_capacity = capacity;

    return node;
}

void ast_node_append_child(TrASTNode *parent, TrASTNode *child)
{
    if (parent == NULL || child == NULL)
        return;

    if (parent->kind != NK_ARRAY) {
        log_error("cannot append child to non-array node\n");
        return;
    }

    // Need to resize to hold more children
    bool needs_resize = parent->s_children_count >= parent->s_children_capacity;
    if (needs_resize) {
        parent->s_children_capacity =
            parent->s_children_capacity == 0 ? 1 : parent->s_children_capacity * 2;
        parent->s_children = (TrASTNode **)realloc(
            parent->s_children, sizeof(TrASTNode *) * parent->s_children_capacity);
    }

    parent->s_children[parent->s_children_count++] = child;
}

void ast_node_destroy(TrASTNode *node)
{
    if (!node)
        return;

    if (node->kind == NK_STRING) {
        free(node->data.string_value);
    }

    // Free all children
    for (int i = 0; i < node->s_children_count; i++) {
        ast_node_destroy(node->s_children[i]);
    }
    free(node->s_children);
    free(node);
}

#endif