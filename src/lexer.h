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

typedef struct
{
    enum TokenType type;
    char *value;
    int line, start, end;

    union {
        int int_value;
        float float_value;
    };

} Token;

typedef struct
{
    const char *name;
    enum TokenType type;
} Keyword;

typedef struct
{
    char *src;
    int line, start, current;

    Keyword *reserved_kw;
    int reserved_kw_count;
} Lexer;

Lexer lexer_init(char *src);
void lexer_cleanup(Lexer *l);

void format_token(Token t, char *buf);

bool lexer_eof(Lexer *l);
char lexer_advance(Lexer *l);
char lexer_peek(Lexer *l, int offset);
bool lexer_match(Lexer *l, char expect);
Token lexer_poll(Lexer *l);

Token lexer_scan_kw(Lexer *l);
Token lexer_scan_number(Lexer *l);
#endif
