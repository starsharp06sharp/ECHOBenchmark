#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

void errexit (int status, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);

    vfprintf(stderr, format, ap);

    va_end(ap);

    exit(status);
}

#endif // UTILS_H_INCLUDED
