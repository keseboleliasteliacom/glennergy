#!/bin/sh
set -e


if [ -f Makefile ]; then
    echo "Uninstalling"
    sudo make uninstall
fi

crontab -l 2>/dev/null | grep -v "# Glennergy" | crontab -

find . -type f -name Makefile | while read mf; do
    dir=$(dirname "$mf")
    echo "Processing $dir"
    sudo make -C "$dir" uninstall

done