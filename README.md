# cdefault

This is a collection of stb-like libraries for the C programming language.
Developed for personal use with a primary focus on game development, but
files are sufficiently modularized to plug and play for a variety of usecases.

*   `cdefault_audio.h`    - Unified API for playing audio on operating systems.
*   `cdefault_geometry.h` - Structs and functions for 3d geometry, including some collision detection.
*   `cdefault_math.h`     - Math library, particularly 2/3/4D vectors and 3x3 / 4x4 Matrices for graphics.
*   `cdefault_profile.h`  - Simple profiler.
*   `cdefault_std.h`      - Platform agnostic support for e.g. memory allocation (Arena), concurrency datastructures, strings.
*   `cdefault_video.h`    - Unified API for basic rendering.

TODO:

*   Platform-agnostic input library.
*   Networking library.
*   Port basic physics library.
