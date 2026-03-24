#ifndef _CLAMSHELL_DISPLAY_HH
#define _CLAMSHELL_DISPLAY_HH

#include <optional>

#include "utils.hh"

std::optional<utils::unique_fd> get_display_fd() noexcept;
int get_display_count() noexcept;

#endif
