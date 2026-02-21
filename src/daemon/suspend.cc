#include <fstream>
#include <sstream>
#include <string>

#include <unistd.h>

#include "suspend.hh"
#include "config.hh"
#include "log.hh"

namespace {
  using namespace clamshell;

  constexpr const char* power_state_path = "/sys/power/state";
  constexpr const char* mem_power_state_path = "/sys/power/mem_sleep";
  constexpr const char* nvidia_suspend_path = "/proc/driver/nvidia/suspend";
  constexpr const char* cgroup_freeze_path = "/sys/fs/cgroup/user.slice/cgroup.freeze";
  constexpr const char* cgroup_proc_path = "/sys/fs/cgroup/system.slice/cgroup.procs";

  config::suspend_mode use_suspend_mode;

  struct sleep_caps {
    unsigned char freeze : 1;
    unsigned char standby : 1;
    unsigned char mem: 1;
    unsigned char disk: 1;
    unsigned char _ : 4;
  };

  struct mem_sleep_caps {
    unsigned char s2idle : 1;
    unsigned char shallow : 1;
    unsigned char deep: 1;
    unsigned char _ : 5;
  };

  sleep_caps sleep_caps {
    .freeze = 0,
    .standby = 0,
    .mem = 0,
    .disk = 0,
  };

  mem_sleep_caps mem_sleep_caps {
    .s2idle = 0,
    .shallow = 0,
    .deep = 0,
  };

  void get_sleep_cap() noexcept {
    std::ifstream file(power_state_path);

    if (file.is_open()) {
      std::string state;
      std::string part;
      std::getline(file, state);
      std::istringstream iss(state);

      while (iss >> part) {
        if (part == "freeze") {
          sleep_caps.freeze = 1;
        } else if (part == "standby") {
          sleep_caps.standby = 1;
        } else if (part == "mem") {
          sleep_caps.mem = 1;
        } else if (part == "disk") {
          sleep_caps.disk = 1;
        }
      }
    }
  }

  void get_mem_sleep_cap() noexcept {
    std::ifstream file(mem_power_state_path);

    if (file.is_open()) {
      std::string state;
      std::string part;
      std::getline(file, state);
      std::istringstream iss(state);

      while (iss >> part) {
        if (!part.empty() && part.front() == '[') {
          part = part.substr(1, part.size() - 2);
        }

        if (part == "s2idle") {
          mem_sleep_caps.s2idle = 1;
        } else if (part == "shallow") {
          mem_sleep_caps.shallow = 1;
        } else if (part == "deep") {
          mem_sleep_caps.deep = 1;
        }
      }
    }
  }

  bool is_freeze_available() noexcept {
    return sleep_caps.freeze;
  }

  bool is_suspend_to_ram_available() noexcept {
    return sleep_caps.mem && mem_sleep_caps.deep;
  }

  bool is_suspend_to_disk_available() noexcept {
    return sleep_caps.disk;
  }

  void freeze() noexcept {
    CLAMSHELL_TRACE("execute suspend with \033[1mfreeze\033[22m");
    std::ofstream state(power_state_path);
    if (state.is_open()) {
      state << "freeze";
    }
  }

  void suspend_to_ram() noexcept {
    CLAMSHELL_TRACE("execute suspend with \033[1msuspend to ram\033[22m");
    std::ofstream mem(mem_power_state_path);
    if (mem.is_open()) {
      mem << "deep";
    }

    std::ofstream state(power_state_path);
    if (state.is_open()) {
      state << "mem";
    }
  }

  void suspend_to_disk() noexcept {
    CLAMSHELL_TRACE("execute suspend with \033[1msuspend to disk\033[22m");
    std::ofstream state(power_state_path);
    if (state.is_open()) {
      state << "disk";
    }
  }

  void write_nvidia_suspend() noexcept {
    switch (config::nvidia_method_type) {
      case config::nvidia_method::official_script:
        if (config::suspend_mode_type == config::suspend_mode::suspend_to_disk) {
          CLAMSHELL_TRACE("execute nvidia suspend with \033[1mhibernate\033[22m");
          std::system("/usr/bin/nvidia-sleep.sh hibernate");
        } else {
          CLAMSHELL_TRACE("execute nvidia suspend with \033[1msuspend\033[22m");
          std::system("/usr/bin/nvidia-sleep.sh suspend");
        }
        break;
      case config::nvidia_method::direct_proc:
        CLAMSHELL_TRACE("execute nvidia suspend with \033[1mdirect proc\033[22m");
        std::ofstream state(nvidia_suspend_path);
        if (state.is_open()) {
          state << "suspend";
        }
        break;
    }
  }

