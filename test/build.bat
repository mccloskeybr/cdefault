@echo off
cls

cl /std:clatest dll_test.c /Fobuild/dll_test.obj /Febin/dll_test.exe
cl /std:clatest sll_test.c /Fobuild/sll_test.obj /Febin/sll_test.exe
cl /std:clatest log_test.c /Fobuild/log_test.obj /Febin/log_test.exe
cl /std:clatest arena_test.c /Fobuild/arena_test.obj /Febin/arena_test.exe
cl /std:clatest string_test.c /Fobuild/string_test.obj /Febin/string_test.exe

echo "Testing:"
bin\dll_test.exe
bin\sll_test.exe
bin\log_test.exe
bin\arena_test.exe
bin\string_test.exe
