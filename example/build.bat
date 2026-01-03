@echo off
cls

set WARN=/W4 /wd4127 /wd4244 /wd4456 /wd4355 /wd4457 /wd4245 /wd4701 /wd4201
set FLAGS=/Zo /Z7 /nologo %WARN% /DPROFILE
set LIBS=ole32.lib user32.lib opengl32.lib gdi32.lib

echo Compiling:
cl %FLAGS% audio.c /Fobuild/audio.obj /Febin/audio.exe /link %LIBS%
cl %FLAGS% render_shapes.c /Fobuild/render_shapes.obj /Febin/render_shapes.exe /link %LIBS%
cl %FLAGS% render_image.c /Fobuild/render_image.obj /Febin/render_image.exe /link %LIBS%
cl %FLAGS% render_model.c /Fobuild/render_model.obj /Febin/render_model.exe /link %LIBS%
cl %FLAGS% render_font.c /Fobuild/render_font.obj /Febin/render_font.exe /link %LIBS%
cl %FLAGS% render_ui.c /Fobuild/render_ui.obj /Febin/render_ui.exe /link %LIBS%
cl %FLAGS% snake.c /Fobuild/snake.obj /Febin/snake.exe /link %LIBS%
cl %FLAGS% monstro.c /Fobuild/monstro.obj /Febin/monstro.exe /link %LIBS%
cl %FLAGS% collision_ray_test.c /Fobuild/collision_line_test.obj /Febin/collision_ray_test.exe /link %LIBS%
cl %FLAGS% collision_line_test.c /Fobuild/collision_line_test.obj /Febin/collision_line_test.exe /link %LIBS%
cl %FLAGS% collision_aabb_test.c /Fobuild/collision_aabb_test.obj /Febin/collision_aabb_test.exe /link %LIBS%
cl %FLAGS% collision_circle_test.c /Fobuild/collision_circle_test.obj /Febin/collision_circle_test.exe /link %LIBS%
cl %FLAGS% collision_convex_hull_test.c /Fobuild/collision_convex_hull_test.obj /Febin/collision_convex_hull_test.exe /link %LIBS%
cl %FLAGS% physics_2d_test.c /Fobuild/physics_2d_test.obj /Febin/physics_2d_test.exe /link %LIBS%
cl %FLAGS% physics_3d_test.c /Fobuild/physics_3d_test.obj /Febin/physics_3d_test.exe /link %LIBS%
cl %FLAGS% gjk_debug.c /Fobuild/gjk_debug.obj /Febin/gjk_debug.exe /link %LIBS%
cl %FLAGS% file.c /Fobuild/file.obj /Febin/file.exe /link %LIBS%
