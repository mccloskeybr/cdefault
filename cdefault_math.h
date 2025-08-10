#ifndef CDEFAULT_MATH_H_
#define CDEFAULT_MATH_H_

#include <stdlib.h>
#include <math.h>
#include "cdefault_std.h"

///////////////////////////////////////////////////////////////////////////////
// NOTE: Num
///////////////////////////////////////////////////////////////////////////////

#define U32_MAX 4294967295
#define F32_MIN 1.175494351e-38F
#define F32_MAX 3.402823466e+38F
#define F32_PI 3.14159265358979323846264338327950288419716939937510582097494459230781640628620899f

B32 F32ApproxEq(F32 x, F32 y);
F32 F32Abs(F32 x);
F32 F32Pow(F32 x, F32 y);
F32 F32Sqrt(F32 x);
F32 F32Sin(F32 x);
F32 F32Cos(F32 x);
F32 F32Tan(F32 x);
F32 F32ArcSin(F32 x);
F32 F32ArcCos(F32 x);
F32 F32ArcTan(F32 x);
F32 F32ArcTan2(F32 y, F32 x);

///////////////////////////////////////////////////////////////////////////////
// NOTE: Random
///////////////////////////////////////////////////////////////////////////////

typedef struct RandomSeries RandomSeries;
struct RandomSeries { U32 state; };

void RandInit(RandomSeries* rand, U32 seed);
S32 RandS32(RandomSeries* rand, S32 min, S32 max);
F32 RandF32(RandomSeries* rand, F32 min, F32 max);

///////////////////////////////////////////////////////////////////////////////
// NOTE: V2
///////////////////////////////////////////////////////////////////////////////

#define V2_X_POS (V2) {+1,  0}
#define V2_Y_POS (V2) { 0, +1}
#define V2_X_NEG (V2) {-1,  0}
#define V2_Y_NEG (V2) { 0, -1}

typedef union V2 V2;
union V2 {
  struct { F32 x, y; };
  struct { F32 u, v; };
  F32 e[2];
};

void V2AddV2(const V2* x, const V2* y, V2* out);
void V2AddF32(const V2* x, F32 c, V2* out);
void V2SubV2(const V2* x, const V2* y, V2* out);
void V2SubF32(const V2* x, F32 c, V2* out);
void V2MultF32(const V2* x, F32 c, V2* out);
void V2DivF32(const V2* x, F32 c, V2* out);
B32 V2ApproxEq(const V2* x, const V2* y);
void V2HadamardV2(const V2* x, const V2* y, V2* out);
F32 V2DotV2(const V2* x, const V2* y);
F32 V2InnerMultV2(const V2* x, const V2* y);
F32 V2LengthSq(const V2* x);
F32 V2Length(const V2* x);
void V2Normalize(const V2* x, V2* out);

///////////////////////////////////////////////////////////////////////////////
// NOTE: V3
///////////////////////////////////////////////////////////////////////////////

#define V3_X_POS (V3) {+1,  0,  0}
#define V3_Y_POS (V3) { 0, +1,  0}
#define V3_Z_POS (V3) { 0,  0, +1}
#define V3_X_NEG (V3) {-1,  0,  0}
#define V3_Y_NEG (V3) { 0, -1,  0}
#define V3_Z_NEG (V3) { 0,  0, -1}

#define V3_NORTH (V3) { 0, +1, 0}
#define V3_EAST  (V3) {+1,  0, 0}
#define V3_SOUTH (V3) { 0, -1, 0}
#define V3_WEST  (V3) {-1,  0, 0}

#define V3_BLACK (V3) {0, 0, 0}
#define V3_WHITE (V3) {1, 1, 1}
#define V3_RED   (V3) {1, 0, 0}
#define V3_GREEN (V3) {0, 1, 0}
#define V3_BLUE  (V3) {0, 0, 1}

typedef union V3 V3;
union V3 {
  struct { F32 x, y, z; };
  struct { F32 r, g, b; };
  F32 e[3];
};

