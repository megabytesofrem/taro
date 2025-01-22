#ifndef TARO_LEXER
#define TARO_LEXER

#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

enum TrTokenType
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
    enum TrTokenType type;
    char *value;
    int line, start, end;

    union {
        int int_value;
        float float_value;
    };

} TrToken;

typedef struct
{
    const char *name;
    enum TrTokenType type;
} TrKeyword;

typedef struct
{
    char *src;
    int line, start, current;

    TrKeyword *reserved_kw;
    int reserved_kw_count;
} TrLexer;

TrLexer taro_lexer_init(char *src);
void taro_lexer_cleanup(TrLexer *l);

void format_token(TrToken t, char *buf);

bool lexer_eof(TrLexer *l);
char lexer_advance(TrLexer *l);
char lexer_peek(TrLexer *l, int offset);
bool lexer_match(TrLexer *l, char expect);
TrToken lexer_poll(TrLexer *l);

TrToken lexer_scan_kw(TrLexer *l);
TrToken lexer_scan_number(TrLexer *l);
#endif