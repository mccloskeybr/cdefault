#!/bin/bash

set -e
cd "$(dirname "$0")"

mkdir -p ./bin/

echo "Compiling..."
# gcc dll_test.c -o ./bin/dll_test
# gcc sll_test.c -o ./bin/sll_test
# gcc log_test.c -o ./bin/log_test
# gcc arena_test.c -o ./bin/arena_test
# gcc string_test.c -o ./bin/string_test
# gcc vector_test.c -o ./bin/vector_test -lm
# gcc matrix_test.c -o ./bin/matrix_test -lm
# gcc time_test.c -o ./bin/time_test -lm
# gcc sort_test.c -o ./bin/sort_test -lm
gcc io_test.c -o ./bin/io_test -lm

echo "Testing:"
# ./bin/dll_test
# ./bin/sll_test
# ./bin/log_test
# ./bin/arena_test
# ./bin/string_test
# ./bin/vector_test
# ./bin/matrix_test
# ./bin/time_test
# ./bin/sort_test
./bin/io_test
