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

V2 V2AddV2(V2 x, V2 y);
V2 V2AddF32(V2 x, F32 c);
V2 V2SubV2(V2 x, V2 y);
V2 V2SubF32(V2 x, F32 c);
V2 V2MultF32(V2 x, F32 c);
V2 V2DivF32(V2 x, F32 c);
V2 V2HadamardV2(V2 x, V2 y);
F32 V2DotV2(V2 x, V2 y);
F32 V2InnerMultV2(V2 x, V2 y);
F32 V2LengthSq(V2 x);
F32 V2Length(V2 x);
V2 V2Normalize(V2 x);

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

V3 V3AddV3(V3 x, V3 y);
V3 V3AddF32(V3 x, F32 c);
V3 V3SubV3(V3 x, V3 y);
V3 V3SubF32(V3 x, F32 c);
V3 V3MultF32(V3 x, F32 c);
V3 V3DivF32(V3 x, F32 c);
F32 V3LengthSq(V3 x);
F32 V3Length(V3 x);
V3 V3HadamardV3(V3 x, V3 y);
F32 V3InnerMultV3(V3 x, V3 y);
F32 V3DotV3(V3 x, V3 y);
V3 V3CrossV3(V3 x, V3 y);
V3 V3Normalize(V3 x);
F32 V3MinValue(V3 x);
F32 V3MaxValue(V3 x);
B32 V3IsBetween(V3 x, V3 start, V3 end);
V3 V3Project(V3 x, V3 dest);
V3 V3RotateX(V3 x, F32 angle_rad);
V3 V3RotateY(V3 x, F32 angle_rad);
V3 V3RotateZ(V3 x, F32 angle_rad);
V3 V3Clamp(V3 x, F32 min, F32 max);
B32 V3Equal(V3 a, V3 b);
B32 V3AreCollinear(V3 a, V3 b, V3 c);
V3 V3Splat(F32 c);
F32 V3AngleBetween(V3 a, V3 b);
F32 V3AngleBetween360(V3 a, V3 b, V3 normal);
V3 V3Lerp(V3 x, V3 y, F32 t);
V3 V3RotateAroundAxis(V3 v, V3 axis, F32 theta);

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

V4 V4AddV4(V4 x, V4 y);
V4 V4AddF32(V4 x, F32 c);
V4 V4SubV4(V4 x, V4 y);
V4 V4SubF32(V4 x, F32 c);
V4 V4MultF32(V4 x, F32 c);
V4 V4DivF32(V4 x, F32 c);
F32 V4LengthSq(V4 x);
F32 V4Length(V4 x);
V4 V4HadamardV4(V4 x, V4 y);
F32 V4InnerMultV4(V4 x, V4 y);
F32 V4DotV4(V4 x, V4 y);
V4 V4Normalize(V4 x);
B32 V4Equal(V4 a, V4 b);
V4 V4Lerp(V4 a, V4 b, F32 t);
V4 V4Slerp(V4 a, V4 b, F32 t);
V4 V4RotateAroundAxis(V3 axis, F32 theta);
V4 V4LookInDir(V3 dir);
V4 V4LookAt(V3 pos, V3 target);

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

M3 M3MultM3(M3 x, M3 y);
V3 M3MultV3(M3 x, V3 y);
M3 M3Transpose(M3 x);

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

M4 M4MultM4(M4 x, M4 y);
V4 M4MultV4(M4 x, V4 y);
M4 M4Transpose(M4 x);
M4 M4Invert(M4 x);
M4 M4Perspective(F32 fov_y_rad, F32 aspect_ratio, F32 near_plane, F32 far_plane);
M4 M4Orthographic(F32 left, F32 right, F32 bottom, F32 top, F32 near_plane, F32 far_plane);
M4 LookAt(V3 eye, V3 target, V3 up);

#endif // CDEFAULT_MATH_H_

#ifdef CDEFAULT_MATH_IMPLEMENTATION

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

