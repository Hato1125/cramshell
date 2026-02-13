#include <bit>
#include <iostream>
#include <thread>
#include <chrono>

#include "lid.hh"
#include "suspend.hh"
#include "display.hh"

int main() {
  if (!cramshell::has_lid()) {
    std::cout << "Clamshell is not possible with this device as there is no lid.\n";
    return EXIT_FAILURE;
  }

  const auto suspend_caps = cramshell::get_suspend_caps();

  if (std::bit_cast<unsigned char>(suspend_caps) == 0) {
    std::cout << "No suspend available for this device.\n";
    return EXIT_FAILURE;
  }

  while (true) {
    if (cramshell::get_display_count() == 1) {
      if (!cramshell::is_open_lid()) {
        // Program execution stops here during suspend, preventing multiple
        // suspend requests while the system is already suspended.
        cramshell::suspend(cramshell::suspend_type::freeze);
      }
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  return EXIT_SUCCESS;
}
