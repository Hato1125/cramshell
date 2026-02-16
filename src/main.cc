#include <print>
#include <thread>
#include <chrono>

#include "lid.hh"
#include "suspend.hh"
#include "display.hh"

using namespace std::chrono_literals;

int main() {
  if (!clamshell::has_lid()) {
    std::println("Clamshell is not possible with this device as there is no lid.");
    return EXIT_FAILURE;
  }

  if (!clamshell::check_suspend_caps()) {
    std::println("No suspend available for this device.");
    return EXIT_FAILURE;
  }

  while (true) {
    if (clamshell::get_display_count() == 1) {
      if (!clamshell::is_open_lid()) {
        // Program execution stops here during suspend, preventing multiple
        // suspend requests while the system is already suspended.
        clamshell::suspend();
        clamshell::resume();
      }
    }

    std::this_thread::sleep_for(1s);
  }

  return EXIT_SUCCESS;
}
