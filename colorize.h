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
 * @section buffer Buffer API
 * @brief Utility API for efficient appending to strings.
 */

#ifndef CORTO_COLORIZE_H_
#define CORTO_COLORIZE_H_

#include "base.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Colorize a random unformatted string.
 *
 * This function returns a colorized version of the input string. It colorizes:
 * - numbers
 * - quoted sections (hello 'world')
 * - environment variables ($MY_ENV_VARIABLE)
 *
 * The function handles many edge cases, like dots and percentage signs in
 * numbers, ignoring numbers that come directly after a letter etc.
 * 
 * @param msg The input string
 * @return A colorized version of the input string.
 */
char* colorize(
    char *msg);

#ifdef __cplusplus
}
#endif

#endif
