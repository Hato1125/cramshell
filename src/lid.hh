#ifndef _CLAMSHELL_LID_HH
#define _CLAMSHELL_LID_HH

#include <optional>

#include "utils.hh"

std::optional<utils::unique_fd> get_lid_fd() noexcept;
bool get_lid_closed(utils::unique_fd& fd) noexcept;

#endif