void V3AddV3(const V3* x, const V3* y, V3* out);
void V3AddF32(const V3* x, F32 c, V3* out);
void V3SubV3(const V3* x, const V3* y, V3* out);
void V3SubF32(const V3* x, F32 c, V3* out);
void V3MultF32(const V3* x, F32 c, V3* out);
void V3DivF32(const V3* x, F32 c, V3* out);
B32 V3ApproxEq(const V3* x, const V3* y);
B32 V3Equal(const V3* x, const V3* y);
F32 V3LengthSq(const V3* x);
F32 V3Length(const V3* x);
void V3HadamardV3(const V3* x, const V3* y, V3* out);
F32 V3InnerMultV3(const V3* x, const V3* y);
F32 V3DotV3(const V3* x, const V3* y);
void V3CrossV3(const V3* x, const V3* y, V3* out);
void V3Normalize(const V3* x, V3* out);
F32 V3MinValue(const V3* x);
F32 V3MaxValue(const V3* x);
B32 V3IsBetween(const V3* x, const V3* start, const V3* end);
void V3Project(const V3* x, const V3* dest, V3* out);
void V3RotateX(const V3* x, F32 angle_rad, V3* out);
void V3RotateY(const V3* x, F32 angle_rad, V3* out);
void V3RotateZ(const V3* x, F32 angle_rad, V3* out);
void V3Clamp(const V3* x, F32 min, F32 max, V3* out);
B32 V3AreCollinear(const V3* x, const V3* y, const V3* z);
V3 V3Splat(F32 c);
F32 V3AngleBetween(const V3* x, const V3* y);
F32 V3AngleBetween360(const V3* a, const V3* b, const V3* normal);
void V3Lerp(const V3* x, const V3* y, F32 t, V3* out);
void V3RotateAroundAxis(const V3* x, const V3* axis, F32 theta, V3* out);

///////////////////////////////////////////////////////////////////////////////
// NOTE: V4
///////////////////////////////////////////////////////////////////////////////

#define V4_QUAT_IDENT (V4) {0, 0, 0, 1}
#define V4_X_POS (V4) {+1,  0,  0,  0}
#define V4_Y_POS (V4) { 0, +1,  0,  0}
#define V4_Z_POS (V4) { 0,  0, +1,  0}
#define V4_W_POS (V4) { 0,  0,  0, +1}
#define V4_X_NEG (V4) {-1,  0,  0,  0}
#define V4_Y_NEG (V4) { 0, -1,  0,  0}
#define V4_Z_NEG (V4) { 0,  0, -1,  0}
#define V4_W_NEG (V4) { 0,  0,  0, -1}

#define V4_TRANS (V4) {0, 0, 0, 0}
#define V4_BLACK (V4) {0, 0, 0, 1}
#define V4_WHITE (V4) {1, 1, 1, 1}
#define V4_RED   (V4) {1, 0, 0, 1}
#define V4_GREEN (V4) {0, 1, 0, 1}
#define V4_BLUE  (V4) {0, 0, 1, 1}

typedef union V4 V4;
union V4 {
  struct { F32 x, y, z, w; };
  struct { F32 r, g, b, a; };
  F32 e[4];
};

void V4AddV4(const V4* x, const V4* y, V4* out);
void V4AddF32(const V4* x, F32 c, V4* out);
void V4SubV4(const V4* x, const V4* y, V4* out);
void V4SubF32(const V4* x, F32 c, V4* out);
void V4MultF32(const V4* x, F32 c, V4* out);
void V4DivF32(const V4* x, F32 c, V4* out);
B32 V4ApproxEq(const V4* a, const V4* b);
F32 V4LengthSq(const V4* x);
F32 V4Length(const V4* x);
void V4HadamardV4(const V4* x, const V4* y, V4* out);
F32 V4InnerMultV4(const V4* x, const V4* y);
F32 V4DotV4(const V4* x, const V4* y);
void V4Normalize(const V4* x, V4* out);
void V4Lerp(const V4* a, const V4* b, F32 t, V4* out);
void V4Slerp(const V4* a, const V4* b, F32 t, V4* out);
void V4RotateAroundAxis(const V3* axis, F32 theta, V4* out);
void V4LookInDir(const V3* dir, V4* out);
void V4LookAt(const V3* pos, const V3* target, V4* out);

///////////////////////////////////////////////////////////////////////////////
// NOTE: M3
///////////////////////////////////////////////////////////////////////////////

#define M3_IDENTITY (M3) {1, 0, 0 \
                          0, 1, 0 \
                          0, 0, 1 }

typedef union M3 M3;
union M3 {
  F32 e[3][3];
  F32 i[9];
};

void M3MultM3(const M3* x, const M3* y, M3* out);
void M3MultV3(const M3* x, const V3* y, V3* out);
void M3Transpose(const M3* x, M3* out);

///////////////////////////////////////////////////////////////////////////////
// NOTE: M4
///////////////////////////////////////////////////////////////////////////////

#define M4_IDENTITY (M3) {1, 0, 0, 0 \
                          0, 1, 0, 0 \
                          0, 0, 1, 0 \
                          0, 0, 0, 1}