V2 V2AddV2(V2 x, V2 y) {
  V2 result;
  result.x = x.x + y.x;
  result.y = x.y + y.y;
  return result;
}

V2 V2AddF32(V2 x, F32 c) {
  V2 result;
  result.x = x.x + c;
  result.y = x.y + c;
  return result;
}

V2 V2SubV2(V2 x, V2 y) {
  V2 result;
  result.x = x.x - y.x;
  result.y = x.y - y.y;
  return result;
}

V2 V2SubF32(V2 x, F32 c) {
  V2 result;
  result.x = x.x - c;
  result.y = x.y - c;
  return result;
}

V2 V2MultF32(V2 x, F32 c) {
  V2 result;
  result.x = x.x * c;
  result.y = x.y * c;
  return result;
}

V2 V2DivF32(V2 x, F32 c) {
  ASSERT(c != 0);
  V2 result;
  result.x = x.x / c;
  result.y = x.y / c;
  return result;
}

V2 V2HadamardV2(V2 x, V2 y) {
  V2 result;
  result.x = x.x * y.x;
  result.y = x.y * y.y;
  return result;
}

F32 V2DotV2(V2 x, V2 y) {
  return (x.x * y.x) + (x.y * y.y);
}

F32 V2InnerMultV2(V2 x, V2 y) {
  return V2DotV2(x, y);
}

F32 V2LengthSq(V2 x) {
  return V2DotV2(x, x);
}

F32 V2Length(V2 x) {
  return F32Sqrt(V2LengthSq(x));
}

V2 V2Normalize(V2 x) {
  return V2DivF32(x, V2Length(x));
}

///////////////////////////////////////////////////////////////////////////////
// NOTE: V3 implementation
///////////////////////////////////////////////////////////////////////////////

V3 V3AddV3(V3 x, V3 y) {
  V3 result;
  result.x = x.x + y.x;
  result.y = x.y + y.y;
  result.z = x.z + y.z;
  return result;
}

V3 V3AddF32(V3 x, F32 c) {
  V3 result;
  result.x = x.x + c;
  result.y = x.y + c;
  result.z = x.z + c;
  return result;
}

V3 V3SubV3(V3 x, V3 y) {
  V3 result;
  result.x = x.x - y.x;
  result.y = x.y - y.y;
  result.z = x.z - y.z;
  return result;
}

V3 V3SubF32(V3 x, F32 c) {
  V3 result;
  result.x = x.x - c;
  result.y = x.y - c;
  result.z = x.z - c;
  return result;
}

V3 V3MultF32(V3 x, F32 c) {
  V3 result;
  result.x = x.x * c;
  result.y = x.y * c;
  result.z = x.z * c;
  return result;
}

V3 V3DivF32(V3 x, F32 c) {
  ASSERT(c != 0);
  V3 result;
  result.x = x.x / c;
  result.y = x.y / c;
  result.z = x.z / c;
  return result;
}

F32 V3LengthSq(V3 x) {
  return V3DotV3(x, x);
}

F32 V3Length(V3 x) {
  return F32Sqrt(V3LengthSq(x));
}

V3 V3HadamardV3(V3 x, V3 y) {
  V3 result;
  result.x = x.x * y.x;
  result.y = x.y * y.y;
  result.z = x.z * y.z;
  return result;
}

F32 V3InnerMultV3(V3 x, V3 y) {
  return V3DotV3(x, y);
}

F32 V3DotV3(V3 x, V3 y) {
  return (x.x * y.x) + (x.y * y.y) + (x.z * y.z);
}

V3 V3CrossV3(V3 x, V3 y) {
  V3 result;
  result.x = x.y * y.z - x.z * y.y;
  result.y = x.z * y.x - x.x * y.z;
  result.z = x.x * y.y - x.y * y.x;
  return result;
}

V3 V3Normalize(V3 x) {
  return V3DivF32(x, V3Length(x));
}

