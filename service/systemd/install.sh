#!/bin/bash

if ! xmake; then
  exit 1
fi

if ! sudo xmake install; then
  exit 1
fi

sudo mkdir -p /etc/systemd/logind.conf.d/
echo '[Login]
HandleLidSwitch=ignore' | sudo tee /etc/systemd/logind.conf.d/clamshell.conf
sudo cp service/systemd/clamshell.service /usr/lib/systemd/system/
