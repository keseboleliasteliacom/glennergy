#!/bin/sh
set -e


if [ -f Makefile ]; then
    echo "Building Server"
    make
    sudo make install
fi

find . -type f -name Makefile | while read mf; do
    dir=$(dirname "$mf")
    echo "Processing $dir"
    make -C "$dir"
    sudo make -C "$dir" install
done