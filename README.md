# cdefault

This is a collection of stb-like libraries for the C programming language.
Developed for personal use with a primary focus on game development, but
files are sufficiently modularized to plug and play for a variety of usecases.

*   `cdefault_3dgeo.h`   - Structs and functions for 3d geometry, including some collision detection. TODO: expand to other shapes, implement GJK.
*   `cdefault_audio.h`   - Unified API for playing audio on operating systems. WASAPI and PulseAudio are supported right now. TODO: Expand to CoreAudio, consider supporting e.g. Jack, PipeWire.
*   `cdefault_math.h`    - Math library, particularly 2/3/4D vectors and 3x3 / 4x4 Matrices for graphics.
*   `cdefault_profile.h` - Simple profiler. TODO: Support a profile visualizer (as a separate module?).
*   `cdefault_std.h`     - Platform agnostic support for e.g. memory allocation (Arena), concurrency datastructures, strings. TODO: remove dependency on stdlib.
*   `cdefault_video.h`   - Unified API (on top of OpenGL) for basic rendering. TODO: Support Linux / OSX, expand core functionality, support basic 3D use cases.

TODO:

*   Platform-agnostic input library.
*   Networking library.
*   Port basic physics library.
