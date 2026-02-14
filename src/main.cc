#include <iostream>
#include <thread>
#include <chrono>

#include "lid.hh"
#include "suspend.hh"
#include "display.hh"

int main() {
  if (!clamshell::has_lid()) {
    std::cout << "Clamshell is not possible with this device as there is no lid.\n";
    return EXIT_FAILURE;
  }

  if (!clamshell::check_suspend_caps()) {
    std::cout << "No suspend available for this device.\n";
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

    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  return EXIT_SUCCESS;
}
