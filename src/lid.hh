#ifndef _CLAMSHELL_LID_HH
#define _CLAMSHELL_LID_HH

#include <optional>

#include "utils.hh"

namespace clamshell {
  std::optional<unique_fd> get_lid_fd() noexcept;
  bool get_lid_closed(unique_fd& fd) noexcept;
}

#endif