typedef union M4 M4;
union M4 {
  F32 e[4][4];
  F32 i[16];
};

void M4MultM4(const M4* x, const M4* y, M4* out);
void M4MultV4(const M4* x, const V4* y, V4* out);
void M4Transpose(const M4* x, M4* out);
void M4Invert(const M4* x, M4* out);
void M4Perspective(F32 fov_y_rad, F32 aspect_ratio, F32 near_plane, F32 far_plane, M4* out);
void M4Orthographic(F32 left, F32 right, F32 bottom, F32 top, F32 near_plane, F32 far_plane, M4* out);
void LookAt(const V3* eye, const V3* target, const V3* up, M4* out);

#endif // CDEFAULT_MATH_H_

#ifdef CDEFAULT_MATH_IMPLEMENTATION
#undef CDEFAULT_MATH_IMPLEMENTATION

///////////////////////////////////////////////////////////////////////////////
// NOTE: Num implementation
///////////////////////////////////////////////////////////////////////////////

B32 F32ApproxEq(F32 a, F32 b) { return F32Abs(a - b) < 0.00001f; }
F32 F32Abs(F32 x) { return abs(x); }
F32 F32Pow(F32 x, F32 y) { return powf(x, y); }
F32 F32Sqrt(F32 x) { return sqrtf(x); }
F32 F32Sin(F32 x) { return sinf(x); }
F32 F32Cos(F32 x) { return cosf(x); }
F32 F32Tan(F32 x) { return tanf(x); }
F32 F32ArcSin(F32 x) { return asinf(x); }
F32 F32ArcCos(F32 x) { return acosf(x); }
F32 F32ArcTan(F32 x) { return atanf(x); }
F32 F32ArcTan2(F32 y, F32 x) { return atan2f(y, x); }
F32 F32Lerp(F32 x, F32 y, F32 t) { return (x * (1 - t)) + (y * t); }

///////////////////////////////////////////////////////////////////////////////
// NOTE: Random implementation
///////////////////////////////////////////////////////////////////////////////

static U32 XOrShift32(U32 x) {
  x ^= x << 13;
  x ^= x >> 17;
  x ^= x << 5;
  return x;
}

void RandInit(RandomSeries* rand, U32 seed) {
  MEMORY_ZERO_STRUCT(rand);
  rand->state = seed;
}

S32 RandS32(RandomSeries* rand, S32 min, S32 max) {
  ASSERT(min < max);
  rand->state = XOrShift32(rand->state);
  int32_t r = *(int32_t*)&rand->state;
  r %= max - min;
  r += min;
  return r;
}

F32 RandF32(RandomSeries* rand, F32 min, F32 max) {
  ASSERT(min < max);
  rand->state = XOrShift32(rand->state);
  F32 r = (F32)rand->state / (F32)U32_MAX;
  r *= (max - min);
  r += min;
  return r;
}

///////////////////////////////////////////////////////////////////////////////
// NOTE: V2 implementation
///////////////////////////////////////////////////////////////////////////////

void V2AddV2(const V2* x, const V2* y, V2* out) {
  out->x = x->x + y->x;
  out->y = x->y + y->y;
}

void V2AddF32(const V2* x, F32 c, V2* out) {
  out->x = x->x + c;
  out->y = x->y + c;
}

void V2SubV2(const V2* x, const V2* y, V2* out) {
  out->x = x->x - y->x;
  out->y = x->y - y->y;
}

void V2SubF32(const V2* x, F32 c, V2* out) {
  out->x = x->x - c;
  out->y = x->y - c;
}

void V2MultF32(const V2* x, F32 c, V2* out) {
  out->x = x->x * c;
  out->y = x->y * c;
}

void V2DivF32(const V2* x, F32 c, V2* out) {
  DEBUG_ASSERT(c != 0);
  out->x = x->x / c;
  out->y = x->y / c;
}

B32 V2ApproxEq(const V2* x, const V2* y) {
  return F32ApproxEq(x->x, y->x) &&
         F32ApproxEq(x->y, y->y);
}

void V2HadamardV2(const V2* x, const V2* y, V2* out) {
  out->x = x->x * y->x;
  out->y = x->y * y->y;
}

F32 V2DotV2(const V2* x, const V2* y) {
  return (x->x * y->x) + (x->y * y->y);
}

F32 V2InnerMultV2(const V2* x, const V2* y) {
  return V2DotV2(x, y);
}

F32 V2LengthSq(const V2* x) {
  return V2DotV2(x, x);
}

