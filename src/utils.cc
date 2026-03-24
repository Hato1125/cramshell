#include <compare>
#include <utility>

#include "utils.hh"

namespace utils {
  unique_fd::unique_fd(int fd) noexcept : fd(fd) {}
  unique_fd::~unique_fd() noexcept {
    if (fd >= 0) {
      close(fd);
    }
  }

  unique_fd::unique_fd(unique_fd&& other) noexcept
    : fd(std::exchange(other.fd, invalid_fd)) {}

  unique_fd& unique_fd::operator=(unique_fd&& other) noexcept {
    if (this != &other) {
      if (fd >= 0) {
        close(fd);
      }
      fd = std::exchange(other.fd, invalid_fd);
    }
    return *this;
  }

  unique_fd::operator int() const noexcept { return fd; }
  unique_fd::operator bool() const noexcept { return fd >= 0; }

  auto unique_fd::operator<=>(int rhs) const noexcept { return fd <=> rhs; }
  bool unique_fd::operator==(int rhs) const noexcept { return fd == rhs; }
}
