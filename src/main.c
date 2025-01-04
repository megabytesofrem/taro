#include "lexer.h"

#include <stdio.h>

int main()
{
    TrLexer l = lexer_init("if x >= 1234 then\n0.1234\n else def end");
    lexer_scan(&l);

    char buf[32] = {0};

    for (int i = 0; i < l.token_count; i++)
    {
        Token t = l.tokens[i];
        bzero(buf, 32);
        format_token(t, buf);

        printf("token %d: %s @ %d:%d:%d\n", i, buf, t.line, t.start, t.end);
    }

    printf("Done lexing\n");
    lexer_free(&l);

    return 0;
}