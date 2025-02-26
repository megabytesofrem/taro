/**
 * Frontend lexer interface for Taro.
 *
 * Authors:
 * - Charlotte (megabytesofrem)
 */

#include "lexer.h"
#include "util/logger.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static bool at_eof(Lexer *l);
static char advance(Lexer *l);
static char peek(Lexer *l, int offset);

static struct Token make_token(Lexer *l, enum TokenType type, char *value, int length);
static struct Token make_error(Lexer *l, const char *message);
static struct Token lexer_scan_kw(Lexer *l);
static struct Token lexer_scan_number(Lexer *l);

/* Reserved keywords */
static struct Keyword g_reserved_keywords[] = {
    {"if",       TOK_KWIF      },
    {"then",     TOK_KWTHEN    },
    {"else",     TOK_KWELSE    },
    {"end",      TOK_KWEND     },
    {"while",    TOK_KWWHILE   },
    {"for",      TOK_KWFOR     },
    {"function", TOK_KWFUNCTION},
};

Lexer lexer_init(char *src) {
    Lexer l;
    l.src     = src;
    l.line    = 1;
    l.start   = 0;
    l.current = 0;

    // Reserved keywords
    l.reserved_kw       = g_reserved_keywords;
    l.reserved_kw_count = (sizeof(g_reserved_keywords) / sizeof(struct Keyword));

    return l;
}

void lexer_cleanup(Lexer *l) {
    l->src     = NULL;
    l->line    = 0;
    l->start   = 0;
    l->current = 0;
}

static struct Token make_token(Lexer *l, enum TokenType type, char *value, int length) {
    struct Token t;
    t.type  = type;
    t.value = strdup(value);
    t.line  = l->line;
    t.start = l->start;
    t.end   = length;

    return t;
}

static struct Token make_error(Lexer *l, const char *message) {
    struct Token t;
    t.type  = TOK_ERR;
    t.value = strdup(message);
    t.line  = l->line;
    t.start = l->start;
    t.end   = (int)(l->current - l->start);

    return t;
}

static void skip_whitespace(Lexer *l) {
    while (!at_eof(l)) {
        char c = peek(l, 0);
        switch (c) {
        case ' ':
        case '\r':
        case '\t':
            advance(l);
            break;
        case '\n':
            l->line++;
            advance(l);
            break;
        default:
            return;
        }
    }
}

void format_token(struct Token t, char *buf) {
    if (buf == NULL) {
        log_error("buffer provided to format_token was NULL\n");
        return;
    }

    switch (t.type) {
    case TOK_IDENTIFIER:
        sprintf(buf, "TOK_IDENTIFIER(%s)", t.value);
        break;
    case TOK_INT:
        sprintf(buf, "TOK_INT(%d)", t.int_value);
        break;
    case TOK_FLOAT:
        sprintf(buf, "TOK_FLOAT(%f)", t.float_value);
        break;
    case TOK_STRING:
        sprintf(buf, "TOK_STRING(%s)", t.value);
        break;
    case TOK_COMMENT:
        sprintf(buf, "TOK_COMMENT(%s)", t.value);
        break;
    case TOK_ERR:
        sprintf(buf, "TOK_ERR(%s)", t.value);
        break;
    case TOK_EOF:
        sprintf(buf, "TOK_EOF");
        break;
    case TOK_PLUS:
        sprintf(buf, "TOK_PLUS");
        break;
    case TOK_MINUS:
        sprintf(buf, "TOK_MINUS");
        break;
    case TOK_STAR:
        sprintf(buf, "TOK_STAR");
        break;
    case TOK_SLASH:
        sprintf(buf, "TOK_SLASH");
        break;
    case TOK_EQ:
        sprintf(buf, "TOK_EQ");
        break;
    case TOK_EQEQ:
        sprintf(buf, "TOK_EQEQ");
        break;
    case TOK_BANG:
        sprintf(buf, "TOK_BANG");
        break;
    case TOK_BANGEQ:
        sprintf(buf, "TOK_BANGEQ");
        break;
    case TOK_LT:
        sprintf(buf, "TOK_LT");
        break;
    case TOK_LTEQ:
        sprintf(buf, "TOK_LTEQ");
        break;
    case TOK_GT:
        sprintf(buf, "TOK_GT");
        break;
    case TOK_GTEQ:
        sprintf(buf, "TOK_GTEQ");
        break;
    case TOK_ARROW:
        sprintf(buf, "TOK_ARROW");
        break;
    case TOK_KWIF:
        sprintf(buf, "TOK_KWIF");
        break;
    case TOK_KWTHEN:
        sprintf(buf, "TOK_KWTHEN");
        break;
    case TOK_KWELSE:
        sprintf(buf, "TOK_KWELSE");
        break;
    case TOK_KWEND:
        sprintf(buf, "TOK_KWEND");
        break;
    case TOK_KWWHILE:
        sprintf(buf, "TOK_KWWHILE");
        break;
    case TOK_KWFOR:
        sprintf(buf, "TOK_KWFOR");
        break;
    case TOK_KWFUNCTION:
        sprintf(buf, "TOK_KWFUNCTION");
        break;
    default:
        sprintf(buf, "TOK_UNKNOWN");
        break;
    }
}

