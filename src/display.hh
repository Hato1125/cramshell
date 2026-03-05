#ifndef _CLAMSHELL_DISPLAY_HH
#define _CLAMSHELL_DISPLAY_HH

#include <optional>

namespace clamshell {
  std::optional<int> get_display_fd() noexcept;
  int get_display_count() noexcept;
}

#endif
