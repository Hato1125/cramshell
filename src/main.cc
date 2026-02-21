#include <thread>

#include "log.hh"
#include "lid.hh"
#include "suspend.hh"
#include "display.hh"
#include "config.hh"

using namespace std::chrono_literals;

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

  while (true) {
    CLAMSHELL_INFO("display count: \033[1m{}\033[22m", clamshell::get_display_count());
    CLAMSHELL_INFO("lid open: \033[1m{}\033[22m", clamshell::is_open_lid());
    if (clamshell::get_display_count() == 1) {
      if (!clamshell::is_open_lid()) {
        // Program execution stops here during suspend, preventing multiple
        // suspend requests while the system is already suspended.
        clamshell::suspend();
      }
    }

    std::this_thread::sleep_for(1s);
  }

  return EXIT_SUCCESS;
}
