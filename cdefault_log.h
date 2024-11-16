#ifndef CDEFAULT_LOG_H
#define CDEFAULT_LOG_H

#include <stdio.h>

#define CDEFAULT_LOG_FILENAME (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#define CDEFAULT_LOG_IMPL(level, fd, fmt, ...)                          \
  {                                                                     \
    fprintf(fd, "[%s|%s:%d] ", level, CDEFAULT_LOG_FILENAME, __LINE__); \
    fprintf(fd, fmt, __VA_ARGS__);                                      \
    fprintf(fd, "\n");                                                  \
  }
#define LOG_INFO(fmt, ...) CDEFAULT_LOG_IMPL("INFO", stdout, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...) CDEFAULT_LOG_IMPL("WARN", stdout, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) CDEFAULT_LOG_IMPL("ERROR", stderr, fmt, ##__VA_ARGS__)
#define LOG_FATAL(fmt, ...) { CDEFAULT_LOG_IMPL("FATAL", stderr, fmt, ##__VA_ARGS__); exit(1); }

#ifdef DEBUG
#define LOG_DEBUG(fmt, ...) CDEFAULT_LOG_IMPL("DEBUG", stdout, fmt, ##__VA_ARGS__)
#else
#define LOG_DEBUG(fmt, ...)
#endif

#endif