F32 V3MinValue(V3 x) {
  return MIN3(x.x, x.y, x.z);
}

F32 V3MaxValue(V3 x) {
  return MAX3(x.x, x.y, x.z);
}

B32 V3IsBetween(V3 x, V3 start, V3 end) {
  V3 i = start;
  V3 j = end;
  V3 k = x;
  if (!V3AreCollinear(i, j, k)) { return false; }
  V3 ij = V3SubV3(j, i);
  V3 ik = V3SubV3(k, i);
  F32 ij_dot_ik = V3DotV3(ij, ik);
  F32 ij_dot_ij = V3DotV3(ij, ij);
  return 0 < ij_dot_ik && ij_dot_ik < ij_dot_ij;
}

// https://en.wikibooks.org/wiki/Linear_Algebra/Orthogonal_Projection_Onto_a_Line
V3 V3Project(V3 x, V3 dest) {
  return V3MultF32(dest, V3DotV3(x, dest) / V3DotV3(dest, dest));
}

V3 V3RotateX(V3 x, F32 angle_rad) {
  F32 c = F32Cos(angle_rad);
  F32 s = F32Sin(angle_rad);
  V3 result;
  result.x = x.x;
  result.y = x.y * c - x.z * s;
  result.z = x.y * s + x.z * c;
  return result;
}

V3 V3RotateY(V3 x, F32 angle_rad) {
  F32 c = F32Cos(angle_rad);
  F32 s = F32Sin(angle_rad);
  V3 result;
  result.x = x.x * c + x.z * s;
  result.y = x.y;
  result.z = -x.x * s + x.z * c;
  return result;
}

V3 V3RotateZ(V3 x, F32 angle_rad) {
  F32 c = F32Cos(angle_rad);
  F32 s = F32Sin(angle_rad);
  V3 result;
  result.x = x.x * c - x.y * s;
  result.y = x.x * s + x.y * c;
  result.z = x.z;
  return result;
}

V3 V3Clamp(V3 x, F32 min, F32 max) {
  V3 result;
  result.x = CLAMP(min, x.x, max);
  result.y = CLAMP(min, x.y, max);
  result.z = CLAMP(min, x.z, max);
  return result;
}

B32 V3Equal(V3 a, V3 b) {
  return a.x == b.x && a.y == b.y && a.z == b.z;
}

B32 V3AreCollinear(V3 a, V3 b, V3 c) {
  V3 ab = V3SubV3(b, a);
  V3 ac = V3SubV3(c, a);
  return V3Equal(V3CrossV3(ab, ac), V3Splat(0));
}

V3 V3Splat(F32 c) {
  V3 result;
  result.x = c;
  result.y = c;
  result.z = c;
  return result;
}

F32 V3AngleBetween(V3 a, V3 b) {
  return F32ArcCos(V3DotV3(a, b));
}

// https://stackoverflow.com/questions/43493711/the-angle-between-two-3d-vectors-with-a-result-range-0-360
F32 V3AngleBetween360(V3 a, V3 b, V3 normal) {
  V3 cross = V3CrossV3(a, b);
  F32 dot = V3DotV3(a, b);
  F32 angle = F32ArcTan2(V3Length(cross), dot);
  if (V3DotV3(normal, cross) < 0) { angle = -angle; }
  return angle;
}

V3 V3Lerp(V3 x, V3 y, F32 t) {
  V3 result;
  result.x = F32Lerp(x.x, y.x, t);
  result.y = F32Lerp(x.y, y.y, t);
  result.z = F32Lerp(x.z, y.z, t);
  return result;
}

// https://stackoverflow.com/questions/69245724/rotate-a-vector-around-an-axis-in-3d-space
V3 V3RotateAroundAxis(V3 v, V3 axis, F32 theta) {
  V3 cross = V3CrossV3(axis, v);
  V3 double_cross = V3CrossV3(axis, cross);
  V3 a = V3MultF32(cross, F32Sin(theta));
  V3 b = V3MultF32(double_cross, (1.0f - F32Cos(theta)));
  V3 result;
  result.x = v.x + a.x + b.x;
  result.y = v.y + a.y + b.y;
  result.z = v.z + a.z + b.z;
  return result;
}

