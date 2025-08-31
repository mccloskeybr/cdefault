@echo off
cls

set WARN=/W4 /wd4127 /wd4244 /wd4456 /wd4355 /wd4457 /wd4245 /wd4701
set FLAGS=/std:clatest /Zo /Z7 /nologo %WARN%
set LIBS=ole32.lib user32.lib opengl32.lib gdi32.lib

echo Compiling:
REM cl %FLAGS% audio.c /Fobuild/audio.obj /Febin/audio.exe /link %LIBS%
cl %FLAGS% video.c /Fobuild/video.obj /Febin/video.exe /link %LIBS%
