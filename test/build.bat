@echo off
cls

set WARN=/W4 /wd4127 /wd4244 /wd4456 /wd4355 /wd4457 /wd4245 /wd4701 /wd4201
set FLAGS=/Zo /Z7 /nologo %WARN%
set LIBS=ole32.lib user32.lib opengl32.lib gdi32.lib

REM goto skip_unit_tests

echo Compiling unit tests:
REM cl %FLAGS% dll_test.c /Fobuild/dll_test.obj /Febin/dll_test.exe /link %LIBS%
REM cl %FLAGS% sll_test.c /Fobuild/sll_test.obj /Febin/sll_test.exe /link %LIBS%
REM cl %FLAGS% log_test.c /Fobuild/log_test.obj /Febin/log_test.exe /link %LIBS%
REM cl %FLAGS% arena_test.c /Fobuild/arena_test.obj /Febin/arena_test.exe /link %LIBS%
REM cl %FLAGS% string_test.c /Fobuild/string_test.obj /Febin/string_test.exe /link %LIBS%
REM cl %FLAGS% vector_test.c /Fobuild/vector_test.obj /Febin/vector_test.exe /link %LIBS%
REM cl %FLAGS% matrix_test.c /Fobuild/matrix_test.obj /Febin/matrix_test.exe /link %LIBS%
REM cl %FLAGS% time_test.c /Fobuild/time_test.obj /Febin/time_test.exe /link %LIBS%
REM cl %FLAGS% sort_test.c /Fobuild/sort_test.obj /Febin/sort_test.exe /link %LIBS%
REM cl %FLAGS% bin_head_test.c /Fobuild/bin_head_test.obj /Febin/bin_head_test.exe /link %LIBS%
cl %FLAGS% dynamic_array_test.c /Fobuild/dynamic_array_test.obj /Febin/dynamic_array_test.exe /link %LIBS%

echo Running unit tests:
REM bin\dll_test.exe
REM bin\sll_test.exe
REM bin\log_test.exe
REM bin\arena_test.exe
REM bin\string_test.exe
REM bin\vector_test.exe
REM bin\matrix_test.exe
REM bin\time_test.exe
REM bin\sort_test.exe
REM bin\bin_head_test.exe
bin\dynamic_array_test.exe

:skip_unit_tests

goto skip_visual_tests

cl %FLAGS% collision_ray_test.c /Fobuild/collision_line_test.obj /Febin/collision_ray_test.exe /link %LIBS%
REM cl %FLAGS% collision_line_test.c /Fobuild/collision_line_test.obj /Febin/collision_line_test.exe /link %LIBS%
REM cl %FLAGS% collision_aabb_test.c /Fobuild/collision_aabb_test.obj /Febin/collision_aabb_test.exe /link %LIBS%
REM cl %FLAGS% collision_circle_test.c /Fobuild/collision_circle_test.obj /Febin/collision_circle_test.exe /link %LIBS%
REM cl %FLAGS% collision_convex_hull_test.c /Fobuild/collision_convex_hull_test.obj /Febin/collision_convex_hull_test.exe /link %LIBS%

:skip_visual_tests