F32 V2Length(const V2* x) {
  return F32Sqrt(V2LengthSq(x));
}

void V2Normalize(const V2* x, V2* out) {
  V2DivF32(x, V2Length(x), out);
}

///////////////////////////////////////////////////////////////////////////////
// NOTE: V3 implementation
///////////////////////////////////////////////////////////////////////////////

void V3AddV3(const V3* x, const V3* y, V3* out) {
  out->x = x->x + y->x;
  out->y = x->y + y->y;
  out->z = x->z + y->z;
}

void V3AddF32(const V3* x, F32 c, V3* out) {
  out->x = x->x + c;
  out->y = x->y + c;
  out->z = x->z + c;
}

void V3SubV3(const V3* x, const V3* y, V3* out) {
  out->x = x->x - y->x;
  out->y = x->y - y->y;
  out->z = x->z - y->z;
}

void V3SubF32(const V3* x, F32 c, V3* out) {
  out->x = x->x - c;
  out->y = x->y - c;
  out->z = x->z - c;
}

void V3MultF32(const V3* x, F32 c, V3* out) {
  out->x = x->x * c;
  out->y = x->y * c;
  out->z = x->z * c;
}

void V3DivF32(const V3* x, F32 c, V3* out) {
  ASSERT(c != 0);
  out->x = x->x / c;
  out->y = x->y / c;
  out->z = x->z / c;
}

B32 V3ApproxEq(const V3* x, const V3* y) {
  return F32ApproxEq(x->x, y->x) &&
         F32ApproxEq(x->y, y->y) &&
         F32ApproxEq(x->z, y->z);
}

B32 V3Equal(const V3* x, const V3* y) {
  return x->x == y->x &&
         x->y == y->y &&
         x->z == y->z;
}

F32 V3LengthSq(const V3* x) {
  return V3DotV3(x, x);
}

F32 V3Length(const V3* x) {
  return F32Sqrt(V3LengthSq(x));
}

void V3HadamardV3(const V3* x, const V3* y, V3* out) {
  out->x = x->x * y->x;
  out->y = x->y * y->y;
  out->z = x->z * y->z;
}

F32 V3InnerMultV3(const V3* x, const V3* y) {
  return V3DotV3(x, y);
}

F32 V3DotV3(const V3* x, const V3* y) {
  return (x->x * y->x) + (x->y * y->y) + (x->z * y->z);
}

void V3CrossV3(const V3* x, const V3* y, V3* out) {
  out->x = x->y * y->z - x->z * y->y;
  out->y = x->z * y->x - x->x * y->z;
  out->z = x->x * y->y - x->y * y->x;
}

void V3Normalize(const V3* x, V3* out) {
  V3DivF32(x, V3Length(x), out);
}

F32 V3MinValue(const V3* x) {
  return MIN3(x->x, x->y, x->z);
}

F32 V3MaxValue(const V3* x) {
  return MAX3(x->x, x->y, x->z);
}

B32 V3IsBetween(const V3* x, const V3* start, const V3* end) {
  const V3* i = start;
  const V3* j = end;
  const V3* k = x;
  if (!V3AreCollinear(i, j, k)) { return false; }
  V3 ij, ik;
  V3SubV3(j, i, &ij);
  V3SubV3(k, i, &ik);
  F32 ij_dot_ik = V3DotV3(&ij, &ik);
  F32 ij_dot_ij = V3DotV3(&ij, &ij);
  return 0 < ij_dot_ik && ij_dot_ik < ij_dot_ij;
}

// https://en.wikibooks.org/wiki/Linear_Algebra/Orthogonal_Projection_Onto_a_Line
void V3Project(const V3* x, const V3* dest, V3* out) {
  V3MultF32(dest, V3DotV3(x, dest) / V3DotV3(dest, dest), out);
}

void V3RotateX(const V3* x, F32 angle_rad, V3* out_v) {
  F32 c = F32Cos(angle_rad);
  F32 s = F32Sin(angle_rad);
  out_v->x = x->x;
  out_v->y = x->y * c - x->z * s;
  out_v->z = x->y * s + x->z * c;
}

void V3RotateY(const V3* x, F32 angle_rad, V3* out_v) {
  F32 c = F32Cos(angle_rad);
  F32 s = F32Sin(angle_rad);
  out_v->x = x->x * c + x->z * s;
  out_v->y = x->y;
  out_v->z = -x->x * s + x->z * c;
}

