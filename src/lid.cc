#include <filesystem>
#include <string>
#include <fstream>

#include "lid.hh"

namespace {
  constexpr const char* lid_dir = "/proc/acpi/button/lid";

  std::ifstream lid;
}

namespace clamshell {
  bool has_lid() noexcept {
    for (const auto& file : std::filesystem::directory_iterator(lid_dir)) {
      lid = std::ifstream(file.path() / "state");

      if (lid.is_open()) {
        return true;
      }
    }

    return false;
  }

  bool is_open_lid() noexcept {
    if (lid.is_open()) {
      lid.clear();
      lid.seekg(0);
      std::string str;
      std::getline(lid, str);

      return !str.contains("closed");
    }

    return false;
  }
}
