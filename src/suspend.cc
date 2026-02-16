#include <bit>
#include <fstream>
#include <sstream>
#include <string>

#include <unistd.h>

#include "suspend.hh"

namespace {
  constexpr const char* power_state_path = "/sys/power/state";
  constexpr const char* nvidia_suspend_path = "/proc/driver/nvidia/suspend";
  constexpr const char* cgroup_freeze_path = "/sys/fs/cgroup/user.slice/cgroup.freeze";
  constexpr const char* cgroup_proc_path = "/sys/fs/cgroup/system.slice/cgroup.procs";

  struct suspend_caps {
    unsigned char freeze : 1;
    unsigned char standby : 1;
    unsigned char mem: 1;
    unsigned char disk: 1;
    unsigned char _ : 4;
  };

  suspend_caps caps {
    .freeze = 0,
    .standby = 0,
    .mem = 0,
    .disk = 0,
  };

  void write_suspend() noexcept {
    std::ofstream state(power_state_path);
    if (state.is_open()) {
      if (caps.freeze) {
        state << "freeze";
      } else if (caps.standby) {
        state << "standby";
      } else if (caps.mem) {
        state << "mem";
      } else if (caps.disk) {
        state << "disk";
      }
    }
  }

  void write_nvidia_suspend() noexcept {
    std::ofstream state(nvidia_suspend_path);
    if (state.is_open()) {
      state << "suspend";
    }
  }

  void write_nvidia_resume() noexcept {
    std::ofstream state(nvidia_suspend_path);
    if (state.is_open()) {
      state << "resume";
    }
  }

  bool freeze_user_processes() noexcept {
    std::ofstream file(cgroup_freeze_path);
    file << "1";
    return file.good();
  }


  bool unfreeze_user_processes() noexcept {
    std::ofstream file(cgroup_freeze_path);
    file << "0";
    return file.good();
  }

  bool move_self_to_system_slice() noexcept {
    std::ofstream file(cgroup_proc_path);
    file << getpid();
    return file.good();
  }
}

namespace clamshell {
  bool check_suspend_caps() noexcept {
    std::ifstream file(power_state_path);

    if (file.is_open()) {
      std::string state;
      std::string part;
      std::getline(file, state);
      std::istringstream iss(state);

      while (iss >> part) {
        if (part == "freeze") {
          caps.freeze = 1;
        } else if (part == "standby") {
          caps.standby = 1;
        } else if (part == "mem") {
          caps.mem = 1;
        } else if (part == "disk") {
          caps.disk = 1;
        }
      }
    }

    return std::bit_cast<unsigned char>(caps) != 0;
  }

  void suspend() noexcept {
    sync();

    if (move_self_to_system_slice() && freeze_user_processes()) {
      write_nvidia_suspend();
      write_suspend();
    }
  }

  void resume() noexcept {
    unfreeze_user_processes();
    write_nvidia_resume();
  }
}
