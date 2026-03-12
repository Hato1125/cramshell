#ifndef _CLAMSHELL_NVIDIA_HH
#define _CLAMSHELL_NVIDIA_HH

#include "config.hh"

namespace clamshell::nvidia {
  void suspend(config::suspend_mode mode) noexcept;
  void resume(config::suspend_mode mode) noexcept;
}

#endif
