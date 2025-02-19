#ifndef TARO_PARSER
#define TARO_PARSER

#include "ast.h"
#include "lexer.h"

enum AssocDirection
{
    ASSOC_LEFT,
    ASSOC_RIGHT,
};

typedef struct
{
    enum TokenType tt;
    int precedence;
    enum AssocDirection assoc;
} Precedence;

typedef struct
{
    Lexer *lexer;
    Token curr;
} Parser;

Parser parser_init(Lexer *l);
void parser_cleanup(Parser *p);

#endif