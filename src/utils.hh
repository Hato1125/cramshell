#ifndef _CLAMSHELL_UTILS_HH
#define _CLAMSHELL_UTILS_HH

#include <unistd.h>

namespace utils {
  constexpr int invalid_fd = -1;

  struct unique_fd {
    int fd = invalid_fd;

    unique_fd(int fd) noexcept;
    ~unique_fd() noexcept;

    unique_fd(const unique_fd&) = delete;
    unique_fd& operator=(const unique_fd&) = delete;

    unique_fd(unique_fd&& other) noexcept;
    unique_fd& operator=(unique_fd&& other) noexcept;

    operator int() const noexcept;
    operator bool() const noexcept;

    auto operator<=>(int rhs) const noexcept;
    bool operator==(int rhs) const noexcept;
  };
}

#endif