  void write_nvidia_resume() noexcept {
    switch (config::nvidia_method_type) {
      case config::nvidia_method::official_script:
        if (config::suspend_mode_type == config::suspend_mode::suspend_to_disk) {
          CLAMSHELL_TRACE("execute nvidia resume with \033[1mthaw\033[22m");
          std::system("/usr/bin/nvidia-sleep.sh thaw");
        } else {
          CLAMSHELL_TRACE("execute nvidia resume with \033[1mresume\033[22m");
          std::system("/usr/bin/nvidia-sleep.sh resume");
        }
        break;
      case config::nvidia_method::direct_proc:
        CLAMSHELL_TRACE("execute nvidia resume with \033[1mdirect proc\033[22m");
        std::ofstream state(nvidia_suspend_path);
        if (state.is_open()) {
          state << "resume";
        }
        break;
    }
  }

  bool freeze_user_processes() noexcept {
    CLAMSHELL_TRACE("freeze user processes");
    std::ofstream file(cgroup_freeze_path);
    file << "1";
    const bool ok = file.good();
    CLAMSHELL_INFO("freeze user processes: {}", ok ? "ok" : "failed");
    return ok;
  }

  bool unfreeze_user_processes() noexcept {
    CLAMSHELL_TRACE("unfreeze user processes");
    std::ofstream file(cgroup_freeze_path);
    file << "0";
    const bool ok = file.good();
    CLAMSHELL_INFO("unfreeze user processes: {}", ok ? "ok" : "failed");
    return ok;
  }

  bool move_self_to_system_slice() noexcept {
    CLAMSHELL_TRACE("move self to system slice");
    std::ofstream file(cgroup_proc_path);
    file << getpid();
    const bool ok = file.good();
    CLAMSHELL_INFO("move self to system slice: {}", ok ? "ok" : "failed");
    return ok;
  }
}

namespace clamshell {
  bool check_suspend_caps() noexcept {
    get_sleep_cap();
    CLAMSHELL_INFO(
      "sleep caps {{\n  freeze = \033[1m{}\033[22m\n  standby = \033[1m{}\033[22m\n  mem = \033[1m{}\033[22m\n  disk = \033[1m{}\033[22m\n}}",
      sleep_caps.freeze ? "true" : "false",
      sleep_caps.standby ? "true" : "false",
      sleep_caps.mem ? "true" : "false",
      sleep_caps.disk ? "true" : "false"
    );

    get_mem_sleep_cap();
    CLAMSHELL_INFO(
      "mem_sleep_caps {{\n  s2idle = \033[1m{}\033[22m\n  shallow = \033[1m{}\033[22m\n  deep = \033[1m{}\033[22m\n}}",
      mem_sleep_caps.s2idle ? "true" : "false",
      mem_sleep_caps.shallow ? "true" : "false",
      mem_sleep_caps.deep ? "true" : "false"
    );

    if (!config::fallback) {
      switch (config::suspend_mode_type) {
        case config::suspend_mode::freeze:
          use_suspend_mode = config::suspend_mode::freeze;
          return is_freeze_available();
        case config::suspend_mode::suspend_to_ram:
          use_suspend_mode = config::suspend_mode::suspend_to_ram;
          return is_suspend_to_ram_available();
        case config::suspend_mode::suspend_to_disk:
          use_suspend_mode = config::suspend_mode::suspend_to_disk;
          return is_suspend_to_disk_available();
      }
    }

    using mode = config::suspend_mode;

    static constexpr mode freeze_fallback[3] {
      mode::freeze,
      mode::suspend_to_disk,
      mode::suspend_to_ram,
    };

    static constexpr mode ram_fallback[3] {
      mode::suspend_to_ram,
      mode::suspend_to_disk,
      mode::freeze,
    };

    static constexpr mode disk_fallback[3] {
      mode::suspend_to_disk,
      mode::suspend_to_ram,
      mode::freeze,
    };

    const mode* order = nullptr;

    switch (config::suspend_mode_type) {
      case mode::freeze: order = freeze_fallback; break;
      case mode::suspend_to_ram: order = ram_fallback; break;
      case mode::suspend_to_disk: order = disk_fallback; break;
    }

    const auto is_available = [](mode m) -> bool {
      switch (m) {
        case mode::freeze: return is_freeze_available();
        case mode::suspend_to_ram: return is_suspend_to_ram_available();
        case mode::suspend_to_disk: return is_suspend_to_disk_available();
      }
      return false;
    };

    for (int i = 0; i < 3; ++i) {
      if (is_available(order[i])) {
        use_suspend_mode = order[i];
        return true;
      }
    }

    return false;
  }

  void suspend() noexcept {
    sync();
    write_nvidia_suspend();

    if (move_self_to_system_slice() && freeze_user_processes()) {
      switch (use_suspend_mode) {
        case config::suspend_mode::freeze:
          freeze();
          break;
        case config::suspend_mode::suspend_to_ram:
          suspend_to_ram();
          break;
        case config::suspend_mode::suspend_to_disk:
          suspend_to_disk();
          break;
      }
    }

    unfreeze_user_processes();
    write_nvidia_resume();
  }
}
