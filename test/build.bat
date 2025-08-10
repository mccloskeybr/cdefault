@echo off
cls

REM cl /std:clatest dll_test.c /Fobuild/dll_test.obj /Febin/dll_test.exe
REM cl /std:clatest sll_test.c /Fobuild/sll_test.obj /Febin/sll_test.exe
REM cl /std:clatest log_test.c /Fobuild/log_test.obj /Febin/log_test.exe
REM cl /std:clatest arena_test.c /Fobuild/arena_test.obj /Febin/arena_test.exe
REM cl /std:clatest string_test.c /Fobuild/string_test.obj /Febin/string_test.exe
cl /std:clatest vector_test.c /Fobuild/vector_test.obj /Febin/vector_test.exe

echo Testing:
REM bin\dll_test.exe
REM bin\sll_test.exe
REM bin\log_test.exe
REM bin\arena_test.exe
REM bin\string_test.exe
bin\vector_test.exe
