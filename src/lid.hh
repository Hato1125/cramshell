#ifndef _CLAMSHELL_LID_HH
#define _CLAMSHELL_LID_HH

#include <optional>

namespace clamshell {
  std::optional<int> get_lid_fd() noexcept;
  bool get_lid_closed(int fd) noexcept;
}

#endif