void V3RotateZ(const V3* x, F32 angle_rad, V3* out_v) {
  F32 c = F32Cos(angle_rad);
  F32 s = F32Sin(angle_rad);
  out_v->x = x->x * c - x->y * s;
  out_v->y = x->x * s + x->y * c;
  out_v->z = x->z;
}

void V3Clamp(const V3* x, F32 min, F32 max, V3* out_v) {
  out_v->x = CLAMP(min, x->x, max);
  out_v->y = CLAMP(min, x->y, max);
  out_v->z = CLAMP(min, x->z, max);
}

B32 V3AreCollinear(const V3* x, const V3* y, const V3* z) {
  V3 xy, xz, xy_cross_xz;
  V3SubV3(y, x, &xy);
  V3SubV3(z, x, &xz);
  V3CrossV3(&xy, &xz, &xy_cross_xz);
  V3 zeros = V3Splat(0);
  return V3Equal(&xy_cross_xz, &zeros);
}

V3 V3Splat(F32 c) {
  V3 result;
  result.x = c;
  result.y = c;
  result.z = c;
  return result;
}

F32 V3AngleBetween(const V3* x, const V3* y) {
  return F32ArcCos(V3DotV3(x, y));
}

// https://stackoverflow.com/questions/43493711/the-angle-between-two-3d-vectors-with-a-result-range-0-360
F32 V3AngleBetween360(const V3* x, const V3* y, const V3* normal) {
  V3 cross;
  V3CrossV3(x, y, &cross);
  F32 dot = V3DotV3(x, y);
  F32 angle = F32ArcTan2(V3Length(&cross), dot);
  if (V3DotV3(normal, &cross) < 0) { angle = -angle; }
  return angle;
}

void V3Lerp(const V3* x, const V3* y, F32 t, V3* out) {
  out->x = F32Lerp(x->x, y->x, t);
  out->y = F32Lerp(x->y, y->y, t);
  out->z = F32Lerp(x->z, y->z, t);
}

// https://stackoverflow.com/questions/69245724/rotate-a-vector-around-an-axis-in-3d-space
void V3RotateAroundAxis(const V3* v, const V3* axis, F32 theta, V3* out) {
  V3 cross, double_cross;
  V3CrossV3(axis, v, &cross);
  V3CrossV3(axis, &cross, &double_cross);
  V3 a, b;
  V3MultF32(&cross, F32Sin(theta), &a);
  V3MultF32(&double_cross, (1.0f - F32Cos(theta)), &b);
  out->x = v->x + a.x + b.x;
  out->y = v->y + a.y + b.y;
  out->z = v->z + a.z + b.z;
}

///////////////////////////////////////////////////////////////////////////////
// NOTE: V4 implementation
///////////////////////////////////////////////////////////////////////////////

void V4AddV4(const V4* x, const V4* y, V4* out) {
  out->x = x->x + y->x;
  out->y = x->y + y->y;
  out->z = x->z + y->z;
  out->w = x->w + y->w;
}

void V4AddF32(const V4* x, F32 c, V4* out) {
  out->x = x->x + c;
  out->y = x->y + c;
  out->z = x->z + c;
  out->w = x->w + c;
}

void V4SubV4(const V4* x, const V4* y, V4* out) {
  out->x = x->x - y->x;
  out->y = x->y - y->y;
  out->z = x->z - y->z;
  out->w = x->w - y->w;
}

void V4SubF32(const V4* x, F32 c, V4* out) {
  out->x = x->x - c;
  out->y = x->y - c;
  out->z = x->z - c;
  out->w = x->w - c;
}

void V4MultF32(const V4* x, F32 c, V4* out) {
  out->x = x->x * c;
  out->y = x->y * c;
  out->z = x->z * c;
  out->w = x->w * c;
}

void V4DivF32(const V4* x, F32 c, V4* out) {
  out->x = x->x / c;
  out->y = x->y / c;
  out->z = x->z / c;
  out->w = x->w / c;
}

B32 V4ApproxEq(const V4* x, const V4* y) {
  return F32ApproxEq(x->x, y->x) &&
         F32ApproxEq(x->y, y->y) &&
         F32ApproxEq(x->z, y->z) &&
         F32ApproxEq(x->w, y->w);
}

F32 V4LengthSq(const V4* x) {
  return V4DotV4(x, x);
}

F32 V4Length(const V4* x) {
  return F32Sqrt(V4LengthSq(x));
}

void V4Hadamard(const V4* x, const V4* y, V4* out) {
  out->x = x->x * y->x;
  out->y = x->y * y->y;
  out->z = x->z * y->z;
  out->w = x->w * y->w;
}

