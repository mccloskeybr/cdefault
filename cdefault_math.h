#ifndef CDEFAULT_MATH_H_
#define CDEFAULT_MATH_H_

#include <stdlib.h>
#include <math.h>
#include "cdefault_std.h"

// TODO: color translation funcs?
// TODO: optimize matrix operations
// TODO: in cases where dest cannot equal src, stack allocate a temporary variable instead to allow it?
// TODO: SIMD for matmuls?

typedef union V2 V2;
union V2 {
  struct { F32 x, y; };
  struct { F32 u, v; };
  F32 e[2];
};

typedef union V3 V3;
union V3 {
  struct { F32 x, y, z; };
  struct { F32 r, g, b; };
  F32 e[3];
};

typedef union V4 V4;
union V4 {
  struct { F32 x, y, z, w; };
  struct { F32 r, g, b, a; };
  F32 e[4];
};

typedef union M3 M3;
union M3 {
  F32 e[3][3];
  F32 i[9];
};

typedef union M4 M4;
union M4 {
  F32 e[4][4];
  F32 i[16];
};

///////////////////////////////////////////////////////////////////////////////
// NOTE: Num
///////////////////////////////////////////////////////////////////////////////

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
F32 F32ToRad(F32 deg);
F32 F32ToDeg(F32 rad);
F32 F32Ceil(F32 x);
F32 F32Floor(F32 x);
F32 F32Round(F32 x);

B64 F64ApproxEq(F64 x, F64 y);
F64 F64Abs(F64 x);
F64 F64Pow(F64 x, F64 y);
F64 F64Sqrt(F64 x);
F64 F64Sin(F64 x);
F64 F64Cos(F64 x);
F64 F64Tan(F64 x);
F64 F64ArcSin(F64 x);
F64 F64ArcCos(F64 x);
F64 F64ArcTan(F64 x);
F64 F64ArcTan2(F64 y, F64 x);
F64 F64ToRad(F64 deg);
F64 F64ToDeg(F64 rad);
F64 F64Ceil(F64 x);
F64 F64Floor(F64 x);
F64 F64Round(F64 x);

///////////////////////////////////////////////////////////////////////////////
// NOTE: V2
///////////////////////////////////////////////////////////////////////////////

#define V2_X_POS (V2) {+1,  0}
#define V2_Y_POS (V2) { 0, +1}
#define V2_X_NEG (V2) {-1,  0}
#define V2_Y_NEG (V2) { 0, -1}

#define V2_SWIZZLE(dest, src, a, b) \
  (dest)->x = (src)->a;             \
  (dest)->y = (src)->b;

void V2Splat(V2* dest, F32 c);
void V2AddF32(V2* dest, V2* x, F32 c);
void V2SubF32(V2* dest, V2* x, F32 c);
void V2MultF32(V2* dest, V2* x, F32 c);
void V2DivF32(V2* dest, V2* x, F32 c);
void V2AddV2(V2* dest, V2* x, V2* y);
void V2SubV2(V2* dest, V2* x, V2* y);
void V2HadamardV2(V2* dest, V2* x, V2* y);
F32  V2DotV2(V2* x, V2* y);
F32  V2CrossV2(V2* x, V2* y);
F32  V2InnerMultV2(V2* x, V2* y);
B32  V2Eq(V2* x, V2* y);
B32  V2ApproxEq(V2* x, V2* y);
F32  V2LengthSq(V2* x);
F32  V2Length(V2* x);
void V2Normalize(V2* dest, V2* src);
F32  V2MinValue(V2* x);
F32  V2MaxValue(V2* x);
void V2Project(V2* dest, V2* x, V2* y);
void V2Clamp(V2* dest, V2* src, F32 min, F32 max);
void V2Rotate(V2* dest, V2* src, F32 angle_rad);
void V2Lerp(V2* dest, V2* x, V2* y, F32 t);

///////////////////////////////////////////////////////////////////////////////
// NOTE: V3
///////////////////////////////////////////////////////////////////////////////

#define V3_X_POS (V3) {+1,  0,  0}
#define V3_Y_POS (V3) { 0, +1,  0}
#define V3_Z_POS (V3) { 0,  0, +1}
#define V3_X_NEG (V3) {-1,  0,  0}
#define V3_Y_NEG (V3) { 0, -1,  0}
#define V3_Z_NEG (V3) { 0,  0, -1}

#define V3_BLACK (V3) {0, 0, 0}
#define V3_WHITE (V3) {1, 1, 1}
#define V3_RED   (V3) {1, 0, 0}
#define V3_GREEN (V3) {0, 1, 0}
#define V3_BLUE  (V3) {0, 0, 1}

#define V3_SWIZZLE(dest, src, a, b, c) \
  (dest)->x = (src)->a;                \
  (dest)->y = (src)->b;                \
  (dest)->z = (src)->c;

