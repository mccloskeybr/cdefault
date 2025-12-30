#!/bin/bash

set -e
cd "$(dirname "$0")"
clear

# gcc audio.c -o ./bin/audio -lm -lpulse -lpulse-simple
gcc file.c -o ./bin/file

#./bin/audio
