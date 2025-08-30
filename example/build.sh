#!/bin/bash

set -e
cd "$(dirname "$0")"
clear

gcc audio.c -o ./bin/audio -lm -lpulse -lpulse-simple

#./bin/audio
