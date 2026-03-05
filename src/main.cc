#include "log.hh"
#include "poll.hh"
#include "suspend.hh"
#include "config.hh"

int main() {
  clamshell::config::load();

  if (!clamshell::check_suspend_caps()) {
    CLAMSHELL_FATAL("no suspend available for this device");
    return EXIT_FAILURE;
  }

  clamshell::poll([](bool closed, int displays) {
    if (closed && displays == 1) {
      // Program execution stops here during suspend, preventing multiple
      // suspend requests while the system is already suspended.
      clamshell::suspend();
    }
  });

  return EXIT_SUCCESS;
}
