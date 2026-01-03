#!/bin/bash

set -e
cd "$(dirname "$0")"
clear

# gcc audio.c -o ./bin/audio -lm -lpulse -lpulse-simple
# gcc file.c -o ./bin/file
gcc log.c -o ./bin/log

#./bin/audio
./bin/log
