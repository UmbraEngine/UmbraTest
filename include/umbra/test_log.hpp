#pragma once

#include <format>
#include <source_location>
#include <cstdio>
#include <cstdarg>
#include <string_view>
#include <array>
#include "test_macro_helpers.h"

namespace Umbra::Test::Log {

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

enum class LogColor : unsigned {
  Default = 0,
  Red = 1,
  Green = 2,
  Yellow = 3,
  Blue = 4,
  Magenta = 5,
  Cyan = 6,
  White = 7,
};

constexpr std::array<std::string_view, 9> ansiCodes = {
    LOG_COLOR_CODE_DEFAULT, LOG_COLOR_CODE_RED,     LOG_COLOR_CODE_GREEN, LOG_COLOR_CODE_YELLOW,
    LOG_COLOR_CODE_BLUE,    LOG_COLOR_CODE_MAGENTA, LOG_COLOR_CODE_CYAN,  LOG_COLOR_CODE_WHITE
};

constexpr std::string_view getAnsiCode(LogColor color)
{
  return ansiCodes[static_cast<std::size_t>(color)];
}

template <typename... Args>
UMBRA_MAYBE_UNUSED inline void testLogPrint(
    LogColor color,
    const char* msg,
    Args&&... args,
    const std::source_location& loc = std::source_location::current()
)
{
  std::string_view strview = std::string_view(msg);
  auto fmt = std::format(strview, args...);

  if (color != LogColor::Default) {
    std::fputs(getAnsiCode(color).data(), stdout);
  }

  std::fprintf(stdout, "[%s:%d] ", loc.file_name(), loc.line());

  std::fprintf(stdout, fmt.data(), std::forward<Args>(args)...);
  std::fputc('\n', stdout);

  if (color != LogColor::Default) {
    std::fputs(getAnsiCode(LogColor::Default).data(), stdout);
  }
}

#define TEST_LOG_PRINT(color, fmt, ...)                                                            \
  do {                                                                                             \
    Umbra::Test::Log::testLogPrint(color, fmt, ##__VA_ARGS__);                                     \
  } while (0)

#define TAG_INFO "[INFO] "
#define TAG_WARN "[WARNING] "
#define TAG_ERROR "[ERROR] "
#define TAG_DEBUG "[DEBUG] "

#define TEST_LOG_INFO(fmt, ...)                                                                    \
  TEST_LOG_PRINT(::Umbra::Test::Log::LogColor::Default, TAG_INFO fmt "\n", ##__VA_ARGS__)
#define TEST_LOG_WARN(fmt, ...)                                                                    \
  TEST_LOG_PRINT(::Umbra::Test::Log::LogColor::Yellow, TAG_WARN fmt "\n", ##__VA_ARGS__)
#define TEST_LOG_ERROR(fmt, ...)                                                                   \
  TEST_LOG_PRINT(::Umbra::Test::Log::LogColor::Red, TAG_ERROR fmt "\n", ##__VA_ARGS__)
#define TEST_LOG_DEBUG(fmt, ...)                                                                   \
  TEST_LOG_PRINT(::Umbra::Test::Log::LogColor::Magenta, TAG_DEBUG fmt "\n", ##__VA_ARGS__)

} // namespace Umbra::Test::Log
  //