F32 V4InnerMultV4(const V4* x, const V4* y) {
  return V4DotV4(x, y);
}

F32 V4DotV4(const V4* x, const V4* y) {
  return (x->x + y->x) + (x->y + y->y) + (x->z + y->z) + (x->w + y->w);
}

void V4Normalize(const V4* x, V4* out) {
  V4DivF32(x, V4Length(x), out);
}

void V4Lerp(const V4* a, const V4* b, F32 t, V4* out) {
  out->x = F32Lerp(a->x, b->x, t);
  out->y = F32Lerp(a->y, b->y, t);
  out->z = F32Lerp(a->z, b->z, t);
  out->w = F32Lerp(a->w, b->w, t);
}

void V4Slerp(const V4* a, const V4* b, F32 t, V4* out) {
  V4 a_copy = *a;
  V4 b_copy = *b;
  F32 dot = V4DotV4(&a_copy, &b_copy);
  if (dot < 0) {
    dot = -dot;
    V4MultF32(a, -1, &a_copy);
  }

  if (UNLIKELY(F32ApproxEq(dot, 1))) {
    V4Lerp(&a_copy, &b_copy, t, out);
    return;
  }

  F32 theta = F32ArcCos(dot);
  F32 a_scalar = F32Sin((1.0f - t) * theta) / F32Sin(theta);
  F32 b_scalar = F32Sin(t * theta) / F32Sin(theta);

  V4MultF32(&a_copy, a_scalar, &a_copy);
  V4MultF32(&b_copy, b_scalar, &b_copy);
  V4AddV4(&a_copy, &b_copy, out);
}

void V4RotateAroundAxis(const V3* axis, F32 theta, V4* out) {
  F32 factor = F32Sin(theta / 2.0f);
  out->x = factor * axis->x;
  out->y = factor * axis->y;
  out->z = factor * axis->z;
  out->w = F32Cos(theta / 2.0f);
  V4Normalize(out, out);
}

// https://stackoverflow.com/questions/12435671/quaternion-lookat-function
void V4LookInDir(const V3* dir, V4* out) {
  ASSERT(F32ApproxEq(V3LengthSq(dir), 1.0f));
  F32 dot = V3DotV3(&V3_Y_NEG, dir);
  if (F32ApproxEq(dot, -1)) {
    // TODO: this is probably wrong?
    *out = (V4) {0, 0, 1, 0};
    return;
  } else if (F32ApproxEq(dot, +1)) {
    *out = V4_QUAT_IDENT;
    return;
  }
  V3 axis;
  V3CrossV3(&V3_Y_NEG, dir, &axis);
  V3Normalize(&axis, &axis);
  F32 angle = F32ArcCos(dot);
  F32 half_sin = F32Sin(angle / 2.0f);
  F32 half_cos = F32Cos(angle / 2.0f);
  out->x = axis.x * half_sin;
  out->y = axis.y * half_sin;
  out->z = axis.z * half_sin;
  out->w = half_cos;
}

void V4LookAt(const V3* pos, const V3* target, V4* out) {
  V3 dir;
  V3SubV3(target, pos, &dir);
  V3Normalize(&dir, &dir);
  V4LookInDir(&dir, out);
}

///////////////////////////////////////////////////////////////////////////////
// NOTE: M3 implementation
///////////////////////////////////////////////////////////////////////////////

void M3MultM3(const M3* x, const M3* y, M3* out) {
  M3 y_t;
  M3Transpose(y, &y_t);
  for (U32 i = 0; i < 3; i++) {
    for (U32 j = 0; j < 3; j++) {
      V3* x_sub = (V3*) &x->e[i][0];
      V3* y_sub = (V3*) &y_t.e[j][0];
      out->e[i][j] = V3DotV3(x_sub, y_sub);
    }
  }
}

void M3MultV3(const M3* x, const V3* y, V3* out) {
  for (U32 i = 0; i < 3; i++) {
    V3* x_sub = (V3*) &x->e[i][0];
    out->e[i] = V3DotV3(x_sub, y);
  }
}

