@echo off
cls

set FLAGS=/std:clatest /W4 /wd4127 /Zo /Z7 /nologo
set LIBS=ole32.lib

echo Compiling:
REM cl %FLAGS% dll_test.c /Fobuild/dll_test.obj /Febin/dll_test.exe /link %LIBS%
REM cl %FLAGS% sll_test.c /Fobuild/sll_test.obj /Febin/sll_test.exe /link %LIBS%
REM cl %FLAGS% log_test.c /Fobuild/log_test.obj /Febin/log_test.exe /link %LIBS%
REM cl %FLAGS% arena_test.c /Fobuild/arena_test.obj /Febin/arena_test.exe /link %LIBS%
REM cl %FLAGS% string_test.c /Fobuild/string_test.obj /Febin/string_test.exe /link %LIBS%
REM cl %FLAGS% vector_test.c /Fobuild/vector_test.obj /Febin/vector_test.exe /link %LIBS%
cl %FLAGS% audio_test.c /Fobuild/audio_test.obj /Febin/audio_test.exe /link %LIBS%

echo Testing:
REM bin\dll_test.exe
REM bin\sll_test.exe
REM bin\log_test.exe
REM bin\arena_test.exe
REM bin\string_test.exe
REM bin\vector_test.exe
bin\audio_test.exe
