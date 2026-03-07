#!/bin/bash

if ! xmake f -m release || ! xmake; then
  exit 1
fi

ARCH=$(uname -m)
sudo cp build/linux/${ARCH}/release/clamshell /usr/bin/
sudo cp service/openrc/clamshell.in /etc/init.d/clamshell
sudo chmod +x /etc/init.d/clamshell
