#!/bin/bash

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

cp "$SCRIPT_DIR"/*.service "$SCRIPT_DIR"/*.timer /etc/systemd/system/

systemctl daemon-reload

systemctl enable --now glennergy-inputcache.service
systemctl enable --now glennergy-meteo.timer
systemctl enable --now glennergy-spotpris.timer

echo "Services are a GOOOOOOO."