///////////////////////////////////////////////////////////////////////////////
// NOTE: V4 implementation
///////////////////////////////////////////////////////////////////////////////

V4 V4AddV4(V4 x, V4 y) {
  V4 result;
  result.x = x.x + y.x;
  result.y = x.y + y.y;
  result.z = x.z + y.z;
  result.w = x.w + y.w;
  return result;
}

V4 V4AddF32(V4 x, F32 c) {
  V4 result;
  result.x = x.x + c;
  result.y = x.y + c;
  result.z = x.z + c;
  result.w = x.w + c;
  return result;
}

V4 V4SubV4(V4 x, V4 y) {
  V4 result;
  result.x = x.x - y.x;
  result.y = x.y - y.y;
  result.z = x.z - y.z;
  result.w = x.w - y.w;
  return result;
}

V4 V4SubF32(V4 x, F32 c) {
  V4 result;
  result.x = x.x - c;
  result.y = x.y - c;
  result.z = x.z - c;
  result.w = x.w - c;
  return result;
}

V4 V4MultF32(V4 x, F32 c) {
  V4 result;
  result.x = x.x * c;
  result.y = x.y * c;
  result.z = x.z * c;
  result.w = x.w * c;
  return result;
}

V4 V4DivF32(V4 x, F32 c) {
  V4 result;
  result.x = x.x / c;
  result.y = x.y / c;
  result.z = x.z / c;
  result.w = x.w / c;
  return result;
}

F32 V4LengthSq(V4 x) {
  return V4DotV4(x, x);
}

F32 V4Length(V4 x) {
  return F32Sqrt(V4LengthSq(x));
}

V4 V4Hadamard(V4 x, V4 y) {
  V4 result;
  result.x = x.x * y.x;
  result.y = x.y * y.y;
  result.z = x.z * y.z;
  result.w = x.w * y.w;
  return result;
}

F32 V4InnerMultV4(V4 x, V4 y) {
  return V4DotV4(x, y);
}

F32 V4DotV4(V4 x, V4 y) {
  return (x.x + y.x) + (x.y + y.y) + (x.z + y.z) + (x.w + y.w);
}

V4 V4Normalize(V4 x) {
  return V4DivF32(x, V4Length(x));
}

B32 V4Equal(V4 a, V4 b) {
  return (a.x == b.x) && (a.y == b.y) && (a.z == b.z) && (a.w == b.w);
}

V4 V4Lerp(V4 a, V4 b, F32 t) {
  V4 result;
  result.x = F32Lerp(a.x, b.x, t);
  result.y = F32Lerp(a.y, b.y, t);
  result.z = F32Lerp(a.z, b.z, t);
  result.w = F32Lerp(a.w, b.w, t);
  return result;
}

V4 V4Slerp(V4 a, V4 b, F32 t) {
  F32 dot = V4DotV4(a, b);
  if (dot < 0) {
    dot = -dot;
    a = V4MultF32(a, -1);
  }

  if (UNLIKELY(F32ApproxEq(dot, 1))) {
    return V4Lerp(a, b, t);
  }

  F32 theta = F32ArcCos(dot);
  F32 a_scalar = F32Sin((1.0f - t) * theta) / F32Sin(theta);
  F32 b_scalar = F32Sin(t * theta) / F32Sin(theta);
  return V4AddV4(V4MultF32(a, a_scalar), V4MultF32(b, b_scalar));
}

V4 V4RotateAroundAxis(V3 axis, F32 theta) {
  F32 factor = F32Sin(theta / 2.0f);
  V4 result;
  result.x = factor * axis.x;
  result.y = factor * axis.y;
  result.z = factor * axis.z;
  result.w = F32Cos(theta / 2.0f);
  result = V4Normalize(result);
  return result;
}

