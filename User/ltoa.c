/*
**  LTOA.C
**
**  Converts a long integer to a string.
**
**  Copyright 1988-90 by Robert B. Stout dba MicroFirm
**
**  Released to public domain, 1991
**
**  Parameters: 1 - number to be converted
**              2 - buffer in which to build the converted string
**              3 - number base to use for conversion
**
**  Returns:  A character pointer to the converted string if
**            successful, a NULL pointer if the number base specified
**            is out of range.
*/

#include "ltoa.h"

#include <stdlib.h>
#include <string.h>

#define BUFSIZE (sizeof(long) * 8 + 1)

char *_ltoa(long val, char *s, int base)
{
    register int i = 2;
    long uarg;
    char *tail, *head = s, buf[BUFSIZE];

    if (36 < base || 2 > base)
        base = 10;                    /* can only use 0-9, A-Z        */
    tail = &buf[BUFSIZE - 1];           /* last character position      */
    *tail-- = '\0';

    if (10 == base && val < 0L)
    {
        *head++ = '-';
        uarg    = -val;
    }
    else  uarg = val;

    if (uarg)
    {
        for (i = 1; uarg; ++i)
        {
            register ldiv_t r;

            r       = ldiv(uarg, base);
            *tail-- = (char)(r.rem + ((9L < r.rem) ?
                                      ('A' - 10L) : '0'));
            uarg    = r.quot;
        }
    }
    else  *tail-- = '0';

    memcpy(head, ++tail, i);
    return s;
}

char* _ultoa(unsigned long val, char* s, int base) {
    static const char dig[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    char* p, *q;

    q = s;
    do {
        *q++ = dig[val % base];
        val /= base;
    } while (val);
    *q = '\0';

    // Reverse the string (but leave the \0)
    p = s;
    q--;

    while (p < q) {
        char c = *p;
        *p++ = *q;
        *q-- = c;
    }

    return s;
}