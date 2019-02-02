#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <inttypes.h>
#include <limits.h>

#ifndef __cplusplus
/* Boolean definitions (compatible with C++ and C99 stdbool) */
#undef bool
#undef true
#undef false
#define bool char
#define false 0
#define true !false
#endif

/* NULL pointer value */
#ifndef NULL
#define NULL (0x0)
#endif

#define UT_STRBUF_INIT (ut_strbuf){0}
#define UT_STRBUF_ELEMENT_SIZE (511)
#define UT_LOG_USE_COLORS (1)

#define UT_BLACK   "\033[1;30m"
#define UT_RED     "\033[0;31m"
#define UT_GREEN   "\033[0;32m"
#define UT_YELLOW  "\033[0;33m"
#define UT_BLUE    "\033[0;34m"
#define UT_MAGENTA "\033[0;35m"
#define UT_CYAN    "\033[0;36m"
#define UT_WHITE   "\033[1;37m"
#define UT_GREY    "\033[0;37m"
#define UT_NORMAL  "\033[0;49m"
#define UT_BOLD    "\033[1;49m"

typedef struct ut_strbuf_element {
    bool buffer_embedded;
    uint32_t pos;
    char *buf;
    struct ut_strbuf_element *next;
} ut_strbuf_element;

typedef struct ut_strbuf_element_embedded {
    ut_strbuf_element super;
    char buf[UT_STRBUF_ELEMENT_SIZE + 1];
} ut_strbuf_element_embedded;

typedef struct ut_strbuf_element_str {
    ut_strbuf_element super;
    char *alloc_str;
} ut_strbuf_element_str;

typedef struct ut_strbuf {
    /* When set by an application, append will write to this buffer */
    char *buf;

    /* The maximum number of characters that may be printed */
    uint32_t max;

    /* Size of elements minus current element */
    uint32_t size;

    /* The number of elements in use */
    uint32_t elementCount;

    /* Always allocate at least one element */
    ut_strbuf_element_embedded firstElement;

    /* The current element being appended to */
    ut_strbuf_element *current;
} ut_strbuf;

/* Add an extra element to the buffer */
static
void ut_strbuf_grow(
    ut_strbuf *b)
{
    /* Allocate new element */
    ut_strbuf_element_embedded *e = malloc(sizeof(ut_strbuf_element_embedded));
    b->size += b->current->pos;
    b->current->next = (ut_strbuf_element*)e;
    b->current = (ut_strbuf_element*)e;
    b->elementCount ++;
    e->super.buffer_embedded = true;
    e->super.buf = e->buf;
    e->super.pos = 0;
    e->super.next = NULL;
}

/* Add an extra dynamic element */
static
void ut_strbuf_grow_str(
    ut_strbuf *b,
    char *str,
    char *alloc_str,
    uint32_t size)
{
    /* Allocate new element */
    ut_strbuf_element_str *e = malloc(sizeof(ut_strbuf_element_str));
    b->size += b->current->pos;
    b->current->next = (ut_strbuf_element*)e;
    b->current = (ut_strbuf_element*)e;
    b->elementCount ++;
    e->super.buffer_embedded = false;
    e->super.pos = size ? size : strlen(str);
    e->super.next = NULL;
    e->super.buf = str;
    e->alloc_str = alloc_str;
}

static
char* ut_strbuf_ptr(
    ut_strbuf *b)
{
    if (b->buf) {
        return &b->buf[b->current->pos];
    } else {
        return &b->current->buf[b->current->pos];
    }
}

/* Compute the amount of space left in the current element */
static
int32_t ut_strbuf_memLeftInCurrentElement(
    ut_strbuf *b)
{
    if (b->current->buffer_embedded) {
        return UT_STRBUF_ELEMENT_SIZE - b->current->pos;
    } else {
        return 0;
    }
}

/* Compute the amount of space left */
static
int32_t ut_strbuf_memLeft(
    ut_strbuf *b)
{
    if (b->max) {
        return b->max - b->size - b->current->pos;
    } else {
        return INT_MAX;
    }
}

