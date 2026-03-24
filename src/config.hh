#ifndef _CLAMSHELL_CONFIG_HH
#define _CLAMSHELL_CONFIG_HH

#include <cstdint>
#include <string_view>

namespace config {
  enum class nvidia_method : std::uint8_t {
    official_script,
    direct_proc,
  };

  enum class suspend_mode : std::uint8_t {
    freeze,
    suspend_to_ram,
    suspend_to_disk,
  };

  inline bool fallback = false;
  inline nvidia_method nvidia_method_type = nvidia_method::official_script;
  inline suspend_mode suspend_mode_type = suspend_mode::freeze;

  void load() noexcept;

  constexpr std::string_view to_string(suspend_mode m) noexcept {
    switch (m) {
      case suspend_mode::freeze: return "freeze";
      case suspend_mode::suspend_to_ram: return "suspend_to_ram";
      case suspend_mode::suspend_to_disk: return "suspend_to_disk";
    }
    return "";
  }
}

#endif
