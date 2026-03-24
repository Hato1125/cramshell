#include <bitset>
#include <filesystem>

#include <fcntl.h>
#include <unistd.h>

#include <linux/input.h>
#include <linux/input-event-codes.h>

#include <sys/ioctl.h>
#include <sys/epoll.h>

#include "lid.hh"

namespace {
  bool has_sw_event(int fd) {
    std::bitset<EV_MAX + 1> bit;
    return ioctl(fd, EVIOCGBIT(0, sizeof(bit)), &bit) >= 0 && bit[EV_SW];
  }

  bool has_lid_event(int fd) {
    std::bitset<EV_MAX + 1> bit;
    return ioctl(fd, EVIOCGBIT(EV_SW, sizeof(bit)), &bit) >= 0 && bit[SW_LID];
  }
}

std::optional<utils::unique_fd> get_lid_fd() noexcept {
  for (const auto& file : std::filesystem::directory_iterator("/dev/input/")) {
    if (int fd = open(file.path().c_str(), O_RDONLY); fd >= 0) {
      if (has_sw_event(fd) && has_lid_event(fd)) {
        return fd;
      }

      close(fd);
    }
  }

  return std::nullopt;
}

bool get_lid_closed(utils::unique_fd& fd) noexcept {
  std::bitset<SW_MAX + 1> bit;
  return ioctl(fd, EVIOCGSW(sizeof(bit)), &bit) >= 0 && bit[SW_LID];
}
