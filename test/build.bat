@echo off
cls

set WARN=/W4 /wd4127 /wd4244 /wd4456 /wd4355 /wd4457 /wd4245 /wd4701 /wd4201
set FLAGS=/Zo /Z7 /nologo %WARN%
set LIBS=ole32.lib user32.lib opengl32.lib gdi32.lib

REM cl %FLAGS% dll_test.c /Fobuild/dll_test.obj /Febin/dll_test.exe /link %LIBS% && bin\dll_test.exe
REM cl %FLAGS% sll_test.c /Fobuild/sll_test.obj /Febin/sll_test.exe /link %LIBS% && bin\sll_test.exe
REM cl %FLAGS% log_test.c /Fobuild/log_test.obj /Febin/log_test.exe /link %LIBS% && bin\log_test.exe
REM cl %FLAGS% arena_test.c /Fobuild/arena_test.obj /Febin/arena_test.exe /link %LIBS% && bin\arena_test.exe
REM cl %FLAGS% string_test.c /Fobuild/string_test.obj /Febin/string_test.exe /link %LIBS% && bin\string_test.exe
REM cl %FLAGS% vector_test.c /Fobuild/vector_test.obj /Febin/vector_test.exe /link %LIBS% && bin\vector_test.exe
REM cl %FLAGS% matrix_test.c /Fobuild/matrix_test.obj /Febin/matrix_test.exe /link %LIBS% && bin\matrix_test.exe
REM cl %FLAGS% time_test.c /Fobuild/time_test.obj /Febin/time_test.exe /link %LIBS% && bin\time_test.exe
REM cl %FLAGS% sort_test.c /Fobuild/sort_test.obj /Febin/sort_test.exe /link %LIBS% && bin\sort_test.exe
REM cl %FLAGS% bin_stream_test.c /Fobuild/bin_stream_test.obj /Febin/bin_stream_test.exe /link %LIBS% && bin\bin_stream_test.exe
REM cl %FLAGS% dynamic_array_test.c /Fobuild/dynamic_array_test.obj /Febin/dynamic_array_test.exe /link %LIBS% && bin\dynamic_array_test.exe
REM cl %FLAGS% json_test.c /Fobuild/json_test.obj /Febin/json_test.exe /link %LIBS% && bin\json_test.exe
cl %FLAGS% geometry_test.c /Fobuild/geometry_test.obj /Febin/geometry_test.exe /link %LIBS% && bin\geometry_test.exe