static
void ut_strbuf_init(
    ut_strbuf *b)
{
    /* Initialize buffer structure only once */
    if (!b->elementCount) {
        b->size = 0;
        b->firstElement.super.next = NULL;
        b->firstElement.super.pos = 0;
        b->firstElement.super.buffer_embedded = true;
        b->firstElement.super.buf = b->firstElement.buf;
        b->elementCount ++;
        b->current = (ut_strbuf_element*)&b->firstElement;
    }
}

/* Quick custom function to copy a maxium number of characters and
 * simultaneously determine length of source string. */
static
unsigned int fast_strncpy(
    char * dst,
    const char * src,
    unsigned int n_cpy,
    unsigned int n)
{
    const char *ptr, *orig = src;
    char ch;

    for (ptr = src; (ptr - orig < n) && (ch = *ptr); ptr ++) {
        if (ptr - orig < n_cpy) {
            *dst = ch;
            dst ++;
        }
    }

    return ptr - orig;
}

/* Append a format string to a buffer */
static
bool ut_strbuf_append_intern(
    ut_strbuf *b,
    const char* str,
    int n,
    bool fmt_string,
    va_list args)
{
    bool result = true;
    va_list arg_cpy;

    if (!str) {
        return result;
    }

    ut_strbuf_init(b);

    int32_t memLeftInElement = ut_strbuf_memLeftInCurrentElement(b);
    int32_t memLeft = ut_strbuf_memLeft(b);

    if (!memLeft) {
        return false;
    }

    /* Compute the memory required to add the string to the buffer. If user
     * provided buffer, use space left in buffer, otherwise use space left in
     * current element. */
    int32_t max_copy = b->buf ? memLeft : memLeftInElement;
    int32_t memRequired;

    if (n < 0) n = INT_MAX;

    if (!fmt_string) {
        memRequired = fast_strncpy(ut_strbuf_ptr(b), str, max_copy, n);
    } else {
        va_copy(arg_cpy, args);
        memRequired = vsnprintf(ut_strbuf_ptr(b), max_copy + 1, str, args);
    }

    if (memRequired <= memLeftInElement) {
        /* Element was large enough to fit string */
        b->current->pos += memRequired;
    } else if ((memRequired - memLeftInElement) < memLeft) {
        /* Element was not large enough, but buffer still has space */
        if (!fmt_string) {
            b->current->pos += memLeftInElement;
            memRequired -= memLeftInElement;

            /* Current element was too small, copy remainder into new element */
            if (memRequired < UT_STRBUF_ELEMENT_SIZE) {
                /* A standard-size buffer is large enough for the new string */
                ut_strbuf_grow(b);

                /* Copy the remainder to the new buffer */
                if (n) {
                    /* If a max number of characters to write is set, only a
                     * subset of the string should be copied to the buffer */
                    strncpy(
                        ut_strbuf_ptr(b),
                        str + memLeftInElement,
                        memRequired);
                } else {
                    strcpy(ut_strbuf_ptr(b), str + memLeftInElement);
                }

                /* Update to number of characters copied to new buffer */
                b->current->pos += memRequired;
            } else {
                char *remainder = strdup(str + memLeftInElement);
                ut_strbuf_grow_str(b, remainder, remainder, memRequired);
            }
        } else {
            /* If string is a format string, a new buffer of size memRequired is
             * needed to re-evaluate the format string and only use the part that
             * wasn't already copied to the previous element */
            if (memRequired <= UT_STRBUF_ELEMENT_SIZE) {
                /* Resulting string fits in standard-size buffer. Note that the
                 * entire string needs to fit, not just the remainder, as the
                 * format string cannot be partially evaluated */
                ut_strbuf_grow(b);

                /* Copy entire string to new buffer */
                vsprintf(ut_strbuf_ptr(b), str, arg_cpy);

                /* Ignore the part of the string that was copied into the
                 * previous buffer. The string copied into the new buffer could
                 * be memmoved so that only the remainder is left, but that is
                 * most likely more expensive than just keeping the entire
                 * string. */

                /* Update position in buffer */
                b->current->pos += memRequired;
            } else {
                /* Resulting string does not fit in standard-size buffer.
                 * Allocate a new buffer that can hold the entire string. */
                char *dst = malloc(memRequired + 1);
                vsprintf(dst, str, arg_cpy);
                ut_strbuf_grow_str(b, dst, dst, memRequired);
            }
        }
    } else {
        /* Buffer max has been reached */
        result = false;
    }

    if (fmt_string) {
        va_end(arg_cpy);
    }

    return result;
}

