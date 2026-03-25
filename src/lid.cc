#include <bitset>
#include <filesystem>

#include <fcntl.h>
#include <unistd.h>

#include <sys/ioctl.h>

#include <linux/input.h>
#include <linux/input-event-codes.h>


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

utils::unique_fd get_lid_fd() noexcept {
  for (const auto& file : std::filesystem::directory_iterator("/dev/input/")) {
    utils::unique_fd fd(open(file.path().c_str(), O_RDONLY));
    if (fd && has_sw_event(fd) && has_lid_event(fd)) {
      return fd;
    }
  }

  return utils::invalid_fd;
}

bool get_lid_closed(utils::unique_fd& fd) noexcept {
  std::bitset<SW_MAX + 1> bit;
  return ioctl(fd, EVIOCGSW(sizeof(bit)), &bit) >= 0 && bit[SW_LID];
}
