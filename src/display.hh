#ifndef _CLAMSHELL_DISPLAY_HH
#define _CLAMSHELL_DISPLAY_HH

#include "utils.hh"

utils::unique_fd get_display_fd() noexcept;
int get_display_count() noexcept;

#endif
