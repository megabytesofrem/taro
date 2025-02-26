/**
 * Frontend parser interface for Taro.
 *
 * Authors:
 * - Charlotte (megabytesofrem)
 */

#ifndef TARO_PARSER_H
#define TARO_PARSER_H

#include "lexer.h"

enum AssocDirection
{
    ASSOC_LEFT,
    ASSOC_RIGHT,
};

struct Prec
{
    enum TokenType tt;
    int precedence;
    enum AssocDirection assoc;
};

typedef struct
{
    Lexer *lexer;
    Token curr;
} Parser;

Parser parser_init(Lexer *l);
void parser_cleanup(Parser *p);

#endif
