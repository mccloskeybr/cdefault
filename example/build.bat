@echo off
cls

set WARN=/W4 /wd4127 /wd4244 /wd4456 /wd4355 /wd4457 /wd4245 /wd4701 /wd4201
set FLAGS=/Zo /Z7 /nologo %WARN%
set LIBS=ole32.lib user32.lib opengl32.lib gdi32.lib

echo Compiling:
cl %FLAGS% audio.c /Fobuild/audio.obj /Febin/audio.exe /link %LIBS%
REM cl %FLAGS% render_shapes.c /Fobuild/render_shapes.obj /Febin/render_shapes.exe /link %LIBS%
REM cl %FLAGS% render_image.c /Fobuild/render_image.obj /Febin/render_image.exe /link %LIBS%
REM cl %FLAGS% render_clay.c /Fobuild/render_clay.obj /Febin/render_clay.exe /link %LIBS%
REM cl %FLAGS% snake.c /Fobuild/snake.obj /Febin/snake.exe /link %LIBS%
REM cl %FLAGS% space_wars.c /Fobuild/space_wars.obj /Febin/space_wars.exe /link %LIBS%
