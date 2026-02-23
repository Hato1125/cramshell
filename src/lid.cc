#include <filesystem>
#include <string>
#include <fstream>

#include "lid.hh"

namespace {
  constexpr const char* lid_path = "/proc/acpi/button/lid/LID/state";
}

namespace clamshell {
  bool has_lid() noexcept {
    return std::filesystem::exists(lid_path);
  }

  bool is_open_lid() noexcept {
    static std::ifstream lid(lid_path);

    if (lid.is_open()) {
      lid.clear();
      lid.seekg(0);
      std::string str;
      std::getline(lid, str);

      if (str.contains("closed")) {
        return false;
      }
    }

    return true;
  }
}
