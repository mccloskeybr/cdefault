@echo off
cls

set FLAGS=/W4 /wd4127 /wd4201 /Zo /Z7 /nologo
set LIBS=ole32.lib

echo Compiling:
cl %FLAGS% dll_test.c /Fobuild/dll_test.obj /Febin/dll_test.exe /link %LIBS%
cl %FLAGS% sll_test.c /Fobuild/sll_test.obj /Febin/sll_test.exe /link %LIBS%
cl %FLAGS% log_test.c /Fobuild/log_test.obj /Febin/log_test.exe /link %LIBS%
cl %FLAGS% arena_test.c /Fobuild/arena_test.obj /Febin/arena_test.exe /link %LIBS%
cl %FLAGS% string_test.c /Fobuild/string_test.obj /Febin/string_test.exe /link %LIBS%
cl %FLAGS% vector_test.c /Fobuild/vector_test.obj /Febin/vector_test.exe /link %LIBS%

echo Testing:
bin\dll_test.exe
bin\sll_test.exe
bin\log_test.exe
bin\arena_test.exe
bin\string_test.exe
bin\vector_test.exe
