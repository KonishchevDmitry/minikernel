#ifndef TEXTIO_H
#define TEXTIO_H

#include <stdarg.h>

void printf(const char* s, ...);
void printlnf(const char* s, ...);
void printf_args(const char* s, va_list args);

#endif
