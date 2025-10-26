@echo off
cls

set DEBUG=/Od /Zo /Z7
set RELEASE=/O2 /Zo- /Oi /DNDEBUG

set WARN=/W4 /wd4127 /wd4244 /wd4456 /wd4355 /wd4457 /wd4245 /wd4701 /wd4201
set FLAGS=/nologo %WARN% /DPROFILE
set LIBS=ole32.lib user32.lib opengl32.lib gdi32.lib

REM set FLAGS=%FLAGS% %DEBUG%
set FLAGS=%FLAGS% %RELEASE%

echo Compiling benchmarks:
cl %FLAGS% json_benchmark.c /Fobuild/json_benchmark.obj /Febin/json_benchmark.exe /link %LIBS%

echo Running benchmarks:
bin\json_benchmark.exe
