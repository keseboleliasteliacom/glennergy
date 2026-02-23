#!/bin/sh
set -e

LOG_DIR="/var/log/glennergy"

echo "Ensuring log directory exists: $LOG_DIR"

if [ ! -d "$LOG_DIR" ]; then
    echo "Creating $LOG_DIR"
    sudo mkdir -p "$LOG_DIR"
fi

# Set secure default permissions
sudo chmod 777 "$LOG_DIR"
sudo chown root:root "$LOG_DIR"

echo "Log directory ready."


if [ -f Makefile ]; then
    echo "Building Server"
    make
    sudo make install
fi

crontab -l 2>/dev/null | grep -v "# Glennergy" | {
    cat
    echo "* * * * * /usr/local/bin/Glennergy-Spotpris # Glennergy-Spotpris cron"
    echo "* * * * * /usr/local/bin/Glennergy-Meteo # Glennergy-Meteo cron"
} | crontab -

find . -type f -name Makefile | while read mf; do
    dir=$(dirname "$mf")
    echo "Processing $dir"
    make -C "$dir"
    sudo make -C "$dir" install
done