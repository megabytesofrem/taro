#include "lexer.h"
#include "util/error.h"

#include <stdio.h>

TrLexer taro_lexer_init(char *src)
{
    TrLexer l;
    l.src = src;
    l.line = 1;
    l.start = 0;
    l.current = 0;

    // Reserved keywords
    static TrKeyword reserved_kw[] = {
        {"if",       TOK_KWIF      },
        {"then",     TOK_KWTHEN    },
        {"else",     TOK_KWELSE    },
        {"end",      TOK_KWEND     },
        {"while",    TOK_KWWHILE   },
        {"for",      TOK_KWFOR     },
        {"function", TOK_KWFUNCTION},
    };

    l.reserved_kw = reserved_kw;
    l.reserved_kw_count = (sizeof(reserved_kw) / sizeof(TrKeyword));

    return l;
}

void taro_lexer_cleanup(TrLexer *l)
{
    l->src = NULL;
    l->line = 0;
    l->start = 0;
    l->current = 0;
}

static TrToken make_token(TrLexer *l, enum TrTokenType type, char *value, int length)
{
    TrToken t;
    t.type = type;
    t.value = strdup(value);
    t.line = l->line;
    t.start = l->start;
    t.end = length;

    return t;
}

static TrToken make_error(TrLexer *l, const char *message)
{
    TrToken t;
    t.type = TOK_ERR;
    t.value = strdup(message);
    t.line = l->line;
    t.start = l->start;
    t.end = (int)(l->current - l->start);

    return t;
}

static void skip_whitespace(TrLexer *l)
{
    while (!lexer_eof(l)) {
        char c = lexer_peek(l, 0);
        switch (c) {
        case ' ':
        case '\r':
        case '\t':
            lexer_advance(l);
            break;
        case '\n':
            l->line++;
            lexer_advance(l);
            break;
        default:
            return;
        }
    }
}

void format_token(TrToken t, char *buf)
{
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

bool lexer_eof(TrLexer *l)
{
    return l->current >= strlen(l->src);
}

char lexer_advance(TrLexer *l)
{
    if (lexer_eof(l)) {
        log_error("cannot advance lexer past EOF\n");
        return '\0';
    }

    if (lexer_peek(l, 0) == '\n')
        l->line++;

    l->current++;
    return l->src[l->current - 1];
}

char lexer_peek(TrLexer *l, int offset)
{
    return l->src[l->current + offset] != '\0' ? l->src[l->current + offset] : '\0';
}

bool lexer_match(TrLexer *l, char expect)
{
    if (lexer_eof(l))
        return false;

    if (lexer_peek(l, 0) != expect)
        return false;

    lexer_advance(l);
    return true;
}

TrToken lexer_poll(TrLexer *l)
{
    skip_whitespace(l);
    l->start = l->current;

    if (lexer_eof(l))
        return make_token(l, TOK_EOF, "", 0);

    char c = lexer_advance(l);

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
        return make_token(l, lexer_peek(l, 1) == '=' ? TOK_EQEQ : TOK_EQ, "=",
                          lexer_peek(l, 1) == '=' ? 2 : 1);
    case '!':
        return make_token(l, lexer_peek(l, 1) == '=' ? TOK_BANGEQ : TOK_BANG, "!",
                          lexer_peek(l, 1) == '=' ? 2 : 1);
    case '<':
        return make_token(l, lexer_peek(l, 1) == '=' ? TOK_LTEQ : TOK_LT, "<",
                          lexer_peek(l, 1) == '=' ? 2 : 1);
    case '>':
        return make_token(l, lexer_peek(l, 1) == '=' ? TOK_GTEQ : TOK_GT, ">",
                          lexer_peek(l, 1) == '=' ? 2 : 1);
    case '#':
        while (!lexer_eof(l) && lexer_peek(l, 0) != '\n')
            lexer_advance(l);

        return make_token(l, TOK_COMMENT, l->src + l->start + 1,
                          l->current - l->start - 1);
    }

    return make_error(l, "unexpected character");
}

TrToken lexer_scan_kw(TrLexer *l)
{
    // Find the start of the keyword or identifier
    l->start = l->current - 1;

    char c;
    while ((c = lexer_peek(l, 0))) {
        if (!isalpha(c) && !isdigit(c) && c != '_') {
            break;
        }

        lexer_advance(l);
    }

    // Calculate the length of the keyword or identifier
    int length = l->current - l->start;
    char *text = strndup(l->src + l->start, length);

    // Check if the keyword is a reserved keyword
    enum TrTokenType type = TOK_IDENTIFIER;
    for (int i = 0; i < l->reserved_kw_count; i++) {
        if (strcmp(text, l->reserved_kw[i].name) == 0) {
            type = l->reserved_kw[i].type;
            break;
        }
    }

    TrToken token = make_token(l, type, text, length);
    free(text);

    return token;
}

TrToken lexer_scan_number(TrLexer *l)
{
    // Find the start of the number
    l->start = l->current;

    char c;
    bool floating = false;

    while ((c = lexer_peek(l, 0))) {
        if (!isdigit(c)) {
            if (c == '.' && !floating) {
                floating = true;
            } else {
                break;
            }
        }

        lexer_advance(l);
    }

    // Calculate the length of the number
    int length = l->current - l->start;
    char *text = strndup(l->src + l->start, length);

    TrToken token = make_token(l, TOK_INT, text, length);

    if (floating) {
        token.type = TOK_FLOAT;
        token.float_value = strtof(text, NULL);
    } else {
        token.int_value = strtol(text, NULL, 10);
    }

    free(text);
    return token;
}
