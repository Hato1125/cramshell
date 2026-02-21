#ifndef _CLAMSHELL_NOTIFY_HH
#define _CLAMSHELL_NOTIFY_HH

namespace clamshell::notify {
  void init() noexcept;
  void deinit() noexcept;
  void send(
    const char* name,
    const char* icon,
    const char* summary,
    const char* body,
    int timeout = 5000
  ) noexcept;
}

#endif
