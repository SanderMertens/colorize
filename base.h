/* Copyright (c) 2010-2017 the corto developers
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/** @file
 * @section base Corto base platform definitions.
 * @brief Provide a stable platform/compiler independent set of definitions
 */

#ifndef CORTO_BASE_H
#define CORTO_BASE_H

/* Standard C library */
#include <ctype.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Boolean definitions (compatible with C++ and C99 stdbool) */
#ifndef __cplusplus
#undef bool
#undef true
#undef false
#define bool char
#define false 0
#define true !false
#endif

#ifndef FALSE
#define FALSE (0)
#endif

#ifndef TRUE
#define TRUE (!FALSE)
#endif

/* NULL pointer value */
#ifndef NULL
#define NULL (0x0)
#endif

/* Color constants */
#define CORTO_BLACK   "\033[1;30m"
#define CORTO_RED     "\033[0;31m"
#define CORTO_GREEN   "\033[0;32m"
#define CORTO_YELLOW  "\033[0;33m"
#define CORTO_BLUE    "\033[0;34m"
#define CORTO_MAGENTA "\033[0;35m"
#define CORTO_CYAN    "\033[0;36m"
#define CORTO_WHITE   "\033[1;37m"
#define CORTO_GREY    "\033[0;37m"
#define CORTO_NORMAL  "\033[0;49m"
#define CORTO_BOLD    "\033[1;49m"

#ifdef __cplusplus
}
#endif

#endif /* CORTO_BASE_H */
