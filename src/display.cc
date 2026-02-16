#include <string>
#include <fstream>
#include <filesystem>

#include "display.hh"

namespace clamshell {
  int get_display_count() noexcept {
    int count = 0;

    for (const auto& file : std::filesystem::directory_iterator("/sys/class/drm/")) {
      bool is_card = file
        .path()
        .filename()
        .string()
        .starts_with("card");

      if (!is_card) {
        continue;
      }

      std::ifstream card((file.path() / "status").string());

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
