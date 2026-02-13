#include <filesystem>
#include <string>
#include <fstream>
#include <fstream>
#include <sstream>

#include "lid.hh"

namespace cramshell {
  bool has_lid() noexcept {
    return std::filesystem::exists("/proc/acpi/button/lid/LID/state");
  }

  bool is_open_lid() noexcept {
    std::ifstream lid("/proc/acpi/button/lid/LID/state");

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
      lid.close();
    }

    return true;
  }
}