// https://stackoverflow.com/questions/12435671/quaternion-lookat-function
V4 V4LookInDir(V3 dir) {
  ASSERT(F32ApproxEq(V3LengthSq(dir), 1.0f));
  F32 dot = V3DotV3(V3_Y_NEG, dir);

  if (F32ApproxEq(dot, -1)) {
    // TODO: this is probably wrong?
    return (V4) {0, 0, 1, 0};
  } else if (F32ApproxEq(dot, +1)) {
    return V4_QUAT_IDENT;
  }

  F32 angle = F32ArcCos(dot);
  V3 axis = V3Normalize(V3CrossV3(V3_Y_NEG, dir));
  F32 half_sin = F32Sin(angle / 2.0f);
  F32 half_cos = F32Cos(angle / 2.0f);

  V4 result;
  result.x = axis.x * half_sin;
  result.y = axis.y * half_sin;
  result.z = axis.z * half_sin;
  result.w = half_cos;
  return result;
}

V4 V4LookAt(V3 pos, V3 target) {
  V3 dir = V3Normalize(V3SubV3(target, pos));
  return V4LookInDir(dir);
}

///////////////////////////////////////////////////////////////////////////////
// NOTE: M3 implementation
///////////////////////////////////////////////////////////////////////////////

M3 M3MultM3(M3 x, M3 y) {
  M3 y_t = M3Transpose(y);
  M3 result;
  for (U32 i = 0; i < 3; i++) {
    for (U32 j = 0; j < 3; j++) {
      V3* x_sub = (V3*) &x.e[i][0];
      V3* y_sub = (V3*) &y_t.e[j][0];
      result.e[i][j] = V3DotV3(*x_sub, *y_sub);
    }
  }
  return result;
}

V3 M3MultV3(M3 x, V3 y) {
  V3 result;
  for (U32 i = 0; i < 3; i++) {
    V3* x_sub = (V3*) &x.e[i][0];
    result.e[i] = V3DotV3(*x_sub, y);
  }
  return result;
}

M3 M3Transpose(M3 x) {
  M3 x_t;
  for (U32 i = 0; i < 3; i++) {
    for (U32 j = 0; j < 3; j++) {
      x_t.e[j][i] = x.e[i][j];
    }
  }
  return x_t;
}

///////////////////////////////////////////////////////////////////////////////
// NOTE: M4 implementation
///////////////////////////////////////////////////////////////////////////////

M4 M4MultM4(M4 x, M4 y) {
  M4 y_t = M4Transpose(x);
  M4 result;
  for (int32_t i = 0; i < 4; i++) {
    for (int32_t j = 0; j < 4; j++) {
      V4* x_sub = (V4*) &x.e[i][0];
      V4* y_sub = (V4*) &y_t.e[j][0];
      result.e[i][j] = V4DotV4(*x_sub, *y_sub);
    }
  }
  return result;
}

V4 M4MultV4(M4 x, V4 y) {
  V4 result;
  for (int32_t j = 0; j < 4; j++) {
    V4* x_sub = (V4*) &x.e[j][0];
    result.e[j] = V4DotV4(*x_sub, y);
  }
  return result;
}

M4 M4Transpose(M4 x) {
  M4 x_t;
  for (U32 i = 0; i < 4; i++) {
    for (U32 j = 0; j < 4; j++) {
      x_t.e[j][i] = x.e[i][j];
    }
  }
  return x_t;
}

