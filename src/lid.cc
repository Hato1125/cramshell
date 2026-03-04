#include <cstring>
#include <fcntl.h>
#include <unistd.h>

#include <linux/input.h>
#include <linux/input-event-codes.h>

#include <sys/ioctl.h>
#include <sys/epoll.h>

#include <bitset>
#include <filesystem>

#include "log.hh"
#include "lid.hh"

namespace {
  constexpr const char* lid_dir = "/proc/acpi/button/lid";

  int lid;

  bool has_sw_event(int fd) {
    std::bitset<EV_MAX + 1> bit;
    return ioctl(fd, EVIOCGBIT(0, sizeof(bit)), &bit) >= 0 && bit[EV_SW];
  }

  bool has_lid_event(int fd) {
    std::bitset<EV_MAX + 1> bit;
    return ioctl(fd, EVIOCGBIT(EV_SW, sizeof(bit)), &bit) >= 0 && bit[SW_LID];
  }
}

namespace clamshell {
  bool has_lid() noexcept {
    for (const auto& file : std::filesystem::directory_iterator("/dev/input/")) {
      if (int fd = open(file.path().c_str(), O_RDONLY); fd >= 0) {
        if (has_sw_event(fd) && has_lid_event(fd)) {
          lid = fd;
          return true;
        }

        close(fd);
      }
    }

    return false;
  }

  void poll_lid(std::function<void(bool closed)> hook) noexcept {
    int epfd = epoll_create1(0);
    if (epfd < 0) {
      CLAMSHELL_FATAL("failed to create epoll instance: {}", strerror(errno));
      return;
    }

    epoll_event event {
      .events = EPOLLIN,
      .data {
        .fd = lid
      }
    };

    if (epoll_ctl(epfd, EPOLL_CTL_ADD, lid, &event) < 0) {
      CLAMSHELL_FATAL("failed to register lid device to epoll: {}", strerror(errno));
      return;
    }

    epoll_event events[1];

    while (true) {
      if (epoll_wait(epfd, events, 1, -1) < 0) {
        CLAMSHELL_FATAL("epoll_wait failed: {}", strerror(errno));
        return;
      }

      input_event e;
      if (read(events[0].data.fd, &e, sizeof(e)) < 0) {
        CLAMSHELL_FATAL("failed to read lid event: {}", strerror(errno));
        return;
      }

      CLAMSHELL_INFO(
        "input_event {{\n  type = {}\n  code = {}\n  value = {}\n}}",
        e.type,
        e.code,
        e.value
      );

      if (e.type == EV_SW && e.code == SW_LID) {
        hook(e.value);
      }
    }
  }
}
