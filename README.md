# cdefault

This is a collection of stdlib replacement modules for the C programming language (C99).
Designed with minimal external dependencies (although some modules depend on others).
Developed for personal use with a primary focus on game development / prototyping, but
libraries are sufficiently modularized to plug and play for a variety of usecases.

NOTE: Windows is explicitly supported. Linux is partially supported. I'll get around to Mac eventually.

*  `cdefault_audio.h`      - Unified API for playing audio on operating systems.
*  `cdefault_geometry.h`   - Structs and functions for 2/3D geometry, with a focus on collision detection.
*  `cdefault_image.h`      - Image file importer.
*  `cdefault_io.h`         - Filesytem API.
*  `cdefault_math.h`       - Math library, particularly 2/3/4D vectors and 2x2/3x3/4x4 matrices for graphics.
*  `cdefault_mesh.h`       - Mesh file importer.
*  `cdefault_physics_2d.h` - Simple and extendible 2D physics API with a basic rigid body resolver.
*  `cdefault_profile.h`    - Simple profiler.
*  `cdefault_render.h`     - Unified API for basic 2/3D rendering, focusing on prototypes.
*  `cdefault_sound.h`      - Audio file importer.
*  `cdefault_std.h`        - Platform agnostic support for e.g. memory allocation (Arena), concurrency, strings, etc.
*  `cdefault_test.h`       - Basic unit test framework.

See `example/` for example integrations. Unit and visual / component tests are under `test/`.

TODO:

*  Complete independence from stdlib.
*  Networking library.
*  Platform-agnostic input (controllers) library.
*  Font library.

Credits:

Some modules have been influenced by the following:

*  RayLib
*  SDL3
*  RAD debugger
*  STB libraries
