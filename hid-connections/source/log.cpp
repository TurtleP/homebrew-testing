#include "log.h"

static FILE* log = freopen("log.txt", "w", stderr);

void Output(const char* func, size_t line, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    char buffer[255];

    vsnprintf(buffer, sizeof(buffer), format, args);
    fprintf(log, LOG_FORMAT, func, line, buffer);
    fflush(log);
}
