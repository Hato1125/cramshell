#ifndef _CRAMSHELL_SUSPEND_HH
#define _CRAMSHELL_SUSPEND_HH

#include <cstdint>

namespace cramshell {
  struct suspend_caps {
    unsigned char freeze : 1;
    unsigned char standby : 1;
    unsigned char mem: 1;
    unsigned char disk: 1;
    unsigned char _ : 4;
  };

  enum class suspend_type : std::uint8_t {
    freeze,
    standby,
    mem,
    disk,
  };

  suspend_caps get_suspend_caps() noexcept;
  void suspend(suspend_type type) noexcept;
}

#endif
