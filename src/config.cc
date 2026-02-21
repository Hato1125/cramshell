#include <format>
#include <filesystem>

#define TOML_EXCEPTIONS 0
#include <toml++/toml.hpp>
#include <xdgcpp/xdg.h>

#include "config.hh"

namespace {
  constexpr const char* dir = "clamshell";
  constexpr const char* file = "config.conf";

  using namespace clamshell::config;
  using namespace std::string_view_literals;

  nvidia_method parse_nvidia_method(std::string_view type) noexcept {
    if (type == "official_script") return nvidia_method::official_script;
    if (type == "direct_proc") return nvidia_method::direct_proc;
    return nvidia_method::official_script;
  }

  suspend_mode parse_suspend_mode(std::string_view mode) noexcept {
    if (mode == "freeze") return suspend_mode::freeze;
    if (mode == "suspend_to_ram") return suspend_mode::suspend_to_ram;
    if (mode == "suspend_to_disk") return suspend_mode::suspend_to_disk;
    return suspend_mode::freeze;
  }

  void append(std::string_view path) noexcept {
    toml::parse_result result = toml::parse_file(path);

    if (result) {
      const auto& table = result.table();

      fallback = table["fallback"].value_or(false);
      nvidia_method_type = parse_nvidia_method(
        table["nvidia_method_type"].value_or(""sv)
      );
      suspend_mode_type = parse_suspend_mode(
        table["suspend_mode_type"].value_or(""sv)
      );
    }
  }
}

namespace clamshell::config {
  void load() noexcept {
    const auto user_path = std::format(
      "{}/{}/{}",
      xdg::config().home().string(),
      dir,
      file
    );

    if (std::filesystem::exists(user_path)) {
      append(user_path);
      return;
    }

    const auto system_path = std::format(
      "/etc/{}/{}",
      dir,
      file
    );

    if (std::filesystem::exists(system_path)) {
      append(system_path);
    }
  }
}