static
bool ut_strbuf_appendstrn(
    ut_strbuf *b,
    const char* str,
    uint32_t len)
{
    va_list args;
    return ut_strbuf_append_intern(
        b, str, len, false, args
    );
}

static
bool ut_strbuf_appendstr(
    ut_strbuf *b,
    const char* str)
{
    va_list args;
    return ut_strbuf_append_intern(
        b, str, -1, false, args
    );
}

static
char* ut_strbuf_get(ut_strbuf *b) {
    char* result = NULL;

    if (b->elementCount) {
        if (b->buf) {
            result = strdup(b->buf);
        } else {
            void *next = NULL;
            uint32_t len = b->size + b->current->pos + 1;

            ut_strbuf_element *e = (ut_strbuf_element*)&b->firstElement;

            result = malloc(len);
            char* ptr = result;

            do {
                memcpy(ptr, e->buf, e->pos);
                ptr += e->pos;
                next = e->next;
                if (e != &b->firstElement.super) {
                    if (!e->buffer_embedded) {
                        free(((ut_strbuf_element_str*)e)->alloc_str);
                    }
                    free(e);
                }
            } while ((e = next));

            result[len - 1] = '\0';
        }
    } else {
        result = NULL;
    }

    b->elementCount = 0;

    return result;
}

/* -- COLORIZE FUNCTION -- */

