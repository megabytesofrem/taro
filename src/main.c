#include "lexer.h"

#include <stdio.h>
#include <strings.h> // For bzero

int main()
{
    TrLexer l = taro_lexer_init("if x >= 1234 then\n0.1234\n else def end");

    char buf[32] = {0};

    while (1) {
        TrToken t = lexer_poll(&l);
        if (t.type == TOK_EOF) {
            break;
        }

        bzero(buf, 32);
        format_token(t, buf);

        printf("token: %s @ %d:%d:%d\n", buf, t.line, t.start, t.end);
    }

    printf("Done lexing\n");
    taro_lexer_cleanup(&l);

    return 0;
}