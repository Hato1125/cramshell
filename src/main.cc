#include <cstdlib>
#include <print>

#include "def.hh"
#include "log.hh"
#include "poll.hh"
#include "suspend.hh"
#include "config.hh"

int main() {
  std::println("START CLAMSHELL DAEMON");
  std::println(
    "{} MODE: v{}.{}.{}",
#ifdef DEBUG
    "DEBUG",
#else
    "RELEASE",
#endif
    clamshell::major,
    clamshell::minor,
    clamshell::patch
  );

  clamshell::config::load();

  if (!clamshell::check_suspend_caps()) {
    CLAMSHELL_FATAL("no suspend available for this device");
    return EXIT_FAILURE;
  }

  constexpr auto polling = [](bool closed, int displays) {
    if (closed && displays == 1) {
      // Program execution stops here during suspend, preventing multiple
      // suspend requests while the system is already suspended.
      clamshell::suspend();
    }
  };

  return clamshell::poll<polling>()
    ? EXIT_SUCCESS
    : EXIT_FAILURE;
}