char* colorize(
    char *msg) 
{
    ut_strbuf buff = UT_STRBUF_INIT;
    char *ptr, ch, prev = '\0';
    bool isNum = false;
    char isStr = '\0';
    bool isVar = false;
    bool overrideColor = false;
    bool autoColor = true;
    bool dontAppend = false;

    for (ptr = msg; (ch = *ptr); ptr++) {
        dontAppend = false;

        if (!overrideColor) {
            if (isNum && !isdigit(ch) && !isalpha(ch) && (ch != '.') && (ch != '%')) {
                if (UT_LOG_USE_COLORS) ut_strbuf_appendstr(&buff, UT_NORMAL);
                isNum = false;
            }
            if (isStr && (isStr == ch) && prev != '\\') {
                isStr = '\0';
            } else if (((ch == '\'') || (ch == '"')) && !isStr && !isalpha(prev) && (prev != '\\')) {
                if (UT_LOG_USE_COLORS) ut_strbuf_appendstr(&buff, UT_CYAN);
                isStr = ch;
            }

            if ((isdigit(ch) || (ch == '%' && isdigit(prev)) || (ch == '-' && isdigit(ptr[1]))) && !isNum && !isStr && !isVar && !isalpha(prev) && !isdigit(prev) && (prev != '_') && (prev != '.')) {
                if (UT_LOG_USE_COLORS) ut_strbuf_appendstr(&buff, UT_GREEN);
                isNum = true;
            }

            if (isVar && !isalpha(ch) && !isdigit(ch) && ch != '_') {
                if (UT_LOG_USE_COLORS) ut_strbuf_appendstr(&buff, UT_NORMAL);
                isVar = false;
            }

            if (!isStr && !isVar && ch == '$' && isalpha(ptr[1])) {
                if (UT_LOG_USE_COLORS) ut_strbuf_appendstr(&buff, UT_CYAN);
                isVar = true;
            }
        }

        if (!isVar && !isStr && !isNum && ch == '#' && ptr[1] == '[') {
            bool isColor = true;
            overrideColor = true;

            /* Custom colors */
            if (!strncmp(&ptr[2], "]", strlen("]"))) {
                autoColor = false;
            } else if (!strncmp(&ptr[2], "green]", strlen("green]"))) {
                if (UT_LOG_USE_COLORS) ut_strbuf_appendstr(&buff, UT_GREEN);
            } else if (!strncmp(&ptr[2], "red]", strlen("red]"))) {
                if (UT_LOG_USE_COLORS) ut_strbuf_appendstr(&buff, UT_RED);
            } else if (!strncmp(&ptr[2], "blue]", strlen("red]"))) {
                if (UT_LOG_USE_COLORS) ut_strbuf_appendstr(&buff, UT_BLUE);
            } else if (!strncmp(&ptr[2], "magenta]", strlen("magenta]"))) {
                if (UT_LOG_USE_COLORS) ut_strbuf_appendstr(&buff, UT_MAGENTA);
            } else if (!strncmp(&ptr[2], "cyan]", strlen("cyan]"))) {
                if (UT_LOG_USE_COLORS) ut_strbuf_appendstr(&buff, UT_CYAN);
            } else if (!strncmp(&ptr[2], "yellow]", strlen("yellow]"))) {
                if (UT_LOG_USE_COLORS) ut_strbuf_appendstr(&buff, UT_YELLOW);
            } else if (!strncmp(&ptr[2], "grey]", strlen("grey]"))) {
                if (UT_LOG_USE_COLORS) ut_strbuf_appendstr(&buff, UT_GREY);
            } else if (!strncmp(&ptr[2], "white]", strlen("white]"))) {
                if (UT_LOG_USE_COLORS) ut_strbuf_appendstr(&buff, UT_NORMAL);
            } else if (!strncmp(&ptr[2], "bold]", strlen("bold]"))) {
                if (UT_LOG_USE_COLORS) ut_strbuf_appendstr(&buff, UT_BOLD);
            } else if (!strncmp(&ptr[2], "normal]", strlen("normal]"))) {
                if (UT_LOG_USE_COLORS) ut_strbuf_appendstr(&buff, UT_NORMAL);
            } else if (!strncmp(&ptr[2], "reset]", strlen("reset]"))) {
                overrideColor = false;
                if (UT_LOG_USE_COLORS) ut_strbuf_appendstr(&buff, UT_NORMAL);
            } else {
                isColor = false;
                overrideColor = false;
            }

            if (isColor) {
                ptr += 2;
                while ((ch = *ptr) != ']') ptr ++;
                dontAppend = true;
            }
            if (!autoColor) {
                overrideColor = true;
            }
        }

        if (!dontAppend) {
            ut_strbuf_appendstrn(&buff, ptr, 1);
        }

        if (!overrideColor) {
            if (((ch == '\'') || (ch == '"')) && !isStr) {
                if (UT_LOG_USE_COLORS) ut_strbuf_appendstr(&buff, UT_NORMAL);
            }
        }

        prev = ch;
    }

    if (isNum || isStr || isVar || overrideColor) {
        if (UT_LOG_USE_COLORS) ut_strbuf_appendstr(&buff, UT_NORMAL);
    }

    return ut_strbuf_get(&buff);
}

/* -- DEMO -- */

int main(int argc, char * argv[]) {
    char *msg = colorize("Colorize 'demo' version 1.0");
    printf ("%s\n", msg);
    free (msg);

    msg = colorize("Here's an $ENVIRONMENT_VARIABLE");
    printf ("%s\n", msg);
    free (msg);

    msg = colorize("Percentage signs are handled 100% correctly");
    printf ("%s\n", msg);
    free (msg);

    msg = colorize("Numbers after \"letters\" are not colorized: x64_86");
    printf ("%s\n", msg);
    free (msg); 

    msg = colorize("#[red]Sometimes #[yellow]you #[green]just #[blue]want #[magenta]more #[reset]freedom.");
    printf ("%s\n", msg);
    free (msg);    
}


