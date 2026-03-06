#ifndef _CLAMSHELL_POLL_HH
#define _CLAMSHELL_POLL_HH

#include <functional>

namespace clamshell {
  bool poll(
    std::function<void(bool closed, int displays)> hook
  ) noexcept;
}

#endif
