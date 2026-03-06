#include <compare>

#include "utils.hh"

namespace clamshell {
  unique_fd::unique_fd(int fd) noexcept : fd(fd) {}
  unique_fd::~unique_fd() noexcept {
    if (fd >= 0) {
      close(fd);
    }
  }

  unique_fd::operator int() const noexcept { return fd; }
  unique_fd::operator bool() const noexcept { return fd >= 0; }

  auto unique_fd::operator<=>(int rhs) const noexcept { return fd <=> rhs; }
  bool unique_fd::operator==(int rhs) const noexcept { return fd == rhs; }

  unique_fd make_fd(int raw) noexcept { return unique_fd(raw); }
}
