#ifndef TARO_LEXER_H
#define TARO_LEXER_H

#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

enum TokenType
{
    TOK_IDENTIFIER,
    TOK_INT,
    TOK_FLOAT,
    TOK_STRING,
    TOK_COMMENT,
    TOK_ERR, // Error token
    TOK_EOF, // EOF token

    TOK_PLUS,
    TOK_MINUS,
    TOK_STAR,
    TOK_SLASH,
    TOK_EQ,
    TOK_EQEQ,
    TOK_BANG,
    TOK_BANGEQ,
    TOK_LT,
    TOK_LTEQ,
    TOK_GT,
    TOK_GTEQ,
    TOK_ARROW,

    /* Reserved keywords */
    TOK_KWIF,
    TOK_KWTHEN,
    TOK_KWELSE,
    TOK_KWEND,
    TOK_KWWHILE,
    TOK_KWFOR,
    TOK_KWFUNCTION,
};

struct Token
{
    enum TokenType type;
    char *value;
    int line, start, end;

    union {
        int int_value;
        float float_value;
    };
};

struct Keyword
{
    const char *name;
    enum TokenType type;
};

typedef struct
{
    char *src;
    int line, start, current;

    struct Keyword *reserved_kw;
    int reserved_kw_count;
} Lexer;

Lexer lexer_init(char *src);
void lexer_cleanup(Lexer *l);

struct Token lexer_poll(Lexer *l);
#endif