bool at_eof(Lexer *l) { return l->current >= strlen(l->src); }

char advance(Lexer *l) {
    if (at_eof(l)) {
        log_error("cannot advance lexer past EOF\n");
        return '\0';
    }

    if (peek(l, 0) == '\n')
        l->line++;

    l->current++;
    return l->src[l->current - 1];
}

char peek(Lexer *l, int offset) {
    return l->src[l->current + offset] != '\0' ? l->src[l->current + offset] : '\0';
}

struct Token lexer_poll(Lexer *l) {
    skip_whitespace(l);
    l->start = l->current;

    if (at_eof(l))
        return make_token(l, TOK_EOF, "", 0);

    char c = advance(l);

    if (isdigit(c))
        return lexer_scan_number(l);

    if (isalpha(c))
        return lexer_scan_kw(l);

    switch (c) {
    case '+':
        return make_token(l, TOK_PLUS, "+", 1);
    case '-':
        return make_token(l, TOK_MINUS, "-", 1);
    case '*':
        return make_token(l, TOK_STAR, "*", 1);
    case '/':
        return make_token(l, TOK_SLASH, "/", 1);
    case '=':
        if (peek(l, 0) == '=') {
            advance(l);
            return make_token(l, TOK_EQEQ, "==", 2);
        } else {
            return make_token(l, TOK_EQ, "=", 1);
        }
    case '!':
        if (peek(l, 0) == '=') {
            advance(l);
            return make_token(l, TOK_BANGEQ, "!=", 2);
        } else {
            return make_token(l, TOK_BANG, "!", 1);
        }
    case '<':
        if (peek(l, 0) == '=') {
            advance(l);
            return make_token(l, TOK_LTEQ, "<=", 2);
        } else {
            return make_token(l, TOK_LT, "<", 1);
        }
    case '>':
        if (peek(l, 0) == '=') {
            advance(l);
            return make_token(l, TOK_GTEQ, ">=", 2);
        } else {
            return make_token(l, TOK_GT, ">", 1);
        }
    case '#':
        while (!at_eof(l) && peek(l, 0) != '\n')
            advance(l);

        return make_token(l, TOK_COMMENT, l->src + l->start + 1,
                          l->current - l->start - 1);
    }

    return make_error(l, "unexpected character");
}

struct Token lexer_scan_kw(Lexer *l) {
    // Find the start of the keyword or identifier
    l->start = l->current - 1;

    char c;
    while ((c = peek(l, 0))) {
        if (!isalpha(c) && !isdigit(c) && c != '_') {
            break;
        }

        advance(l);
    }

    // Calculate the length of the keyword or identifier
    int length = l->current - l->start;
    char *text = strndup(l->src + l->start, length);

    // Check if the keyword is a reserved keyword
    enum TokenType type = TOK_IDENTIFIER;
    for (int i = 0; i < l->reserved_kw_count; i++) {
        if (strcmp(text, l->reserved_kw[i].name) == 0) {
            type = l->reserved_kw[i].type;
            break;
        }
    }

    struct Token token = make_token(l, type, text, length);
    free(text);

    return token;
}

struct Token lexer_scan_number(Lexer *l) {
    // Find the start of the number
    l->start = l->current;

    char c;
    bool floating = false;

    while ((c = peek(l, 0))) {
        if (!isdigit(c)) {
            if (c == '.' && !floating) {
                floating = true;
            } else {
                break;
            }
        }

        advance(l);
    }

    // Calculate the length of the number
    int length = l->current - l->start + 1;
    char *text = strndup(l->src + (l->start - 1), length);

    struct Token token = make_token(l, TOK_INT, text, length);

    if (floating) {
        token.type        = TOK_FLOAT;
        token.float_value = strtof(text, NULL);
    } else {
        token.int_value = strtol(text, NULL, 10);
    }

    free(text);
    return token;
}
