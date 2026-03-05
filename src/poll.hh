#ifndef _CLAMSHELL_POLL_HH
#define _CLAMSHELL_POLL_HH

#include <functional>

namespace clamshell {
  void poll(
    std::function<void(bool closed, int displays)> hook
  ) noexcept;
}

#endif
