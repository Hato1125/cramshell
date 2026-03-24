#include <cstdlib>
#include <fstream>

#include "log.hh"
#include "config.hh"

namespace {
  #define SCRIPT_PATH "/usr/bin/nvidia-sleep.sh "
  constexpr const char* hibernate_cmd = SCRIPT_PATH "hibernate";
  constexpr const char* suspend_cmd = SCRIPT_PATH "suspend";
  constexpr const char* thaw_cmd = SCRIPT_PATH "thaw";
  constexpr const char* resume_cmd = SCRIPT_PATH "resume";

  constexpr const char* suspend_path = "/proc/driver/nvidia/suspend";
}

namespace nvidia {
  void suspend(config::suspend_mode mode) noexcept {
    switch (config::nvidia_method_type) {
      case config::nvidia_method::official_script:
        if (mode == config::suspend_mode::suspend_to_disk) {
          CLAMSHELL_TRACE("execute nvidia suspend with \033[1mhibernate\033[22m");
          if (std::system(hibernate_cmd) != 0) {
            CLAMSHELL_ERROR("failed to execute nvidia hibernate");
          }
        } else {
          CLAMSHELL_TRACE("execute nvidia suspend with \033[1msuspend\033[22m");
          if (std::system(suspend_cmd) != 0) {
            CLAMSHELL_ERROR("failed to execute nvidia suspend");
          }
        }
        break;
      case config::nvidia_method::direct_proc:
        CLAMSHELL_TRACE("execute nvidia suspend with \033[1mdirect proc\033[22m");
        std::ofstream state(suspend_path);
        state << "suspend";
        if (!state.good()) {
          CLAMSHELL_ERROR("failed to write to \"{}\" for nvidia suspend", suspend_path);
        }
        break;
    }
  }
  void resume(config::suspend_mode mode) noexcept {
    switch (config::nvidia_method_type) {
      case config::nvidia_method::official_script:
        if (mode == config::suspend_mode::suspend_to_disk) {
          CLAMSHELL_TRACE("execute nvidia resume with \033[1mthaw\033[22m");
          if (std::system(thaw_cmd) != 0) {
            CLAMSHELL_ERROR("failed to execute nvidia thaw");
          }
        } else {
          CLAMSHELL_TRACE("execute nvidia resume with \033[1mresume\033[22m");
          if (std::system(resume_cmd) != 0) {
            CLAMSHELL_ERROR("failed to execute nvidia resume");
          }
        }
        break;
      case config::nvidia_method::direct_proc:
        CLAMSHELL_TRACE("execute nvidia resume with \033[1mdirect proc\033[22m");
        std::ofstream state(suspend_path);
        state << "resume";
        if (!state.good()) {
          CLAMSHELL_ERROR("failed to write to \"{}\" for nvidia resume", suspend_path);
        }
        break;
    }
  }
}
