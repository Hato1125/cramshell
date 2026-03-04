#include "log.hh"
#include "lid.hh"
#include "suspend.hh"
#include "display.hh"
#include "config.hh"

int main() {
  clamshell::config::load();

  if (!clamshell::has_lid()) {
    CLAMSHELL_FATAL("clamshell is not possible with this device as there is no lid");
    return EXIT_FAILURE;
  }

  if (!clamshell::check_suspend_caps()) {
    CLAMSHELL_FATAL("no suspend available for this device");
    return EXIT_FAILURE;
  }

  clamshell::poll_lid([](bool closed) {
    if (clamshell::get_display_count() == 1) {
      // Program execution stops here during suspend, preventing multiple
      // suspend requests while the system is already suspended.
      clamshell::suspend();
    }
  });

  return EXIT_SUCCESS;
}
