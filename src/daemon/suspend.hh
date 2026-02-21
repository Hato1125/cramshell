#ifndef _CRAMSHELL_SUSPEND_HH
#define _CRAMSHELL_SUSPEND_HH

namespace clamd {
  bool check_suspend_caps() noexcept;
  void suspend() noexcept;
}

#endif