void V3Splat(V3* dest, F32 c);
void V3FromV2(V3* dest, V2* src);
void V3AddF32(V3* dest, V3* x, F32 c);
void V3SubF32(V3* dest, V3* x, F32 c);
void V3MultF32(V3* dest, V3* x, F32 c);
void V3DivF32(V3* dest, V3* x, F32 c);
void V3AddV3(V3* dest, V3* x, V3* y);
void V3SubV3(V3* dest, V3* x, V3* y);
void V3HadamardV3(V3* dest, V3* x, V3* y);
F32  V3InnerMultV3(V3* x, V3* y);
F32  V3DotV3(V3* x, V3* y);
void V3CrossV3(V3* dest, V3* x, V3* y);
B32  V3ApproxEq(V3* x, V3* y);
B32  V3Eq(V3* x, V3* y);
F32  V3LengthSq(V3* x);
F32  V3Length(V3* x);
void V3Normalize(V3* dest, V3* src);
F32  V3MinValue(V3* x);
F32  V3MaxValue(V3* x);
B32  V3IsBetween(V3* x, V3* start, V3* end);
void V3Project(V3* dest, V3* x, V3* y);
void V3Clamp(V3* dest, V3* src, F32 min, F32 max);
B32  V3AreCollinear(V3* x, V3* y, V3* z);
F32  V3AngleBetween(V3* x, V3* y);
void V3Lerp(V3* dest, V3* x, V3* y, F32 t);
void V3RotateAroundAxis(V3* dest, V3* x, V3* axis, F32 angle_rad);

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

#define V4_SWIZZLE(dest, src, a, b, c, d) \
  (dest)->x = (src)->a;                   \
  (dest)->y = (src)->b;                   \
  (dest)->z = (src)->c;                   \
  (dest)->w = (src)->d;

void V4Splat(V4* dest, F32 c);
void V4FromV2(V4* dest, V2* src);
void V4FromV3(V4* dest, V3* src);
void V4AddF32(V4* dest, V4* x, F32 c);
void V4SubF32(V4* dest, V4* x, F32 c);
void V4MultF32(V4* dest, V4* x, F32 c);
void V4DivF32(V4* dest, V4* x, F32 c);
void V4AddV4(V4* dest, V4* x, V4* y);
void V4SubV4(V4* dest, V4* x, V4* y);
void V4HadamardV4(V4* dest, V4* x, V4* y);
F32  V4InnerMultV4(V4* x, V4* y);
F32  V4DotV4(V4* x, V4* y);
B32  V4ApproxEq(V4* a, V4* b);
B32  V4Eq(V4* x , V4* y);
F32  V4LengthSq(V4* x);
F32  V4Length(V4* x);
void V4Normalize(V4* dest, V4* x);
void V4Project(V4* dest, V4* x, V4* y);
void V4Clamp(V4* dest, V4* src, F32 min, F32 max);
void V4Lerp(V4* dest, V4* a, V4* b, F32 t);
void V4Slerp(V4* dest, V4* a, V4* b, F32 t);
void V4RotateAroundAxis(V4* dest, V3* axis, F32 angle_rad);
void V4LookInDir(V4* dest, V3* dir);
void V4LookAt(V4* dest, V3* pos, V3* target);

///////////////////////////////////////////////////////////////////////////////
// NOTE: M3
///////////////////////////////////////////////////////////////////////////////

#define M3_IDENTITY (M3) {1, 0, 0, \
                          0, 1, 0, \
                          0, 0, 1 }

void M3AddM3(M3* dest, M3* x, M3* y);
void M3SubM3(M3* dest, M3* x, M3* y);
void M3MultF32(M3* dest, F32 c, M3* m);
void M3MultM3(M3* dest, M3* x, M3* y);
void M3MultV3(V3* dest, M3* x, V3* y);
void M3Transpose(M3* dest, M3* x);
F32  M3Det(M3* m);
void M3Invert(M3* dest, M3* x);
void M3FromQuaternion(M3* dest, V4* q);
B32  M3ApproxEq(M3* x, M3* y);
B32  M3Eq(M3* x, M3* y);

///////////////////////////////////////////////////////////////////////////////
// NOTE: M4
///////////////////////////////////////////////////////////////////////////////

#define M4_IDENTITY (M4) {1, 0, 0, 0, \
                          0, 1, 0, 0, \
                          0, 0, 1, 0, \
                          0, 0, 0, 1}

void M4Perspective(M4* dest, F32 fov_y_rad, F32 aspect_ratio, F32 near_plane, F32 far_plane);
void M4Orthographic(M4* dest, F32 left, F32 right, F32 bottom, F32 top, F32 near_plane, F32 far_plane);
void M4LookAt(M4* dest, V3* eye, V3* target, V3* up);
void M4FromTransform(M4* dest, V3* pos, V4* rot, V3* scale);
void M4AddM4(M4* dest, M4* x, M4* y);
void M4SubM4(M4* dest, M4* x, M4* y);
void M4MultF32(M4* dest, F32 c,  M4* m);
void M4MultM4(M4* dest, M4* x, M4* y);
void M4MultV4(V4* dest, M4* x, V4* y);
void M4Transpose(M4* dest, M4* x);
F32  M4Det(M4* m);
void M4Invert(M4* dest, M4* x);
B32  M4ApproxEq(M4* x, M4* y);
B32  M4Eq(M4* x, M4* y);

#endif // CDEFAULT_MATH_H_

#ifdef CDEFAULT_MATH_IMPLEMENTATION
#undef CDEFAULT_MATH_IMPLEMENTATION

