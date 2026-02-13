#include <string>
#include <fstream>
#include <filesystem>

#include "display.hh"

namespace cramshell {
  int get_display_count() noexcept {
    int count = 0;

    for (const auto& file : std::filesystem::directory_iterator("/sys/class/drm/")) {
      const auto fp = file.path();
      const auto root_path = fp.root_path();
      const auto rela_path = fp.relative_path();
      const auto path = root_path / rela_path / "status";
      std::ifstream card(path);

      if (card.is_open()) {
        std::string status;
        std::getline(card, status);
        if (status == "connected") {
          count++;
        }

        card.close();
      }
    }

    return count;
  }
}
