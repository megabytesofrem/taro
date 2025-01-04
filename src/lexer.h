#ifndef TARO_LEXER
#define TARO_LEXER

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

    Token *tokens;
    int token_count;
    int token_capacity;

    Keyword *reserved_kw;
    int reserved_kw_count;
} TrLexer;

TrLexer lexer_init(char *src);
void lexer_free(TrLexer *l);

void format_token(Token t, char *buf);

bool lexer_eof(TrLexer *l);
void lexer_advance(TrLexer *l);
char lexer_peek(TrLexer *l, int offset);
void lexer_scan(TrLexer *l);
void lexer_scan_kw(TrLexer *l);
void lexer_scan_numeric(TrLexer *l);
void lexer_push(TrLexer *l, Token token);

Token token_init(enum TokenType type, char *value, int length, int line);

#endif