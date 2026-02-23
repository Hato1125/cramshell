#include <filesystem>
#include <string>
#include <fstream>
#include <sstream>

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
      std::string str;
      std::string part;
      std::getline(lid, str);
      std::istringstream iss(str);

      while (iss >> part) {
        if (part == "opened") {
          return true;
        }
        if (part == "closed") {
          return false;
        }
      }
    }

    return true;
  }
}