///////////////////////////////////////////////////////////////////////////////
// NOTE: Num implementation
///////////////////////////////////////////////////////////////////////////////

B32 F32ApproxEq(F32 x, F32 y) { return F32Abs(x - y) < 0.00001f; }
F32 F32Abs(F32 x) { return fabsf(x); }
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
F32 F32ToRad(F32 deg) { return deg * (F32_PI / 180.0f); }
F32 F32ToDeg(F32 rad) { return rad * (180.0f / F32_PI); }
F32 F32Ceil(F32 x) { return ceilf(x); }
F32 F32Floor(F32 x) { return floorf(x); }
F32 F32Round(F32 x) { return F32Floor(x + 0.5f); }

B64 F64ApproxEq(F64 x, F64 y) { return F64Abs(x - y) < 0.00001; }
F64 F64Abs(F64 x) { return fabs(x); }
F64 F64Pow(F64 x, F64 y) { return pow(x, y); }
F64 F64Sqrt(F64 x) { return sqrt(x); }
F64 F64Sin(F64 x) { return sin(x); }
F64 F64Cos(F64 x) { return cos(x); }
F64 F64Tan(F64 x) { return tan(x); }
F64 F64ArcSin(F64 x) { return asin(x); }
F64 F64ArcCos(F64 x) { return acos(x); }
F64 F64ArcTan(F64 x) { return atan(x); }
F64 F64ArcTan2(F64 y, F64 x) { return atan2(y, x); }
F64 F64ToRad(F64 deg) { return deg * (F64_PI / 180.0); }
F64 F64ToDeg(F64 rad) { return rad * (180.0 / F64_PI); }
F64 F64Ceil(F64 x) { return ceil(x); }
F64 F64Floor(F64 x) { return floor(x); }
F64 F64Round(F64 x) { return F64Floor(x + 0.5); }

///////////////////////////////////////////////////////////////////////////////
// NOTE: V2 implementation
///////////////////////////////////////////////////////////////////////////////

void V2Splat(V2* dest, F32 c) {
  dest->x = c;
  dest->y = c;
}

void V2AddF32(V2* dest, V2* x, F32 c) {
  dest->x = x->x + c;
  dest->y = x->y + c;
}

void V2SubF32(V2* dest, V2* x, F32 c) {
  dest->x = x->x - c;
  dest->y = x->y - c;
}

void V2MultF32(V2* dest, V2* x, F32 c) {
  dest->x = x->x * c;
  dest->y = x->y * c;
}

void V2DivF32(V2* dest, V2* x, F32 c) {
  DEBUG_ASSERT(c != 0);
  dest->x = x->x / c;
  dest->y = x->y / c;
}

void V2AddV2(V2* dest, V2* x, V2* y) {
  dest->x = x->x + y->x;
  dest->y = x->y + y->y;
}

void V2SubV2(V2* dest, V2* x, V2* y) {
  dest->x = x->x - y->x;
  dest->y = x->y - y->y;
}

void V2HadamardV2(V2* dest, V2* x, V2* y) {
  dest->x = x->x * y->x;
  dest->y = x->y * y->y;
}

F32 V2DotV2(V2* x, V2* y) {
  return (x->x * y->x) + (x->y * y->y);
}

F32 V2CrossV2(V2* x, V2* y) {
  return (x->x * y->y) - (x->y * y->x);
}

F32 V2InnerMultV2(V2* x, V2* y) {
  return V2DotV2(x, y);
}

B32 V2Eq(V2* x, V2* y) {
  return (x->x == y->x) &&
         (x->y == y->y);
}

B32 V2ApproxEq(V2* x, V2* y) {
  return F32ApproxEq(x->x, y->x) &&
         F32ApproxEq(x->y, y->y);
}

F32 V2LengthSq(V2* x) {
  return V2DotV2(x, x);
}

F32 V2Length(V2* x) {
  return F32Sqrt(V2LengthSq(x));
}

void V2Normalize(V2* dest, V2* x) {
  V2DivF32(dest, x, V2Length(x));
}

F32 V2MinValue(V2* x) {
  return MIN(x->x, x->y);
}

F32 V2MaxValue(V2* x) {
  return MAX(x->x, x->y);
}

void V2Clamp(V2* dest, V2* src, F32 min, F32 max) {
  dest->x = CLAMP(min, src->x, max);
  dest->y = CLAMP(min, src->y, max);
}

void V2Project(V2* dest, V2* x, V2* y) {
  F32 ratio = V2DotV2(x, y) / V2DotV2(y, y);
  V2MultF32(dest, y, ratio);
}
 
void V2Rotate(V2* dest, V2* src, F32 angle_rad) {
  DEBUG_ASSERT(dest != src);
  F32 s = F32Sin(angle_rad);
  F32 c = F32Cos(angle_rad);
  dest->x = src->x * c - src->y * s;
  dest->y = src->x * c + src->y * s;
}

void V2Lerp(V2* dest, V2* x, V2* y, F32 t) {
  dest->x = F32Lerp(x->x, y->x, t);
  dest->y = F32Lerp(x->y, y->y, t);
}

///////////////////////////////////////////////////////////////////////////////
// NOTE: V3 implementation
///////////////////////////////////////////////////////////////////////////////