// http://rodolphe-vaillant.fr/entry/7/c-code-for-4x4-matrix-inversion
M4 M4Invert(M4 x) {
  M4 t;
  t.i[ 0] =  x.i[5] * x.i[10] * x.i[15] - x.i[5] * x.i[14] * x.i[11] - x.i[6] * x.i[9] * x.i[15] + x.i[6] * x.i[13] * x.i[11] + x.i[7] * x.i[9] * x.i[14] - x.i[7] * x.i[13] * x.i[10];
  t.i[ 1] = -x.i[1] * x.i[10] * x.i[15] + x.i[1] * x.i[14] * x.i[11] + x.i[2] * x.i[9] * x.i[15] - x.i[2] * x.i[13] * x.i[11] - x.i[3] * x.i[9] * x.i[14] + x.i[3] * x.i[13] * x.i[10];
  t.i[ 2] =  x.i[1] * x.i[ 6] * x.i[15] - x.i[1] * x.i[14] * x.i[ 7] - x.i[2] * x.i[5] * x.i[15] + x.i[2] * x.i[13] * x.i[ 7] + x.i[3] * x.i[5] * x.i[14] - x.i[3] * x.i[13] * x.i[ 6];
  t.i[ 3] = -x.i[1] * x.i[ 6] * x.i[11] + x.i[1] * x.i[10] * x.i[ 7] + x.i[2] * x.i[5] * x.i[11] - x.i[2] * x.i[ 9] * x.i[ 7] - x.i[3] * x.i[5] * x.i[10] + x.i[3] * x.i[ 9] * x.i[ 6];
  t.i[ 4] = -x.i[4] * x.i[10] * x.i[15] + x.i[4] * x.i[14] * x.i[11] + x.i[6] * x.i[8] * x.i[15] - x.i[6] * x.i[12] * x.i[11] - x.i[7] * x.i[8] * x.i[14] + x.i[7] * x.i[12] * x.i[10];
  t.i[ 5] =  x.i[0] * x.i[10] * x.i[15] - x.i[0] * x.i[14] * x.i[11] - x.i[2] * x.i[8] * x.i[15] + x.i[2] * x.i[12] * x.i[11] + x.i[3] * x.i[8] * x.i[14] - x.i[3] * x.i[12] * x.i[10];
  t.i[ 6] = -x.i[0] * x.i[ 6] * x.i[15] + x.i[0] * x.i[14] * x.i[ 7] + x.i[2] * x.i[4] * x.i[15] - x.i[2] * x.i[12] * x.i[ 7] - x.i[3] * x.i[4] * x.i[14] + x.i[3] * x.i[12] * x.i[ 6];
  t.i[ 7] =  x.i[0] * x.i[ 6] * x.i[11] - x.i[0] * x.i[10] * x.i[ 7] - x.i[2] * x.i[4] * x.i[11] + x.i[2] * x.i[ 8] * x.i[ 7] + x.i[3] * x.i[4] * x.i[10] - x.i[3] * x.i[ 8] * x.i[ 6];
  t.i[ 8] =  x.i[4] * x.i[ 9] * x.i[15] - x.i[4] * x.i[13] * x.i[11] - x.i[5] * x.i[8] * x.i[15] + x.i[5] * x.i[12] * x.i[11] + x.i[7] * x.i[8] * x.i[13] - x.i[7] * x.i[12] * x.i[ 9];
  t.i[ 9] = -x.i[0] * x.i[ 9] * x.i[15] + x.i[0] * x.i[13] * x.i[11] + x.i[1] * x.i[8] * x.i[15] - x.i[1] * x.i[12] * x.i[11] - x.i[3] * x.i[8] * x.i[13] + x.i[3] * x.i[12] * x.i[ 9];
  t.i[10] =  x.i[0] * x.i[ 5] * x.i[15] - x.i[0] * x.i[13] * x.i[ 7] - x.i[1] * x.i[4] * x.i[15] + x.i[1] * x.i[12] * x.i[ 7] + x.i[3] * x.i[4] * x.i[13] - x.i[3] * x.i[12] * x.i[ 5];
  t.i[11] = -x.i[0] * x.i[ 5] * x.i[11] + x.i[0] * x.i[ 9] * x.i[ 7] + x.i[1] * x.i[4] * x.i[11] - x.i[1] * x.i[ 8] * x.i[ 7] - x.i[3] * x.i[4] * x.i[ 9] + x.i[3] * x.i[ 8] * x.i[ 5];
  t.i[12] = -x.i[4] * x.i[ 9] * x.i[14] + x.i[4] * x.i[13] * x.i[10] + x.i[5] * x.i[8] * x.i[14] - x.i[5] * x.i[12] * x.i[10] - x.i[6] * x.i[8] * x.i[13] + x.i[6] * x.i[12] * x.i[ 9];
  t.i[13] =  x.i[0] * x.i[ 9] * x.i[14] - x.i[0] * x.i[13] * x.i[10] - x.i[1] * x.i[8] * x.i[14] + x.i[1] * x.i[12] * x.i[10] + x.i[2] * x.i[8] * x.i[13] - x.i[2] * x.i[12] * x.i[ 9];
  t.i[14] = -x.i[0] * x.i[ 5] * x.i[14] + x.i[0] * x.i[13] * x.i[ 6] + x.i[1] * x.i[4] * x.i[14] - x.i[1] * x.i[12] * x.i[ 6] - x.i[2] * x.i[4] * x.i[13] + x.i[2] * x.i[12] * x.i[ 5];
  t.i[15] =  x.i[0] * x.i[ 5] * x.i[10] - x.i[0] * x.i[ 9] * x.i[ 6] - x.i[1] * x.i[4] * x.i[10] + x.i[1] * x.i[ 8] * x.i[ 6] + x.i[2] * x.i[4] * x.i[ 9] - x.i[2] * x.i[ 8] * x.i[ 5];

  F32 det = x.i[0] * t.i[0] + x.i[4] * t.i[1] + x.i[8] * t.i[2] + x.i[12] * t.i[3];
  ASSERT(det != 0);
  det = 1 / det;

  M4 inverse = {};
  for(int32_t j = 0; j < 16; j++) { inverse.i[j] = t.i[j] * det; }
  return inverse;
}

