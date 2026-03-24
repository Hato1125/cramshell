#ifndef _CLAMSHELL_POLL_HH
#define _CLAMSHELL_POLL_HH

#include <cstring>

#include <linux/input.h>
#include <linux/input-event-codes.h>

#include <sys/ioctl.h>
#include <sys/epoll.h>

#include "log.hh"
#include "lid.hh"
#include "display.hh"

template <void hook(bool closed, int displays)>
bool poll() noexcept {
  utils::unique_fd epfd(epoll_create1(0));
  if (!epfd) {
    CLAMSHELL_FATAL("failed to create epoll instance: {}", strerror(errno));
    return false;
  }

  auto lid = get_lid_fd();
  if (lid) {
    epoll_event event {
      .events = EPOLLIN,
      .data { .fd = lid }
    };

    if (epoll_ctl(epfd, EPOLL_CTL_ADD, lid, &event) < 0) {
      CLAMSHELL_FATAL("cannot monitor lid: epoll_ctl failed ({})", strerror(errno));
      return false;
    }
  } else {
    CLAMSHELL_FATAL("this device does not appear to have a lid");
    return false;
  }

  auto display = get_display_fd();
  if (display) {
    epoll_event event {
      .events = EPOLLIN,
      .data { .fd = display }
    };

    if (epoll_ctl(epfd, EPOLL_CTL_ADD, display, &event) < 0) {
      CLAMSHELL_FATAL("cannot monitor display hotplug: epoll_ctl failed ({})", strerror(errno));
      return false;
    }
  } else {
    CLAMSHELL_FATAL("cannot monitor display hotplug: failed to open netlink socket");
    return false;
  }

  epoll_event events[1];

  bool closed = get_lid_closed(lid);
  int displays = get_display_count();

  hook(closed, displays);

  while (true) {
    if (epoll_wait(epfd, events, 1, -1) < 0) {
      CLAMSHELL_FATAL("epoll_wait failed: {}", strerror(errno));
      return false;
    }

    if (events[0].data.fd == lid) {
      input_event e;

      if (read(events[0].data.fd, &e, sizeof(e)) < 0) {
        CLAMSHELL_FATAL("failed to read lid event: {}", strerror(errno));
        return false;
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
    } else if (events[0].data.fd == display) {
      char buffer[4096];

      const auto length = read(
        display,
        buffer,
        sizeof(buffer)
      );

      if (length > 0) {
        const std::string_view uevent(buffer, length);

        if (uevent.contains("SUBSYSTEM=drm") && uevent.contains("HOTPLUG=1")) {
#ifdef DEBUG
          if (get_display_count() > displays) {
            CLAMSHELL_TRACE("display connected");
          } else {
            CLAMSHELL_TRACE("display disconnected");
          }
#endif

          displays = get_display_count();

          hook(closed, displays);
        }
      }
    }
  }

  return true;
}

#endif
