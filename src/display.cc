#include <string>
#include <fstream>
#include <filesystem>

#include "display.hh"

namespace {
  constexpr const char* display_dir = "/sys/class/drm/";
}

namespace clamshell {
  int get_display_count() noexcept {
    int count = 0;

    if (!std::filesystem::exists(display_dir)) {
      return count;
    }

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
