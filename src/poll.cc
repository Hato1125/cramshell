#include <cstring>

#include <fcntl.h>
#include <unistd.h>

#include <linux/input.h>
#include <linux/input-event-codes.h>

#include <sys/ioctl.h>
#include <sys/epoll.h>

#include "log.hh"
#include "lid.hh"
#include "display.hh"
#include "poll.hh"

namespace {
  struct fd_guard {
    int raw = -1;

    ~fd_guard() noexcept {
      if (raw >= 0) {
        close(raw);
      }
    }

    operator int() const noexcept { return raw; }
  };
}

namespace clamshell {
  void poll(
    std::function<void(bool closed, int displays)> hook
  ) noexcept {
    int epfd = epoll_create1(0);
    if (epfd < 0) {
      CLAMSHELL_FATAL("failed to create epoll instance: {}", strerror(errno));
      return;
    }

    fd_guard lid_fd;
    if (auto lid = get_lid_fd(); lid) {
      lid_fd.raw = *lid;

      epoll_event event {
        .events = EPOLLIN,
        .data { .fd = *lid }
      };

      if (epoll_ctl(epfd, EPOLL_CTL_ADD, *lid, &event) < 0) {
        CLAMSHELL_FATAL("cannot monitor lid: epoll_ctl failed ({})", strerror(errno));
        return;
      }
    } else {
      CLAMSHELL_FATAL("this device does not appear to have a lid");
      return;
    }

    fd_guard display_fd;
    if (auto display = get_display_fd(); display) {
      display_fd.raw = *display;

      epoll_event event {
        .events = EPOLLIN,
        .data { .fd = *display }
      };

      if (epoll_ctl(epfd, EPOLL_CTL_ADD, *display, &event) < 0) {
        CLAMSHELL_FATAL("cannot monitor display hotplug: epoll_ctl failed ({})", strerror(errno));
        return;
      }
    } else {
      CLAMSHELL_FATAL("cannot monitor display hotplug: failed to open netlink socket");
      return;
    }

    epoll_event events[1];

    bool closed = get_lid_closed(lid_fd);
    int displays = get_display_count();

    while (true) {
      if (epoll_wait(epfd, events, 1, -1) < 0) {
        CLAMSHELL_FATAL("epoll_wait failed: {}", strerror(errno));
        return;
      }

      if (events[0].data.fd == lid_fd) {
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
          closed = e.value;
          hook(closed, displays);
        }
      } else if (events[0].data.fd == display_fd) {
        char buffer[4096];

        const auto length = read(
          display_fd,
          buffer,
          sizeof(buffer)
        );

        if (length > 0) {
          if (std::string_view(buffer, length).contains("SUBSYSTEM=drm")) {
            displays = get_display_count();
            CLAMSHELL_INFO("display hotplug: displays = {}", displays);
            hook(closed, displays);
          }
        }
      }
    }
  }
}
