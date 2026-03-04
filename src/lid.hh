#ifndef _CLAMSHELL_LID_HH
#define _CLAMSHELL_LID_HH

#include <functional>

namespace clamshell {
  bool has_lid() noexcept;
  void poll_lid(std::function<void()> hook) noexcept;
}

#endif
