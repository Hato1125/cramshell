#include <thread>

#include "log.hh"
#include "lid.hh"
#include "suspend.hh"
#include "display.hh"
#include "config.hh"

using namespace std::chrono_literals;

int main() {
  clamd::config::load();

  if (!clamd::has_lid()) {
    CLAMSHELL_FATAL("clamshell is not possible with this device as there is no lid");
    return EXIT_FAILURE;
  }

  if (!clamd::check_suspend_caps()) {
    CLAMSHELL_FATAL("no suspend available for this device");
    return EXIT_FAILURE;
  }

  while (true) {
    CLAMSHELL_INFO("display count: \033[1m{}\033[22m", clamd::get_display_count());
    CLAMSHELL_INFO("lid open: \033[1m{}\033[22m", clamd::is_open_lid());
    if (clamd::get_display_count() == 1) {
      if (!clamd::is_open_lid()) {
        // Program execution stops here during suspend, preventing multiple
        // suspend requests while the system is already suspended.
        clamd::suspend();
      }
    }

    std::this_thread::sleep_for(1s);
  }

  return EXIT_SUCCESS;
}
