# cdefault

This is a collection of stdlib replacement modules for the C programming language (C99).
Designed with minimal external dependencies (although some modules depend on others).
Developed for personal use with a primary focus on game development / prototyping, but
libraries are sufficiently modularized to plug and play for a variety of usecases.

NOTE: Windows is explicitly supported. Linux is partially supported. I'll get around to Mac eventually.

*  `cdefault_audio.h`      - Unified API for playing audio on operating systems.
*  `cdefault_font.h`       - TTF file parser & font rasterizer (supports generating single-channel bitmap and SDF atlases).
*  `cdefault_geometry.h`   - Structs and functions for 2/3D geometry, with a focus on collision detection.
*  `cdefault_image.h`      - Image file importer (.png, .bmp).
*  `cdefault_io.h`         - Filesytem and logging API.
*  `cdefault_json.h`       - JSON serializer / deserializer.
*  `cdefault_math.h`       - Math library, particularly 2/3/4D vectors and 2x2/3x3/4x4 matrices for graphics.
*  `cdefault_model.h`      - Model file importer (.glb, .obj).
*  `cdefault_physics_2d.h` - Simple and extendible 2D physics API with a basic rigid body resolver (translation + rotation).
*  `cdefault_physics_3d.h` - Simple and extendible 3D physics API with a basic rigid body resolver (translation only).
*  `cdefault_profile.h`    - Simple profiler.
*  `cdefault_render.h`     - Unified API for basic 2/3D rendering, focusing on prototypes.
*  `cdefault_sound.h`      - Audio file importer (.wav).
*  `cdefault_std.h`        - Platform agnostic support for e.g. memory allocation (Arena), concurrency, strings (incl. formatting), etc.
*  `cdefault_test.h`       - Basic unit test framework.
*  `cdefault_ui.h`         - Immediate mode UI library (mainly for debug views, that kind of thing).

See `example/` for example integrations. Unit and visual / component tests are under `test/`.

TODO:

*  Complete independence from stdlib.
*  Networking library.
*  Platform-agnostic input (controllers) library.

Credits:

Some modules have been influenced by the following:

*  RayLib
*  SDL3
*  RAD debugger
*  STB libraries
*  Dear ImGui
*  Clay