void V3Splat(V3* dest, F32 c) {
  dest->x = c;
  dest->y = c;
  dest->z = c;
}

void V3FromV2(V3* dest, V2* src) {
  dest->x = src->x;
  dest->y = src->y;
  dest->z = 0;
}

void V3AddF32(V3* dest, V3* x, F32 c) {
  dest->x = x->x + c;
  dest->y = x->y + c;
  dest->z = x->z + c;
}

void V3SubF32(V3* dest, V3* x, F32 c) {
  dest->x = x->x - c;
  dest->y = x->y - c;
  dest->z = x->z - c;
}

void V3MultF32(V3* dest, V3* x, F32 c) {
  dest->x = x->x * c;
  dest->y = x->y * c;
  dest->z = x->z * c;
}

void V3DivF32(V3* dest, V3* x, F32 c) {
  ASSERT(c != 0);
  dest->x = x->x / c;
  dest->y = x->y / c;
  dest->z = x->z / c;
}

void V3AddV3(V3* dest, V3* x, V3* y) {
  dest->x = x->x + y->x;
  dest->y = x->y + y->y;
  dest->z = x->z + y->z;
}

void V3SubV3(V3*dest, V3* x, V3* y) {
  dest->x = x->x - y->x;
  dest->y = x->y - y->y;
  dest->z = x->z - y->z;
}

void V3HadamardV3(V3* dest, V3* x, V3* y) {
  dest->x = x->x * y->x;
  dest->y = x->y * y->y;
  dest->z = x->z * y->z;
}

F32 V3InnerMultV3(V3* x, V3* y) {
  return V3DotV3(x, y);
}

F32 V3DotV3(V3* x, V3* y) {
  return (x->x * y->x) + (x->y * y->y) + (x->z * y->z);
}

void V3CrossV3(V3* dest, V3* x, V3* y) {
  DEBUG_ASSERT(x != dest);
  DEBUG_ASSERT(y != dest);
  dest->x = x->y * y->z - x->z * y->y;
  dest->y = x->z * y->x - x->x * y->z;
  dest->z = x->x * y->y - x->y * y->x;
}

B32 V3ApproxEq(V3* x, V3* y) {
  return F32ApproxEq(x->x, y->x) &&
         F32ApproxEq(x->y, y->y) &&
         F32ApproxEq(x->z, y->z);
}

B32 V3Eq(V3* x, V3* y) {
  return x->x == y->x &&
         x->y == y->y &&
         x->z == y->z;
}

F32 V3LengthSq(V3* x) {
  return V3DotV3(x, x);
}

F32 V3Length(V3* x) {
  return F32Sqrt(V3LengthSq(x));
}

void V3Normalize(V3* dest, V3* src) {
  V3DivF32(dest, src, V3Length(src));
}

F32 V3MinValue(V3* x) {
  return MIN3(x->x, x->y, x->z);
}

F32 V3MaxValue(V3* x) {
  return MAX3(x->x, x->y, x->z);
}

B32 V3IsBetween(V3* x, V3* start, V3* end) {
  V3* i = start;
  V3* j = end;
  V3* k = x;
  if (!V3AreCollinear(i, j, k)) { return false; }
  V3 ij, ik;
  V3SubV3(&ij, j, i);
  V3SubV3(&ik, k, i);
  F32 ij_dot_ik = V3DotV3(&ij, &ik);
  F32 ij_dot_ij = V3DotV3(&ij, &ij);
  return 0 < ij_dot_ik && ij_dot_ik < ij_dot_ij;
}

// https://en.wikibooks.org/wiki/Linear_Algebra/Orthogonal_Projection_Onto_a_Line
void V3Project(V3* dest, V3* x, V3* y) {
  F32 ratio = V3DotV3(x, y) / V3DotV3(y, y);
  V3MultF32(dest, y, ratio);
}

void V3Clamp(V3* dest, V3* src, F32 min, F32 max) {
  dest->x = CLAMP(min, src->x, max);
  dest->y = CLAMP(min, src->y, max);
  dest->z = CLAMP(min, src->z, max);
}

B32 V3AreCollinear(V3* x, V3* y, V3* z) {
  V3 xy, xz, xy_cross_xz;
  V3SubV3(&xy, y, x);
  V3SubV3(&xz, z, x);
  V3CrossV3(&xy_cross_xz, &xy, &xz);
  V3 zeros = { 0, 0, 0 };
  return V3Eq(&xy_cross_xz, &zeros);
}

F32 V3AngleBetween(V3* x, V3* y) {
  return F32ArcCos(V3DotV3(x, y));
}

void V3Lerp(V3* dest, V3* x, V3* y, F32 t) {
  dest->x = F32Lerp(x->x, y->x, t);
  dest->y = F32Lerp(x->y, y->y, t);
  dest->z = F32Lerp(x->z, y->z, t);
}

// https://stackoverflow.com/questions/69245724/rotate-a-vector-around-an-axis-in-3d-space
void V3RotateAroundAxis(V3* dest, V3* v, V3* axis, F32 angle_rad) {
  V3 cross, double_cross;
  V3CrossV3(&cross, axis, v);
  V3CrossV3(&double_cross, axis, &cross);
  V3 a, b;
  V3MultF32(&a, &cross, F32Sin(angle_rad));
  V3MultF32(&b, &double_cross, (1.0f - F32Cos(angle_rad)));
  dest->x = v->x + a.x + b.x;
  dest->y = v->y + a.y + b.y;
  dest->z = v->z + a.z + b.z;
}

