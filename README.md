<div align="center">
  <img src="asset/shell.png" width="82">
  <h1>CLAMSHELL</h1>
  <p>Comfortable clamshell mode for Linux</p>
</div>

<div align="center">
  <img src="https://img.shields.io/badge/LANG-C++-000000.svg?logo=&style=for-the-badge">
  <img src="https://img.shields.io/badge/OS-Linux-000000.svg?logo=&style=for-the-badge">
</div>

## What is clamshell mode?
Clamshell mode allows you to work with your laptop lid closed, using an external monitor, keyboard, and mouse.

## How to enable clamshell mode
**Required**
- At least one external monitor is connected
- The laptop lid is closed

**Optional**
- ~~The laptop is connected to AC power~~ TODO

## Comparison
macOS and systemd-based implementations require AC power as an absolute condition for clamshell mode, but clamshell does not.
It also does not depend on any init system, so it works in any environment.

## Configuration
The configuration file is in TOML format and is loaded in the following order:
`~/.config/clamshell/config.conf` → `/etc/clamshell/config.conf`

### fallback
Whether to automatically fall back to another mode if the mode specified by `suspend_mode_type` is unavailable.  
Default: `false`

### suspend_mode_type
The suspend method to use.  
Default: `freeze`
| Value | Description |
|:-|:-|
| `freeze` | Freezes the CPU. The lightest option and works in most environments. |
| `suspend_to_ram` | Uses RAM suspend (S3). |
| `suspend_to_disk` | Uses disk suspend (hibernation). |

### nvidia_method_type
The suspend method to use when an NVIDIA GPU is present.  
Default: `official_script`
| Value | Description |
|:-|:-|
| `official_script` | Uses `/usr/bin/nvidia-sleep.sh` This is the recommended option. |
| `direct_proc` | Directly uses `/proc/driver/nvidia/suspend` Not recommended due to instability. |
