@echo off
cls

set compiler_warnings=/WX /W4 /WL /wd4201 /wd4100 /wd4189 /wd4505 /wd4127 /wd4115 /wd4456 /wd4457 /wd4244 /wd4245 /wd4701 /wd4310 /wd4702
set compiler_defs=/DWIN32 /D_CRT_SECURE_NO_WARNINGS /Od /Zo /Z7 /DDEBUG

set compiler_flags=%compiler_defs% %compiler_warnings% /nologo /diagnostics:column /fp:fast /fp:except- /GR- /EHa- /FC /GS- /IZ:\chttp\src\
set linker_flags=/STACK:0x100000,0x100000 /incremental:no /opt:ref
set linker_libs=user32.lib kernel32.lib ws2_32.lib

pushd Z:\cdefault\test
IF NOT EXIST .\build mkdir .\build
IF NOT EXIST .\bin mkdir .\bin

cl %compiler_flags% main.c /Fobuild\ /Fmbuild\ /Fdbuild\ /Febin\^
 /link %linker_flags% %linker_libs% /SUBSYSTEM:console

popd