///////////////////////////////////////////////////////////////////////////////
// NOTE: V4 implementation
///////////////////////////////////////////////////////////////////////////////

void V4Splat(V4* dest, F32 c) {
  dest->x = c;
  dest->y = c;
  dest->z = c;
  dest->w = c;
}

void V4FromV2(V4* dest, V2* src) {
  dest->x = src->x;
  dest->y = src->y;
  dest->z = 0;
  dest->w = 0;
}

void V4FromV3(V4* dest, V3* src) {
  dest->x = src->x;
  dest->y = src->y;
  dest->z = src->z;
  dest->w = 0;
}

void V4AddV4(V4* dest, V4* x, V4* y) {
  dest->x = x->x + y->x;
  dest->y = x->y + y->y;
  dest->z = x->z + y->z;
  dest->w = x->w + y->w;
}

void V4AddF32(V4* dest, V4* x, F32 c) {
  dest->x = x->x + c;
  dest->y = x->y + c;
  dest->z = x->z + c;
  dest->w = x->w + c;
}

void V4MultF32(V4* dest, V4* x, F32 c) {
  dest->x = x->x * c;
  dest->y = x->y * c;
  dest->z = x->z * c;
  dest->w = x->w * c;
}

void V4DivF32(V4* dest, V4* x, F32 c) {
  dest->x = x->x / c;
  dest->y = x->y / c;
  dest->z = x->z / c;
  dest->w = x->w / c;
}

void V4SubF32(V4* dest, V4* x, F32 c) {
  dest->x = x->x - c;
  dest->y = x->y - c;
  dest->z = x->z - c;
  dest->w = x->w - c;
}

void V4SubV4(V4* dest, V4* x, V4* y) {
  dest->x = x->x - y->x;
  dest->y = x->y - y->y;
  dest->z = x->z - y->z;
  dest->w = x->w - y->w;
}

F32 V4LengthSq(V4* x) {
  return V4DotV4(x, x);
}

F32 V4Length(V4* x) {
  return F32Sqrt(V4LengthSq(x));
}

void V4Hadamard(V4* dest, V4* x, V4* y) {
  dest->x = x->x * y->x;
  dest->y = x->y * y->y;
  dest->z = x->z * y->z;
  dest->w = x->w * y->w;
}

F32 V4InnerMultV4(V4* x, V4* y) {
  return V4DotV4(x, y);
}

F32 V4DotV4(V4* x, V4* y) {
  return (x->x * y->x) + (x->y * y->y) + (x->z * y->z) + (x->w * y->w);
}

B32 V4Eq(V4* x , V4* y) {
  return x->x == y->x && x->y == y->y && x->z == y->z && x->w == y->w;
}

B32 V4ApproxEq(V4* x, V4* y) {
  return F32ApproxEq(x->x, y->x) &&
         F32ApproxEq(x->y, y->y) &&
         F32ApproxEq(x->z, y->z) &&
         F32ApproxEq(x->w, y->w);
}

void V4Normalize(V4* dest, V4* x) {
  V4DivF32(dest, x, V4Length(x));
}

void V4Project(V4* dest, V4* x, V4* y) {
  F32 ratio = V4DotV4(x, y) / V4DotV4(y, y);
  V4MultF32(dest, y, ratio);
}

void V4Clamp(V4* dest, V4* src, F32 min, F32 max) {
  dest->x = CLAMP(min, src->x, max);
  dest->y = CLAMP(min, src->y, max);
  dest->z = CLAMP(min, src->z, max);
  dest->w = CLAMP(min, src->w, max);
}

void V4Lerp(V4* dest, V4* a, V4* b, F32 t) {
  dest->x = F32Lerp(a->x, b->x, t);
  dest->y = F32Lerp(a->y, b->y, t);
  dest->z = F32Lerp(a->z, b->z, t);
  dest->w = F32Lerp(a->w, b->w, t);
}

void V4Slerp(V4* dest, V4* a, V4* b, F32 t) {
  V4 a_copy = *a;
  V4 b_copy = *b;
  F32 dot = V4DotV4(&a_copy, &b_copy);
  if (dot < 0) {
    dot = -dot;
    V4MultF32(&a_copy, a, -1);
  }

  if (UNLIKELY(F32ApproxEq(dot, 1))) {
    V4Lerp(dest, &a_copy, &b_copy, t);
    return;
  }

  F32 angle_rad = F32ArcCos(dot);
  F32 a_scalar = F32Sin((1.0f - t) * angle_rad) / F32Sin(angle_rad);
  F32 b_scalar = F32Sin(t * angle_rad) / F32Sin(angle_rad);

  V4MultF32(&a_copy, &a_copy, a_scalar);
  V4MultF32(&b_copy, &b_copy, b_scalar);
  V4AddV4(dest, &a_copy, &b_copy);
}

void V4RotateAroundAxis(V4* dest, V3* axis, F32 angle_rad) {
  F32 factor = F32Sin(angle_rad / 2.0f);
  dest->x = factor * axis->x;
  dest->y = factor * axis->y;
  dest->z = factor * axis->z;
  dest->w = F32Cos(angle_rad / 2.0f);
  V4Normalize(dest, dest);
}

