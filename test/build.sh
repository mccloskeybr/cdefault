#! /bin/bash

set -e

mkdir -p ./bin/

gcc dll_test.c -o ./bin/dll_test
gcc log_test.c -o ./bin/log_test

if [[ "$*" == *"--run"* ]]; then
  echo "Testing:"
  ./bin/dll_test
  ./bin/log_test
fi
