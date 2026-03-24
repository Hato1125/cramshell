#ifndef _CLAMSHELL_UTILS_HH
#define _CLAMSHELL_UTILS_HH

#include <unistd.h>

namespace utils {
  struct unique_fd {
    int fd = -1;

    unique_fd(int fd) noexcept;
    ~unique_fd() noexcept;

    operator int() const noexcept;
    operator bool() const noexcept;

    auto operator<=>(int rhs) const noexcept;
    bool operator==(int rhs) const noexcept;
  };

  unique_fd make_fd(int raw) noexcept;
}

#endif