// https://stackoverflow.com/questions/12435671/quaternion-lookat-function
void V4LookInDir(V4* dest, V3* dir) {
  ASSERT(F32ApproxEq(V3LengthSq(dir), 1.0f));
  F32 dot = V3DotV3(&V3_Y_NEG, dir);
  if (F32ApproxEq(dot, -1)) {
    // TODO: this is probably wrong?
    *dest = (V4) {0, 0, 1, 0};
    return;
  } else if (F32ApproxEq(dot, +1)) {
    *dest = V4_QUAT_IDENT;
    return;
  }

  V3 axis;
  V3CrossV3(&axis, &V3_Y_NEG, dir);
  V3Normalize(&axis, &axis);

  F32 angle = F32ArcCos(dot);
  F32 half_sin = F32Sin(angle / 2.0f);
  F32 half_cos = F32Cos(angle / 2.0f);
  dest->x = axis.x * half_sin;
  dest->y = axis.y * half_sin;
  dest->z = axis.z * half_sin;
  dest->w = half_cos;
}

void V4LookAt(V4* dest, V3* pos, V3* target) {
  V3 dir;
  V3SubV3(&dir, target, pos);
  V3Normalize(&dir, &dir);
  V4LookInDir(dest, &dir);
}

///////////////////////////////////////////////////////////////////////////////
// NOTE: M3 implementation
///////////////////////////////////////////////////////////////////////////////

void M3AddM3(M3* dest, M3* x, M3* y) {
  for (U32 i = 0; i < 3; i++) {
    for (U32 j = 0; j < 3; j++) {
      dest->e[i][j] = x->e[i][j] + y->e[i][j];
    }
  }
}

void M3SubM3(M3* dest, M3* x, M3* y) {
  for (U32 i = 0; i < 3; i++) {
    for (U32 j = 0; j < 3; j++) {
      dest->e[i][j] = x->e[i][j] - y->e[i][j];
    }
  }
}

void M3MultF32(M3* dest, F32 c, M3* m) {
  for (U32 i = 0; i < 3; i++) {
    for (U32 j = 0; j < 3; j++) {
      dest->e[i][j] = m->e[i][j] * c;
    }
  }
}

void M3MultM3(M3* dest, M3* x, M3* y) {
  DEBUG_ASSERT(x != dest);
  DEBUG_ASSERT(y != dest);
  M3 y_t;
  M3Transpose(&y_t, y);
  for (U32 i = 0; i < 3; i++) {
    for (U32 j = 0; j < 3; j++) {
      V3* x_sub = (V3*) &x->e[i][0];
      V3* y_sub = (V3*) &y_t.e[j][0];
      dest->e[i][j] = V3DotV3(x_sub, y_sub);
    }
  }
}

void M3MultV3(V3* dest, M3* x, V3* y) {
  DEBUG_ASSERT(y != dest);
  for (U32 i = 0; i < 3; i++) {
    V3* x_sub = (V3*) &x->e[i][0];
    dest->e[i] = V3DotV3(x_sub, y);
  }
}

void M3Transpose(M3* dest, M3* x) {
  DEBUG_ASSERT(x != dest);
  for (U32 i = 0; i < 3; i++) {
    for (U32 j = 0; j < 3; j++) {
      dest->e[j][i] = x->e[i][j];
    }
  }
}

// https://www.chilimath.com/lessons/advanced-algebra/determinant-3x3-matrix/
F32 M3Det(M3* m) {
  F32 sub_0 = (m->e[1][1] * m->e[2][2]) - (m->e[1][2] * m->e[2][1]);
  F32 sub_1 = (m->e[1][0] * m->e[2][2]) - (m->e[1][2] * m->e[2][0]);
  F32 sub_2 = (m->e[1][0] * m->e[2][1]) - (m->e[1][1] * m->e[2][0]);
  return (m->e[0][0] * sub_0) - (m->e[0][1] * sub_1) + (m->e[0][2] * sub_2);
}

void M3Invert(M3* dest, M3* m) {
  M3 adj;
  adj.e[0][0] = +((m->e[1][1] * m->e[2][2]) - (m->e[1][2] * m->e[2][1]));
  adj.e[1][0] = -((m->e[1][0] * m->e[2][2]) - (m->e[1][2] * m->e[2][0]));
  adj.e[2][0] = +((m->e[1][0] * m->e[2][1]) - (m->e[1][1] * m->e[2][0]));
  adj.e[0][1] = -((m->e[0][1] * m->e[2][2]) - (m->e[0][2] * m->e[2][1]));
  adj.e[1][1] = +((m->e[0][0] * m->e[2][2]) - (m->e[0][2] * m->e[2][0]));
  adj.e[2][1] = -((m->e[0][0] * m->e[2][1]) - (m->e[0][1] * m->e[2][0]));
  adj.e[0][2] = +((m->e[0][1] * m->e[1][2]) - (m->e[0][2] * m->e[1][1]));
  adj.e[1][2] = -((m->e[0][0] * m->e[1][2]) - (m->e[0][2] * m->e[1][0]));
  adj.e[2][2] = +((m->e[0][0] * m->e[1][1]) - (m->e[0][1] * m->e[1][0]));

  // NOTE: reuse cofactor calcs from adj mat.
  F32 det = (m->e[0][0] * adj.e[0][0]) + (m->e[0][1] * adj.e[1][0]) + (m->e[0][2] * adj.e[2][0]);
  DEBUG_ASSERT(det != 0);
  F32 inv_det = 1.0f / det;
  for (S32 i = 0; i < 9; i++) { dest->i[i] = inv_det * adj.i[i]; }
}

