#!/usr/bin/env bash

# Install all the stupid windows dependencies locally
# But you have to install x86_64-w64-mingw32-gcc-g++ first,
# and also probably install all the openGL stuff
#
# Best options is probably to just not bother doing this


url="https://armandosbasement.xyz/windows.zip"

if [ -z "$XDG_RUNTIME_DIR" ]; then
    tmp_dir="/tmp/windows"
else 
    tmp_dir="$XDG_RUNTIME_DIR/windows"
fi

tmp_file="$tmp_dir/windows.zip"

mkdir -p "$tmp_dir"
wget -O "$tmp_file" "$url"
unzip -q "$tmp_file" -d .
