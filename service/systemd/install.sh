#!/bin/bash

if ! xmake; then
  exit 1
fi

ARCH=$(uname -m)
sudo cp build/linux/${ARCH}/release/clamshell /usr/bin/
sudo cp service/systemd/clamshell.service /usr/lib/systemd/system/
sudo mkdir -p /etc/systemd/logind.conf.d/
echo '[Login]
HandleLidSwitch=ignore' | sudo tee /etc/systemd/logind.conf.d/clamshell.conf
