/*  MIT License

© 2011 Connor Lane Smith <cls@lubutu.com>
© 2011-2016 Dimitris Papastamos <sin@2f30.org>
© 2014-2016 Laslo Hunhold <dev@frign.de>

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.

*/

/* suckless sbase repo: https://git.suckless.org/sbase/ */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static void xvprintf(const char *fmt, va_list ap) {
    vfprintf(stderr, fmt, ap);

    if (fmt[0] && fmt[strlen(fmt) - 1] == ':') {
        fputc(' ', stderr);
        perror(NULL);
    }
}

void eprintf(const char *fmt, ...) {
    va_list ap;

    va_start(ap, fmt);
    xvprintf(fmt, ap);
    va_end(ap);

    exit(EXIT_FAILURE);
}