void M3Transpose(const M3* x, M3* out) {
  for (U32 i = 0; i < 3; i++) {
    for (U32 j = 0; j < 3; j++) {
      out->e[j][i] = x->e[i][j];
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// NOTE: M4 implementation
///////////////////////////////////////////////////////////////////////////////

void M4MultM4(const M4* x, const M4* y, M4* out) {
  M4 y_t;
  M4Transpose(y, &y_t);
  for (int32_t i = 0; i < 4; i++) {
    for (int32_t j = 0; j < 4; j++) {
      V4* x_sub = (V4*) &x->e[i][0];
      V4* y_sub = (V4*) &y_t.e[j][0];
      out->e[i][j] = V4DotV4(x_sub, y_sub);
    }
  }
}

void M4MultV4(const M4* x, const V4* y, V4* out) {
  for (int32_t j = 0; j < 4; j++) {
    V4* x_sub = (V4*) &x->e[j][0];
    out->e[j] = V4DotV4(x_sub, y);
  }
}

void M4Transpose(const M4* x, M4* out) {
  for (U32 i = 0; i < 4; i++) {
    for (U32 j = 0; j < 4; j++) {
      out->e[j][i] = x->e[i][j];
    }
  }
}

// http://rodolphe-vaillant.fr/entry/7/c-code-for-4x4-matrix-inversion
void M4Invert(const M4* x, M4* out) {
  M4 t;
  t.i[ 0] =  x->i[5] * x->i[10] * x->i[15] - x->i[5] * x->i[14] * x->i[11] - x->i[6] * x->i[9] * x->i[15] + x->i[6] * x->i[13] * x->i[11] + x->i[7] * x->i[9] * x->i[14] - x->i[7] * x->i[13] * x->i[10];
  t.i[ 1] = -x->i[1] * x->i[10] * x->i[15] + x->i[1] * x->i[14] * x->i[11] + x->i[2] * x->i[9] * x->i[15] - x->i[2] * x->i[13] * x->i[11] - x->i[3] * x->i[9] * x->i[14] + x->i[3] * x->i[13] * x->i[10];
  t.i[ 2] =  x->i[1] * x->i[ 6] * x->i[15] - x->i[1] * x->i[14] * x->i[ 7] - x->i[2] * x->i[5] * x->i[15] + x->i[2] * x->i[13] * x->i[ 7] + x->i[3] * x->i[5] * x->i[14] - x->i[3] * x->i[13] * x->i[ 6];
  t.i[ 3] = -x->i[1] * x->i[ 6] * x->i[11] + x->i[1] * x->i[10] * x->i[ 7] + x->i[2] * x->i[5] * x->i[11] - x->i[2] * x->i[ 9] * x->i[ 7] - x->i[3] * x->i[5] * x->i[10] + x->i[3] * x->i[ 9] * x->i[ 6];
  t.i[ 4] = -x->i[4] * x->i[10] * x->i[15] + x->i[4] * x->i[14] * x->i[11] + x->i[6] * x->i[8] * x->i[15] - x->i[6] * x->i[12] * x->i[11] - x->i[7] * x->i[8] * x->i[14] + x->i[7] * x->i[12] * x->i[10];
  t.i[ 5] =  x->i[0] * x->i[10] * x->i[15] - x->i[0] * x->i[14] * x->i[11] - x->i[2] * x->i[8] * x->i[15] + x->i[2] * x->i[12] * x->i[11] + x->i[3] * x->i[8] * x->i[14] - x->i[3] * x->i[12] * x->i[10];
  t.i[ 6] = -x->i[0] * x->i[ 6] * x->i[15] + x->i[0] * x->i[14] * x->i[ 7] + x->i[2] * x->i[4] * x->i[15] - x->i[2] * x->i[12] * x->i[ 7] - x->i[3] * x->i[4] * x->i[14] + x->i[3] * x->i[12] * x->i[ 6];
  t.i[ 7] =  x->i[0] * x->i[ 6] * x->i[11] - x->i[0] * x->i[10] * x->i[ 7] - x->i[2] * x->i[4] * x->i[11] + x->i[2] * x->i[ 8] * x->i[ 7] + x->i[3] * x->i[4] * x->i[10] - x->i[3] * x->i[ 8] * x->i[ 6];
  t.i[ 8] =  x->i[4] * x->i[ 9] * x->i[15] - x->i[4] * x->i[13] * x->i[11] - x->i[5] * x->i[8] * x->i[15] + x->i[5] * x->i[12] * x->i[11] + x->i[7] * x->i[8] * x->i[13] - x->i[7] * x->i[12] * x->i[ 9];
  t.i[ 9] = -x->i[0] * x->i[ 9] * x->i[15] + x->i[0] * x->i[13] * x->i[11] + x->i[1] * x->i[8] * x->i[15] - x->i[1] * x->i[12] * x->i[11] - x->i[3] * x->i[8] * x->i[13] + x->i[3] * x->i[12] * x->i[ 9];
  t.i[10] =  x->i[0] * x->i[ 5] * x->i[15] - x->i[0] * x->i[13] * x->i[ 7] - x->i[1] * x->i[4] * x->i[15] + x->i[1] * x->i[12] * x->i[ 7] + x->i[3] * x->i[4] * x->i[13] - x->i[3] * x->i[12] * x->i[ 5];
  t.i[11] = -x->i[0] * x->i[ 5] * x->i[11] + x->i[0] * x->i[ 9] * x->i[ 7] + x->i[1] * x->i[4] * x->i[11] - x->i[1] * x->i[ 8] * x->i[ 7] - x->i[3] * x->i[4] * x->i[ 9] + x->i[3] * x->i[ 8] * x->i[ 5];
  t.i[12] = -x->i[4] * x->i[ 9] * x->i[14] + x->i[4] * x->i[13] * x->i[10] + x->i[5] * x->i[8] * x->i[14] - x->i[5] * x->i[12] * x->i[10] - x->i[6] * x->i[8] * x->i[13] + x->i[6] * x->i[12] * x->i[ 9];
  t.i[13] =  x->i[0] * x->i[ 9] * x->i[14] - x->i[0] * x->i[13] * x->i[10] - x->i[1] * x->i[8] * x->i[14] + x->i[1] * x->i[12] * x->i[10] + x->i[2] * x->i[8] * x->i[13] - x->i[2] * x->i[12] * x->i[ 9];
  t.i[14] = -x->i[0] * x->i[ 5] * x->i[14] + x->i[0] * x->i[13] * x->i[ 6] + x->i[1] * x->i[4] * x->i[14] - x->i[1] * x->i[12] * x->i[ 6] - x->i[2] * x->i[4] * x->i[13] + x->i[2] * x->i[12] * x->i[ 5];
  t.i[15] =  x->i[0] * x->i[ 5] * x->i[10] - x->i[0] * x->i[ 9] * x->i[ 6] - x->i[1] * x->i[4] * x->i[10] + x->i[1] * x->i[ 8] * x->i[ 6] + x->i[2] * x->i[4] * x->i[ 9] - x->i[2] * x->i[ 8] * x->i[ 5];

  F32 det = x->i[0] * t.i[0] + x->i[4] * t.i[1] + x->i[8] * t.i[2] + x->i[12] * t.i[3];
  ASSERT(det != 0);
  det = 1 / det;
  for(int32_t j = 0; j < 16; j++) { out->i[j] = t.i[j] * det; }
}

void M4Perspective(F32 fov_y_rad, F32 aspect_ratio, F32 near_plane, F32 far_plane, M4* out) {
  F32 tan_half_fov_y = F32Tan(fov_y_rad / 2.0f);

  F32 a = 1.0f / (aspect_ratio * tan_half_fov_y);
  F32 b = 1.0f / (tan_half_fov_y);
  F32 c = - (far_plane + near_plane) / (far_plane - near_plane);
  F32 d = - (2 * far_plane * near_plane) / (far_plane - near_plane);

  *out = (M4) {
    a,  0,  0,  0,
    0,  b,  0,  0,
    0,  0,  c,  d,
    0,  0, -1,  0,
  };
}

void M4Orthographic(F32 left, F32 right, F32 bottom, F32 top, F32 near_plane, F32 far_plane, M4* out) {
  F32 a = 2 / (right - left);
  F32 b = 2 / (top - bottom);
  F32 c = -2 / (far_plane - near_plane);
  F32 d = -(right + left) / (right - left);
  F32 e = -(top + bottom) / (top - bottom);
  F32 f = -(far_plane + near_plane) / (far_plane - near_plane);

  *out = (M4) {
    a, 0, 0, d,
    0, b, 0, e,
    0, 0, c, f,
    0, 0, 0, 1,
  };
}

void LookAt(const V3* eye, const V3* target, const V3* up, M4* out) {
  V3 x, y, z;
  V3SubV3(eye, target, &z);
  V3Normalize(&z, &z);
  y = *up;
  V3CrossV3(&y, &z, &x);
  V3CrossV3(&z, &x, &y);

  V3Normalize(&x, &x);
  V3Normalize(&y, &y);

  F32 a = -V3DotV3(&x, eye);
  F32 b = -V3DotV3(&y, eye);
  F32 c = -V3DotV3(&z, eye);

  *out = (M4) {
    x.x, x.y, x.z, a,
    y.x, y.y, y.z, b,
    z.x, z.y, z.z, c,
      0,   0,   0, 1,
  };
}

#endif // CDEFAULT_MATH_IMPLEMENTATION
