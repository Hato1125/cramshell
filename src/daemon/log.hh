#ifndef _CLAMSHELL_LOG_HH
#define _CLAMSHELL_LOG_HH

#include <print>
#include <format>
#include <string_view>
#include <source_location>

namespace detail {
  template <typename ...Args>
  void log(
    std::string_view esc,
    std::string_view category,
    std::source_location location,
    std::format_string<Args...> fmt,
    Args&&... args
  ) noexcept {
    std::println(
      "{}[clamshell::{} {}:{}] {}\033[0m",
      esc,
      category,
      location.line(),
      location.column(),
      std::format(fmt, std::forward<Args>(args)...)
    );
  }
}

#ifdef RELEASE
  #define CLAMSHELL_INFO(fmt, ...)
  #define CLAMSHELL_TRACE(fmt, ...)
  #define CLAMSHELL_WARN(fmt, ...)
  #define CLAMSHELL_ERROR(fmt, ...)
#else
  #define CLAMSHELL_INFO(fmt, ...) detail::log("\x1b[32m", "info", std::source_location::current(), fmt __VA_OPT__(, ) __VA_ARGS__)
  #define CLAMSHELL_TRACE(fmt, ...) detail::log("\x1b[34m", "trace", std::source_location::current(), fmt __VA_OPT__(, ) __VA_ARGS__)
  #define CLAMSHELL_WARN(fmt, ...) detail::log("\x1b[33m", "warn", std::source_location::current(), fmt __VA_OPT__(, ) __VA_ARGS__)
  #define CLAMSHELL_ERROR(fmt, ...) detail::log("\x1b[31m", "error", std::source_location::current(), fmt __VA_OPT__(, ) __VA_ARGS__)
#endif

#define CLAMSHELL_FATAL(fmt, ...) detail::log("\x1b[31m", "fatal", std::source_location::current(), fmt __VA_OPT__(, ) __VA_ARGS__)

#endif
