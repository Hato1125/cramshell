#include <string>
#include <fstream>
#include <filesystem>

#include <unistd.h>

#include <sys/socket.h>
#include <linux/netlink.h>

#include "display.hh"

namespace {
  constexpr const char* display_dir = "/sys/class/drm/";
}

namespace clamshell {
  std::optional<unique_fd> get_display_fd() noexcept {
    int fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_KOBJECT_UEVENT);

    if (fd >= 0) {
      sockaddr_nl addr {
        .nl_family = AF_NETLINK,
        .nl_groups = 1,
      };

      if (bind(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) >= 0) {
        return fd;
      }

      close(fd);
    }

    return std::nullopt;
  }

  int get_display_count() noexcept {
    int count = 0;

    for (const auto& file : std::filesystem::directory_iterator(display_dir)) {
      const bool is_card = file
        .path()
        .filename()
        .string()
        .starts_with("card");

      if (!is_card) {
        continue;
      }

      std::ifstream card((file.path() / "status"));

      if (card.is_open()) {
        std::string status;
        std::getline(card, status);

        if (status == "connected") {
          count++;
        }
      }
    }

    return count;
  }
}
