@echo off
cls

REM cl /std:clatest /W4 dll_test.c /Fobuild/dll_test.obj /Febin/dll_test.exe
REM cl /std:clatest /W4 sll_test.c /Fobuild/sll_test.obj /Febin/sll_test.exe
REM cl /std:clatest /W4 log_test.c /Fobuild/log_test.obj /Febin/log_test.exe
REM cl /std:clatest /W4 arena_test.c /Fobuild/arena_test.obj /Febin/arena_test.exe
REM cl /std:clatest /W4 string_test.c /Fobuild/string_test.obj /Febin/string_test.exe
REM cl /std:clatest /W4 vector_test.c /Fobuild/vector_test.obj /Febin/vector_test.exe
cl /std:clatest /experimental:c11atomics /W4 /Zo /Z7 audio_test.c /Fobuild/audio_test.obj /Febin/audio_test.exe /link ole32.lib

echo Testing:
REM bin\dll_test.exe
REM bin\sll_test.exe
REM bin\log_test.exe
REM bin\arena_test.exe
REM bin\string_test.exe
REM bin\vector_test.exe
bin\audio_test.exe
