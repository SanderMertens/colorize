 
#include "buffer.h"

char* colorize(
    char *msg) 
{
    corto_buffer buff = CORTO_BUFFER_INIT;
    char *ptr, ch, prev = '\0';
    bool isNum = FALSE;
    char isStr = '\0';
    bool isVar = false;

    for (ptr = msg; (ch = *ptr); ptr++) {

        if (isNum && !isdigit(ch) && !isalpha(ch) && (ch != '.') && (ch != '%')) {
            corto_buffer_appendstr(&buff, CORTO_NORMAL);
            isNum = FALSE;
        }
        if (isStr && (isStr == ch) && !isalpha(ptr[1]) && (prev != '\\')) {
            isStr = '\0';
        } else if (((ch == '\'') || (ch == '"')) && !isStr && !isalpha(prev) && (prev != '\\')) {
            corto_buffer_appendstr(&buff, CORTO_CYAN);
            isStr = ch;
        }

        if ((isdigit(ch) || (ch == '%' && isdigit(prev)) || (ch == '-' && isdigit(ptr[1]))) && !isNum && !isStr && !isVar && !isalpha(prev) && !isdigit(prev) && (prev != '_') && (prev != '.')) {
            corto_buffer_appendstr(&buff, CORTO_GREEN);
            isNum = TRUE;
        }

        if (isVar && !isalpha(ch) && !isdigit(ch) && ch != '_') {
            corto_buffer_appendstr(&buff, CORTO_NORMAL);
            isVar = FALSE;
        }

        if (!isStr && !isVar && ch == '$' && isalpha(ptr[1])) {
            corto_buffer_appendstr(&buff, CORTO_MAGENTA);
            isVar = TRUE;
        }

        corto_buffer_appendstrn(&buff, ptr, 1);

        if (((ch == '\'') || (ch == '"')) && !isStr) {
            corto_buffer_appendstr(&buff, CORTO_NORMAL);
        }

        prev = ch;
    }

    if (isNum || isStr || isVar) {
        corto_buffer_appendstr(&buff, CORTO_NORMAL);
    }

    return corto_buffer_str(&buff);
}




