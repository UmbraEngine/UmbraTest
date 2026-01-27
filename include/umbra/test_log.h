#pragma once

#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include "test_macro_helpers.h"

#define LOG_COLOR_CODE_RED "\x1b[91m"
#define LOG_COLOR_CODE_GREEN "\x1b[92m"
#define LOG_COLOR_CODE_YELLOW "\x1b[93m"
#define LOG_COLOR_CODE_BLUE "\x1b[94m"
#define LOG_COLOR_CODE_MAGENTA "\x1b[95m"
#define LOG_COLOR_CODE_CYAN "\x1b[96m"
#define LOG_COLOR_CODE_WHITE "\x1b[97m"
#define LOG_COLOR_CODE_DEFAULT "\x1b[0m"

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
static bool enable_vt_mode(void)
{
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  if (hOut == INVALID_HANDLE_VALUE)
    return false;

  DWORD dwMode = 0;
  if (!GetConsoleMode(hOut, &dwMode))
    return false;

  dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  return SetConsoleMode(hOut, dwMode) != 0;
}
static const bool vt_enabled = enable_vt_mode();
#else
static const bool vt_enabled = true;
#endif

typedef enum {
  LOG_COLOR_DEFAULT = 0,
  LOG_COLOR_RED = 1,
  LOG_COLOR_GREEN = 2,
  LOG_COLOR_YELLOW = 3,
  LOG_COLOR_BLUE = 4,
  LOG_COLOR_MAGENTA = 5,
  LOG_COLOR_CYAN = 6,
  LOG_COLOR_WHITE = 7,
} LogColor;

UMBRA_MAYBE_UNUSED static const char* ansi_code_for_color(LogColor color)
{
  switch (color) {
  case LOG_COLOR_RED: {
    return LOG_COLOR_CODE_RED;
  }
  case LOG_COLOR_GREEN: {
    return LOG_COLOR_CODE_GREEN;
  }
  case LOG_COLOR_YELLOW: {
    return LOG_COLOR_CODE_YELLOW;
  }
  case LOG_COLOR_BLUE: {
    return LOG_COLOR_CODE_BLUE;
  }
  case LOG_COLOR_MAGENTA: {
    return LOG_COLOR_CODE_MAGENTA;
  }
  case LOG_COLOR_CYAN: {
    return LOG_COLOR_CODE_CYAN;
  }
  case LOG_COLOR_WHITE: {
    return LOG_COLOR_CODE_WHITE;
  }
  case LOG_COLOR_DEFAULT: {
    return LOG_COLOR_CODE_DEFAULT;
  }
  default: {
    return LOG_COLOR_CODE_DEFAULT;
  }
  }
}

UMBRA_MAYBE_UNUSED static void log(LogColor color, const char* fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  if (vt_enabled && (color) != LOG_COLOR_DEFAULT) {
    fputs(ansi_code_for_color(color), stdout);
  }
  if (args) {
    vprintf(fmt, args);
  }
  else {
    printf("%s", fmt);
  }
  va_end(args);
  fputs(ansi_code_for_color(LOG_COLOR_DEFAULT), stdout);
}

#define LOG_PRINT(color, fmt, ...)                                                                 \
  do {                                                                                             \
    log(color, fmt, ##__VA_ARGS__);                                                                \
  } while (0)

#define TAG_INFO "[INFO] "
#define TAG_WARN "[WARNING] "
#define TAG_ERROR "[ERROR] "
#define TAG_DEBUG "[DEBUG] "

#define TEST_LOG_INFO(fmt, ...)                                                                    \
  LOG_PRINT(LOG_COLOR_DEFAULT, TAG_INFO fmt "\n" __VA_OPT__(, )##__VA_ARGS__)
#define TEST_LOG_WARN(fmt, ...)                                                                    \
  LOG_PRINT(LOG_COLOR_YELLOW, TAG_WARN fmt "\n"__VA_OPT__(, )##__VA_ARGS__)
#define TEST_LOG_ERROR(fmt, ...)                                                                   \
  LOG_PRINT(LOG_COLOR_RED, TAG_ERROR fmt "\n" __VA_OPT__(, )##__VA_ARGS__)
#define TEST_LOG_DEBUG(fmt, ...)                                                                   \
  LOG_PRINT(LOG_COLOR_MAGENTA, TAG_DEBUG fmt "\n" __VA_OPT__(, )##__VA_ARGS__)