// https://automaticaddison.com/how-to-convert-a-quaternion-to-a-rotation-matrix/
void M3FromQuaternion(M3* dest, V4* q) {
  dest->e[0][0] = 2.0f*(q->w*q->w + q->x*q->x) - 1.0f;
  dest->e[0][1] = 2.0f*(q->x*q->y - q->w*q->z);
  dest->e[0][2] = 2.0f*(q->x*q->z + q->w*q->y);
  dest->e[1][0] = 2.0f*(q->x*q->y + q->w*q->z);
  dest->e[1][1] = 2.0f*(q->w*q->w + q->y*q->y) - 1.0f;
  dest->e[1][2] = 2.0f*(q->y*q->z - q->w*q->x);
  dest->e[2][0] = 2.0f*(q->x*q->z - q->w*q->y);
  dest->e[2][1] = 2.0f*(q->y*q->z + q->w*q->x);
  dest->e[2][2] = 2.0f*(q->w*q->w + q->z*q->z) - 1.0f;
}

B32 M3ApproxEq(M3* x, M3* y) {
  for (U32 i = 0; i < 9; i++) {
    if (!F32ApproxEq(x->i[i], y->i[i])) { return false; }
  }
  return true;
}

B32 M3Eq(M3* x, M3* y) {
  for (U32 i = 0; i < 9; i++) {
    if (x->i[i] != y->i[i]) { return false; }
  }
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// NOTE: M4 implementation
///////////////////////////////////////////////////////////////////////////////

// TODO: faster way to do this?
void M4FromTransform(M4* dest, V3* pos, V4* rot, V3* scale) {
  M3 scale_m;
  MEMORY_ZERO_STRUCT(&scale_m);
  scale_m.e[0][0] = scale->x;
  scale_m.e[1][1] = scale->y;
  scale_m.e[2][2] = scale->z;

  M3 rot_m;
  M3FromQuaternion(&rot_m, rot);

  M3 rot_scale;
  M3MultM3(&rot_scale, &rot_m, &scale_m);

  MEMORY_ZERO_STRUCT(dest);
  dest->e[0][0] = rot_scale.e[0][0];
  dest->e[0][1] = rot_scale.e[0][1];
  dest->e[0][2] = rot_scale.e[0][2];
  dest->e[1][0] = rot_scale.e[1][0];
  dest->e[1][1] = rot_scale.e[1][1];
  dest->e[1][2] = rot_scale.e[1][2];
  dest->e[2][0] = rot_scale.e[2][0];
  dest->e[2][1] = rot_scale.e[2][1];
  dest->e[2][2] = rot_scale.e[2][2];

  dest->e[0][3] = pos->x;
  dest->e[1][3] = pos->y;
  dest->e[2][3] = pos->z;
  dest->e[3][3] = 1;
}

void M4Perspective(M4* dest, F32 fov_y_rad, F32 aspect_ratio, F32 near_plane, F32 far_plane) {
  F32 tan_half_fov_y = F32Tan(fov_y_rad / 2.0f);

  F32 a = 1.0f / (aspect_ratio * tan_half_fov_y);
  F32 b = 1.0f / (tan_half_fov_y);
  F32 c = - (far_plane + near_plane) / (far_plane - near_plane);
  F32 d = - (2 * far_plane * near_plane) / (far_plane - near_plane);

  *dest = (M4) {
    a,  0,  0,  0,
    0,  b,  0,  0,
    0,  0,  c,  d,
    0,  0, -1,  0,
  };
}

void M4Orthographic(M4* dest, F32 left, F32 right, F32 bottom, F32 top, F32 near_plane, F32 far_plane) {
  F32 a = 2 / (right - left);
  F32 b = 2 / (top - bottom);
  F32 c = -2 / (far_plane - near_plane);
  F32 d = -(right + left) / (right - left);
  F32 e = -(top + bottom) / (top - bottom);
  F32 f = -(far_plane + near_plane) / (far_plane - near_plane);

  *dest = (M4) {
    a, 0, 0, d,
    0, b, 0, e,
    0, 0, c, f,
    0, 0, 0, 1,
  };
}

void M4LookAt(M4* dest, V3* eye, V3* target, V3* up) {
  V3 x, y, z;
  V3SubV3(&z, eye, target);
  V3Normalize(&z, &z);
  y = *up;
  V3CrossV3(&x, &y, &z);
  V3CrossV3(&y, &z, &x);

  V3Normalize(&x, &x);
  V3Normalize(&y, &y);

  F32 a = -V3DotV3(&x, eye);
  F32 b = -V3DotV3(&y, eye);
  F32 c = -V3DotV3(&z, eye);

  *dest = (M4) {
    x.x, x.y, x.z, a,
    y.x, y.y, y.z, b,
    z.x, z.y, z.z, c,
      0,   0,   0, 1,
  };
}

void M4AddM4(M4* dest, M4* x, M4* y) {
  for (U32 i = 0; i < 4; i++) {
    for (U32 j = 0; j < 4; j++) {
      dest->e[i][j] = x->e[i][j] + y->e[i][j];
    }
  }
}

void M4SubM4(M4* dest, M4* x, M4* y) {
  for (U32 i = 0; i < 4; i++) {
    for (U32 j = 0; j < 4; j++) {
      dest->e[i][j] = x->e[i][j] - y->e[i][j];
    }
  }
}

void M4MultF32(M4* dest, F32 c, M4* m) {
  for (U32 i = 0; i < 4; i++) {
    for (U32 j = 0; j < 4; j++) {
      dest->e[i][j] = m->e[i][j] * c;
    }
  }
}

void M4MultM4(M4* dest, M4* x, M4* y) {
  DEBUG_ASSERT(x != dest);
  DEBUG_ASSERT(y != dest);
  M4 y_t;
  M4Transpose(&y_t, y);
  for (int32_t i = 0; i < 4; i++) {
    for (int32_t j = 0; j < 4; j++) {
      V4* x_sub = (V4*) &x->e[i][0];
      V4* y_sub = (V4*) &y_t.e[j][0];
      dest->e[i][j] = V4DotV4(x_sub, y_sub);
    }
  }
}

void M4MultV4(V4* dest, M4* x, V4* y) {
  DEBUG_ASSERT(y != dest);
  for (int32_t j = 0; j < 4; j++) {
    V4* x_sub = (V4*) &x->e[j][0];
    dest->e[j] = V4DotV4(x_sub, y);
  }
}

void M4Transpose(M4* dest, M4* x) {
  DEBUG_ASSERT(x != dest);
  for (U32 i = 0; i < 4; i++) {
    for (U32 j = 0; j < 4; j++) {
      dest->e[j][i] = x->e[i][j];
    }
  }
}

// https://byjus.com/maths/determinant-of-4x4-matrix/
F32 M4Det(M4* m) {
  F32 sub_00 = (m->e[2][2] * m->e[3][3]) - (m->e[2][3] * m->e[3][2]);
  F32 sub_01 = (m->e[2][1] * m->e[3][3]) - (m->e[2][3] * m->e[3][1]);
  F32 sub_02 = (m->e[2][1] * m->e[3][2]) - (m->e[2][2] * m->e[3][1]);
  F32 det_0  = (m->e[1][1] * sub_00) - (m->e[1][2] * sub_01) + (m->e[1][3] * sub_02);
  F32 sub_10 = (m->e[2][2] * m->e[3][3]) - (m->e[2][3] * m->e[3][2]);
  F32 sub_11 = (m->e[2][0] * m->e[3][3]) - (m->e[2][3] * m->e[3][0]);
  F32 sub_12 = (m->e[2][0] * m->e[3][2]) - (m->e[2][2] * m->e[3][0]);
  F32 det_1  = (m->e[1][0] * sub_10) - (m->e[1][2] * sub_11) + (m->e[1][3] * sub_12);
  F32 sub_20 = (m->e[2][1] * m->e[3][3]) - (m->e[2][3] * m->e[3][1]);
  F32 sub_21 = (m->e[2][0] * m->e[3][3]) - (m->e[2][3] * m->e[3][0]);
  F32 sub_22 = (m->e[2][0] * m->e[3][1]) - (m->e[2][1] * m->e[3][0]);
  F32 det_2  = (m->e[1][0] * sub_20) - (m->e[1][1] * sub_21) + (m->e[1][3] * sub_22);
  F32 sub_30 = (m->e[2][1] * m->e[3][2]) - (m->e[2][2] * m->e[3][1]);
  F32 sub_31 = (m->e[2][0] * m->e[3][2]) - (m->e[2][2] * m->e[3][0]);
  F32 sub_32 = (m->e[2][0] * m->e[3][1]) - (m->e[2][1] * m->e[3][0]);
  F32 det_3  = (m->e[1][0] * sub_30) - (m->e[1][1] * sub_31) + (m->e[1][2] * sub_32);
  return ((m->e[0][0] * det_0) - (m->e[0][1] * det_1) + (m->e[0][2] * det_2) - (m->e[0][3] * det_3));
}

// http://rodolphe-vaillant.fr/entry/7/c-code-for-4x4-matrix-inversion
void M4Invert(M4* dest, M4* x) {
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
  DEBUG_ASSERT(det != 0);
  det = 1 / det;
  for(int32_t j = 0; j < 16; j++) { dest->i[j] = t.i[j] * det; }
}

B32 M4ApproxEq(M4* x, M4* y) {
  for (U32 i = 0; i < 16; i++) {
    if (!F32ApproxEq(x->i[i], y->i[i])) { return false; }
  }
  return true;
}

B32 M4Eq(M4* x, M4* y) {
  for (U32 i = 0; i < 16; i++) {
    if (x->i[i] != y->i[i]) { return false; }
  }
  return true;
}


#endif // CDEFAULT_MATH_IMPLEMENTATION
