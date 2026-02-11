#!/usr/bin/env bash

export MANGOHUD_CONFIG="round_corners=10;position=bottom-right;proc_vram;procmem;swap;gpu_voltage" 
program="./bin/program"

if [ -x /usr/bin/mangohud ]; then
    mangohud $program
else
    echo "mangohud not found !!!"
    $program
fi 
