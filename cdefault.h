#ifndef CDEFAULT_H_
#define CDEFAULT_H_

// NOTE: this just provides a convenient shorthand to #include all cdefault modules.

#include "cdefault_audio.h"
#include "cdefault_font.h"
#include "cdefault_geometry.h"
#include "cdefault_image.h"
#include "cdefault_io.h"
#include "cdefault_json.h"
#include "cdefault_math.h"
#include "cdefault_mesh.h"
#include "cdefault_physics_2d.h"
#include "cdefault_profile.h"
#include "cdefault_render.h"
#include "cdefault_sound.h"
#include "cdefault_std.h"
#include "cdefault_test.h"

#endif // CDEFAULT_H_

#ifdef CDEFAULT_IMPLEMENTATION
#undef CDEFAULT_IMPLEMENTATION

#define CDEFAULT_AUDIO_IMPLEMENTATION
#include "cdefault_audio.h"
#define CDEFAULT_FONT_IMPLEMENTATION
#include "cdefault_font.h"
#define CDEFAULT_GEOMETRY_IMPLEMENTATION
#include "cdefault_geometry.h"
#define CDEFAULT_IMAGE_IMPLEMENTATION
#include "cdefault_image.h"
#define CDEFAULT_IO_IMPLEMENTATION
#include "cdefault_io.h"
#define CDEFAULT_JSON_IMPLEMENTATION
#include "cdefault_json.h"
#define CDEFAULT_MATH_IMPLEMENTATION
#include "cdefault_math.h"
#define CDEFAULT_MESH_IMPLEMENTATION
#include "cdefault_mesh.h"
#define CDEFAULT_PHYSICS2_IMPLEMENTATION
#include "cdefault_physics_2d.h"
#define CDEFAULT_PROFILE_IMPLEMENTATION
#include "cdefault_profile.h"
#define CDEFAULT_RENDER_IMPLEMENTATION
#include "cdefault_render.h"
#define CDEFAULT_SOUND_IMPLEMENTATION
#include "cdefault_sound.h"
#define CDEFAULT_STD_IMPLEMENTATION
#include "cdefault_std.h"
#define CDEFAULT_TEST_IMPLEMENTATION
#include "cdefault_test.h"

#endif // CDEFAULT_IMPLEMENTATION