M4 M4Perspective(F32 fov_y_rad, F32 aspect_ratio, F32 near_plane, F32 far_plane) {
  F32 tan_half_fov_y = F32Tan(fov_y_rad / 2.0f);

  F32 a = 1.0f / (aspect_ratio * tan_half_fov_y);
  F32 b = 1.0f / (tan_half_fov_y);
  F32 c = - (far_plane + near_plane) / (far_plane - near_plane);
  F32 d = - (2 * far_plane * near_plane) / (far_plane - near_plane);

  return (M4) {
    a,  0,  0,  0,
    0,  b,  0,  0,
    0,  0,  c,  d,
    0,  0, -1,  0,
  };
}

M4 M4Orthographic(F32 left, F32 right, F32 bottom, F32 top, F32 near_plane, F32 far_plane) {
  F32 a = 2 / (right - left);
  F32 b = 2 / (top - bottom);
  F32 c = -2 / (far_plane - near_plane);
  F32 d = -(right + left) / (right - left);
  F32 e = -(top + bottom) / (top - bottom);
  F32 f = -(far_plane + near_plane) / (far_plane - near_plane);

  return (M4) {
    a, 0, 0, d,
    0, b, 0, e,
    0, 0, c, f,
    0, 0, 0, 1,
  };
}

M4 LookAt(V3 eye, V3 target, V3 up) {
  V3 z = V3Normalize(V3SubV3(eye, target));
  V3 y = up;
  V3 x = V3CrossV3(y, z);
  y = V3CrossV3(z, x);

  x = V3Normalize(x);
  y = V3Normalize(y);

  F32 a = -V3DotV3(x, eye);
  F32 b = -V3DotV3(y, eye);
  F32 c = -V3DotV3(z, eye);

  return (M4) {
    x.x, x.y, x.z, a,
    y.x, y.y, y.z, b,
    z.x, z.y, z.z, c,
      0,   0,   0, 1,
  };
}

#undef CDEFAULT_MATH_IMPLEMENTATION
#endif // CDEFAULT_MATH_IMPLEMENTATION
