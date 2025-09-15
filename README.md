# cdefault

This is a collection of stb-like libraries for the C programming language (C99).
Developed for personal use with a primary focus on game development / prototyping, but
libraries are sufficiently modularized to plug and play for a variety of usecases.

NOTE: Windows is explicitly supported. Linux is partially supported. I'll get around to Mac eventually.

*  `cdefault_audio.h`    - Unified API for playing audio on operating systems.
*  `cdefault_geometry.h` - Structs and functions for 2/3D geometry, including some collision detection.
*  `cdefault_io.h`       - Filesytem API.
*  `cdefault_math.h`     - Math library, particularly 2/3/4D vectors and 3x3 / 4x4 Matrices for graphics.
*  `cdefault_profile.h`  - Simple profiler.
*  `cdefault_render.h`   - Unified API for basic rendering.
*  `cdefault_std.h`      - Platform agnostic support for e.g. memory allocation (Arena), concurrency datastructures, strings, etc.

See `example/` for example integrations. Unit and visual / component tests are under `test/`.

TODO:

*  Port basic physics library.
*  Networking library.
*  Platform-agnostic input (controllers) library.
*  Font library.
*  Library for various fileformats, e.g. image / audio (instead of relying on stb).
*  Refactor to be C89 compatible.

Credits:

Some modules have been influenced by the following:

*  RayLib
*  SDL3
*  RAD debugger
