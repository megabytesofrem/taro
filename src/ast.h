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

typedef struct ASTNode
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
    struct ASTNode **s_children;
    int s_children_count;
    int s_children_capacity;

    struct ASTNode *left, *right;
} ASTNode;

typedef struct
{
    ASTNode *root;

    char **comments;
    int comment_count;
} TrAST;

ASTNode *ast_node_create(enum TrNodeKind kind, ASTNode *left, ASTNode *right)
{
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    node->kind = kind;
    node->left = left;
    node->right = right;

    return node;
}

void ast_node_create_child(ASTNode *parent, ASTNode *child)
{
    if (parent == NULL || child == NULL) {
        log_error("cannot create a child when parent or child is NULL\n");
        return;
    }

    parent->s_children = (ASTNode **)realloc(
        parent->s_children, sizeof(ASTNode *) * (parent->s_children_count + 1));
    parent->s_children[parent->s_children_count] = child;
    parent->s_children_count++;
}

ASTNode *ast_node_create_int(int value)
{
    ASTNode *node = ast_node_create(NK_INT, NULL, NULL);
    node->data.int_value = value;

    return node;
}

ASTNode *ast_node_create_float(float value)
{
    ASTNode *node = ast_node_create(NK_FLOAT, NULL, NULL);
    node->data.float_value = value;

    return node;
}

ASTNode *ast_node_create_string(char *value)
{
    ASTNode *node = ast_node_create(NK_STRING, NULL, NULL);
    node->data.string_value = strdup(value);

    return node;
}

ASTNode *ast_node_create_ident(char *value)
{
    ASTNode *node = ast_node_create(NK_IDENTIFIER, NULL, NULL);
    node->data.string_value = strdup(value);

    return node;
}

ASTNode *ast_node_create_operator(enum TrNodeKind kind, ASTNode *left, ASTNode *right)
{
    if (kind != NK_BINARY_OP || kind != NK_UNARY_OP) {
        log_error("invalid node kind (expected NK_BINARY_OP or NK_UNARY_OP)\n");
        return NULL;
    }

    return ast_node_create(kind, left, right);
}

ASTNode *ast_node_create_array(ASTNode **nodes, int node_count, int capacity)
{
    ASTNode *node = ast_node_create(NK_ARRAY, NULL, NULL);
    node->s_children = nodes;
    node->s_children_count = node_count;
    node->s_children_capacity = capacity;

    return node;
}

void ast_node_append_child(ASTNode *parent, ASTNode *child)
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
        parent->s_children = (ASTNode **)realloc(
            parent->s_children, sizeof(ASTNode *) * parent->s_children_capacity);
    }

    parent->s_children[parent->s_children_count++] = child;
}

void ast_node_destroy(ASTNode *node)
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