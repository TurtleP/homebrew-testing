#pragma once

#include <stdarg.h>
#include <stdio.h>

#include <switch.h>

static constexpr const char* LOG_FORMAT = "%s:%zu:\n%s\n\n";

void Output(const char* func, size_t line, const char* format, ...);

#define LOG(format, ...) Output(__PRETTY_FUNCTION__, __LINE__, format, ##__VA_ARGS__)
