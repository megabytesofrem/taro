#include "lexer.h"
#include "error.h"

#include <stdio.h>

TrLexer lexer_init(char *src)
{
    TrLexer l;
    l.src = src;
    l.line = 1;
    l.start = 0;
    l.current = 0;

    l.tokens = NULL;
    l.token_count = 0;
    l.token_capacity = 0;

    // Reserved keywords
    static Keyword reserved_kw[] = {
        {"if",       TOK_KWIF      },
        {"then",     TOK_KWTHEN    },
        {"else",     TOK_KWELSE    },
        {"end",      TOK_KWEND     },
        {"while",    TOK_KWWHILE   },
        {"for",      TOK_KWFOR     },
        {"function", TOK_KWFUNCTION},
    };

    l.reserved_kw = reserved_kw;
    l.reserved_kw_count = (sizeof(reserved_kw) / sizeof(Keyword));

    return l;
}

void lexer_free(TrLexer *l)
{
    for (int i = 0; i < l->token_count; i++)
    {
        Token t = l->tokens[i];
        free(t.value);
    }

    free(l->tokens);
}

void format_token(Token t, char *buf)
{
    if (buf == NULL)
    {
        log_error("buffer provided to format_token was NULL\n");
        return;
    }

    switch (t.type)
    {
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

void lexer_advance(TrLexer *l)
{
    if (lexer_eof(l))
    {
        log_error("cannot advance lexer past EOF\n");
        return;
    }

    if (lexer_peek(l, 0) == '\n')
        l->line++;

    l->current++;
}

char lexer_peek(TrLexer *l, int offset)
{
    return l->src[l->current + offset] != '\0' ? l->src[l->current + offset] : '\0';
}

void lexer_scan(TrLexer *l)
{
    while (!lexer_eof(l))
    {
        char c = lexer_peek(l, 0);
        switch (c)
        {
        case '+':
            lexer_push(l, token_init(TOK_PLUS, "", 1, l->line));
            lexer_advance(l);
            break;
        case '-':
            if (lexer_peek(l, 1) == '>')
            {
                lexer_push(l, token_init(TOK_ARROW, "", 2, l->line));
                lexer_advance(l);
                lexer_advance(l);
            }
            else
            {
                lexer_push(l, token_init(TOK_MINUS, "", 1, l->line));
                lexer_advance(l);
            }
            break;
        case '*':
            lexer_push(l, token_init(TOK_STAR, "", 1, l->line));
            lexer_advance(l);
            break;
        case '/':
            lexer_push(l, token_init(TOK_SLASH, "", 1, l->line));
            lexer_advance(l);
            break;
        case '=':
            if (lexer_peek(l, 1) == '=')
            {
                lexer_push(l, token_init(TOK_EQEQ, "", 2, l->line));
                lexer_advance(l);
                lexer_advance(l);
            }
            else
            {
                lexer_push(l, token_init(TOK_EQ, "", 1, l->line));
                lexer_advance(l);
            }
            break;
        case '!':
            if (lexer_peek(l, 1) == '=')
            {
                lexer_push(l, token_init(TOK_BANGEQ, "", 2, l->line));
                lexer_advance(l);
                lexer_advance(l);
            }
            else
            {
                lexer_push(l, token_init(TOK_BANG, "", 1, l->line));
                lexer_advance(l);
            }
            break;
        case '<':
            if (lexer_peek(l, 1) == '=')
            {
                lexer_push(l, token_init(TOK_LTEQ, "", 2, l->line));
                lexer_advance(l);
                lexer_advance(l);
            }
            else
            {
                lexer_push(l, token_init(TOK_LT, "", 1, l->line));
                lexer_advance(l);
            }
            break;
        case '>':
            if (lexer_peek(l, 1) == '=')
            {
                lexer_push(l, token_init(TOK_GTEQ, "", 2, l->line));
                lexer_advance(l);
                lexer_advance(l);
            }
            else
            {
                lexer_push(l, token_init(TOK_GT, "", 1, l->line));
                lexer_advance(l);
            }
            break;

        /* Skip comments, but emit a token for later on */
        case '#':
            lexer_advance(l);
            while (!lexer_eof(l) && lexer_peek(l, 0) != '\n')
                lexer_advance(l);

            lexer_push(l, token_init(TOK_COMMENT, l->src + 1 + l->start,
                                     l->current - l->start, l->line));
            break;
        case ' ':
        case '\t':
        case '\r':
            lexer_advance(l);
            break;
        case '\n':
            l->line++;
            lexer_advance(l);
            break;
        default:
            if (isalpha(c))
            {
                lexer_scan_kw(l);
            }
            else if (isdigit(c))
            {
                lexer_scan_numeric(l);
            }
            else
            {
                lexer_advance(l);
            }
            break;
        }
    }
}

void lexer_scan_kw(TrLexer *l)
{
    // Find the start of the keyword or identifier
    l->start = l->current;

    char c;
    while ((c = lexer_peek(l, 0)))
    {
        if (!isalpha(c) && !isdigit(c) && c != '_')
        {
            break;
        }

        lexer_advance(l);
    }

    // Calculate the length of the keyword or identifier
    int length = l->current - l->start;
    char *text = strndup(l->src + l->start, length);

    // Check if the keyword is a reserved keyword
    enum TokenType type = TOK_IDENTIFIER;
    for (int i = 0; i < l->reserved_kw_count; i++)
    {
        if (strcmp(text, l->reserved_kw[i].name) == 0)
        {
            type = l->reserved_kw[i].type;
            break;
        }
    }

    lexer_push(l, token_init(type, text, length, l->line));
    free(text);
}

void lexer_scan_numeric(TrLexer *l)
{
    // Find the start of the number
    l->start = l->current;

    char c;
    bool floating = false;

    while ((c = lexer_peek(l, 0)))
    {
        if (!isdigit(c))
        {
            if (c == '.' && !floating)
            {
                floating = true;
            }
            else
            {
                break;
            }
        }

        lexer_advance(l);
    }

    // Calculate the length of the number
    int length = l->current - l->start;
    char *text = strndup(l->src + l->start, length);

    Token token = token_init(TOK_INT, text, length, l->line);

    if (floating)
    {
        token.type = TOK_FLOAT;
        token.float_value = strtof(text, NULL);
    }
    else
    {
        token.int_value = strtol(text, NULL, 10);
    }

    lexer_push(l, token);
    free(text);
}

void lexer_push(TrLexer *l, Token token)
{
    bool needs_resize = l->token_count >= l->token_capacity;
    if (needs_resize)
    {
        l->token_capacity = l->token_capacity == 0 ? 1 : l->token_capacity * 2;
        l->tokens = realloc(l->tokens, sizeof(Token) * l->token_capacity);
    }

    l->tokens[l->token_count++] = token;
}

Token token_init(enum TokenType type, char *value, int length, int line)
{
    Token t;
    t.type = type;
    t.value = strdup(value);
    t.line = line;
    t.start = 0;
    t.end = length;

    return t;
}