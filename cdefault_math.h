#ifndef CDEFAULT_MATH_H_
#define CDEFAULT_MATH_H_

#include "cdefault_std.h"

#include <math.h>
#include <stdlib.h>

// TODO: color translation funcs?
// TODO: easing funcs?

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

typedef union M2 M2;
union M2 {
  F32 e[2][2];
  F32 i[4];
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
F32 F32MapRange(F32 x, F32 a_min, F32 a_max, F32 b_min, F32 b_max);
F32 F32Lerp(F32 x, F32 y, F32 t);
B32 F32IsInteger(F32 x);

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
F64 F64MapRange(F64 x, F64 a_min, F64 a_max, F64 b_min, F64 b_max);
F64 F64Lerp(F64 x, F64 y, F64 t);
B32 F64IsInteger(F64 x);

///////////////////////////////////////////////////////////////////////////////
// NOTE: V2
///////////////////////////////////////////////////////////////////////////////

#define V2_ZEROES (V2) { 0,  0 }
#define V2_ONES   (V2) { 1,  1 }

#define V2_X_POS  (V2) {+1,  0}
#define V2_Y_POS  (V2) { 0, +1}
#define V2_X_NEG  (V2) {-1,  0}
#define V2_Y_NEG  (V2) { 0, -1}

V2  V2Assign(F32 x, F32 y);
V2  V2Splat(F32 c);
V2* V2AddF32(V2* dest, V2* x, F32 c);
V2* V2SubF32(V2* dest, V2* x, F32 c);
V2* V2MultF32(V2* dest, V2* x, F32 c);
V2* V2DivF32(V2* dest, V2* x, F32 c);
V2* V2AddV2(V2* dest, V2* x, V2* y);
V2* V2SubV2(V2* dest, V2* x, V2* y);
V2* V2HadamardV2(V2* dest, V2* x, V2* y);
F32 V2DotV2(V2* x, V2* y);
F32 V2CrossV2(V2* x, V2* y);
F32 V2InnerMultV2(V2* x, V2* y);
B32 V2Eq(V2* x, V2* y);
B32 V2ApproxEq(V2* x, V2* y);
F32 V2LengthSq(V2* x);
F32 V2Length(V2* x);
V2* V2Abs(V2* dest, V2* src);
V2* V2Normalize(V2* dest, V2* src);
V2* V2Negate(V2* dest, V2* src);
F32 V2MinValue(V2* x);
F32 V2MaxValue(V2* x);
V2* V2Project(V2* dest, V2* x, V2* y);
V2* V2Clamp(V2* dest, V2* src, F32 min, F32 max);
V2* V2Rotate(V2* dest, V2* src, F32 angle_rad);
V2* V2Lerp(V2* dest, V2* x, V2* y, F32 t);

///////////////////////////////////////////////////////////////////////////////
// NOTE: V3
///////////////////////////////////////////////////////////////////////////////

#define V3_ZEROES (V3) { 0,  0,  0}
#define V3_ONES   (V3) { 1,  1,  1}
#define V3_X_POS  (V3) {+1,  0,  0}
#define V3_Y_POS  (V3) { 0, +1,  0}
#define V3_Z_POS  (V3) { 0,  0, +1}
#define V3_X_NEG  (V3) {-1,  0,  0}
#define V3_Y_NEG  (V3) { 0, -1,  0}
#define V3_Z_NEG  (V3) { 0,  0, -1}

#define V3_BLACK  (V3) {0, 0, 0}
#define V3_WHITE  (V3) {1, 1, 1}
#define V3_RED    (V3) {1, 0, 0}
#define V3_GREEN  (V3) {0, 1, 0}
#define V3_BLUE   (V3) {0, 0, 1}

#define V3_MOONFLY_BLACK        V3FromHex(0x080808)
#define V3_MOONFLY_WHITE        V3FromHex(0xeeeeee)
#define V3_MOONFLY_DARK_GRAY    V3FromHex(0x323437)
#define V3_MOONFLY_GRAY         V3FromHex(0x9e9e9e)
#define V3_MOONFLY_LIGHT_GRAY   V3FromHex(0xbdbdbd)
#define V3_MOONFLY_BLUE         V3FromHex(0x80a0ff)
// #define V3_MOONFLY_LIGHT_BLUE   V3FromHex(0xb2ceee)
#define V3_MOONFLY_LIGHT_BLUE   V3FromHex(0x99b7f6)
#define V3_MOONFLY_RED          V3FromHex(0xff5d5d)
#define V3_MOONFLY_LIGHT_RED    V3FromHex(0xff5189)
#define V3_MOONFLY_GREEN        V3FromHex(0x8cc85f)
#define V3_MOONFLY_LIGHT_GREEN  V3FromHex(0x36c692)
#define V3_MOONFLY_YELLOW       V3FromHex(0xe3c78a)
#define V3_MOONFLY_LIGHT_YELLOW V3FromHex(0xc6c684)
#define V3_MOONFLY_PURPLE       V3FromHex(0xcf87e8)
#define V3_MOONFLY_LIGHT_PURPLE V3FromHex(0xae81ff)
#define V3_MOONFLY_CYAN         V3FromHex(0x79dac8)
#define V3_MOONFLY_LIGHT_CYAN   V3FromHex(0x85dc85)

V3  V3Assign(F32 x, F32 y, F32 z);
V3  V3Splat(F32 c);
V3  V3FromV2(V2* src, F32 z);
V3  V3FromHex(U32 x);
V3* V3AddF32(V3* dest, V3* x, F32 c);
V3* V3SubF32(V3* dest, V3* x, F32 c);
V3* V3MultF32(V3* dest, V3* x, F32 c);
V3* V3DivF32(V3* dest, V3* x, F32 c);
V3* V3AddV3(V3* dest, V3* x, V3* y);
V3* V3SubV3(V3* dest, V3* x, V3* y);
V3* V3HadamardV3(V3* dest, V3* x, V3* y);
F32 V3InnerMultV3(V3* x, V3* y);
F32 V3DotV3(V3* x, V3* y);
V3* V3CrossV3(V3* dest, V3* x, V3* y);
B32 V3ApproxEq(V3* x, V3* y);
B32 V3Eq(V3* x, V3* y);
F32 V3LengthSq(V3* x);
F32 V3Length(V3* x);
V3* V3Abs(V3* dest, V3* src);
V3* V3Normalize(V3* dest, V3* src);
V3* V3Negate(V3* dest, V3* src);
F32 V3MinValue(V3* x);
F32 V3MaxValue(V3* x);
B32 V3IsBetween(V3* x, V3* start, V3* end);
V3* V3Project(V3* dest, V3* x, V3* y);
V3* V3Clamp(V3* dest, V3* src, F32 min, F32 max);
B32 V3AreCollinear(V3* x, V3* y, V3* z);
F32 V3AngleBetween(V3* x, V3* y);
V3* V3Lerp(V3* dest, V3* x, V3* y, F32 t);
V3* V3RotateAroundAxis(V3* dest, V3* x, V3* axis, F32 angle_rad);

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

V4  V4Assign(F32 x, F32 y, F32 z, F32 w);
V4  V4Splat(F32 c);
V4  V4FromV2(V2* src, F32 z, F32 w);
V4  V4FromV3(V3* src, F32 w);
V4* V4AddF32(V4* dest, V4* x, F32 c);
V4* V4SubF32(V4* dest, V4* x, F32 c);
V4* V4MultF32(V4* dest, V4* x, F32 c);
V4* V4DivF32(V4* dest, V4* x, F32 c);
V4* V4AddV4(V4* dest, V4* x, V4* y);
V4* V4SubV4(V4* dest, V4* x, V4* y);
V4* V4HadamardV4(V4* dest, V4* x, V4* y);
V4* V4QuatMulV4(V4* dest, V4* x, V4* y);
F32 V4InnerMultV4(V4* x, V4* y);
F32 V4DotV4(V4* x, V4* y);
B32 V4ApproxEq(V4* a, V4* b);
B32 V4Eq(V4* x , V4* y);
F32 V4LengthSq(V4* x);
F32 V4Length(V4* x);
V4* V4Abs(V4* dest, V4* src);
V4* V4Normalize(V4* dest, V4* src);
V4* V4Negate(V4* dest, V4* src);
V4* V4Project(V4* dest, V4* x, V4* y);
V4* V4Clamp(V4* dest, V4* src, F32 min, F32 max);
V4* V4Lerp(V4* dest, V4* a, V4* b, F32 t);
V4* V4Slerp(V4* dest, V4* a, V4* b, F32 t);
V4* V4RotateAroundAxis(V4* dest, V3* axis, F32 angle_rad);
V4* V4LookInDir(V4* dest, V3* dir);
V4* V4LookAt(V4* dest, V3* pos, V3* target);

///////////////////////////////////////////////////////////////////////////////
// NOTE: M2
///////////////////////////////////////////////////////////////////////////////

#define M2_IDENTITY (M2) {1, 0, 0, \
                          0, 1, 0, \
                          0, 0, 1 }

M2* M2AddM2(M2* dest, M2* x, M2* y);
M2* M2SubM2(M2* dest, M2* x, M2* y);
M2* M2MultF32(M2* dest, F32 c, M2* m);
M2* M2MultM2(M2* dest, M2* x, M2* y);
V2* M2MultV2(V2* dest, M2* x, V2* y);
M2* M2Transpose(M2* dest, M2* x);
F32 M2Det(M2* m);
M2* M2Invert(M2* dest, M2* x);
B32 M2ApproxEq(M2* x, M2* y);
B32 M2Eq(M2* x, M2* y);

///////////////////////////////////////////////////////////////////////////////
// NOTE: M3
///////////////////////////////////////////////////////////////////////////////

#define M3_IDENTITY (M3) {1, 0, 0, \
                          0, 1, 0, \
                          0, 0, 1 }

M3  M3FromQuaternion(V4* q);
M3* M3AddM3(M3* dest, M3* x, M3* y);
M3* M3SubM3(M3* dest, M3* x, M3* y);
M3* M3MultF32(M3* dest, F32 c, M3* m);
M3* M3MultM3(M3* dest, M3* x, M3* y);
V3* M3MultV3(V3* dest, M3* x, V3* y);
M3* M3Transpose(M3* dest, M3* x);
F32 M3Det(M3* m);
M3* M3Invert(M3* dest, M3* x);
B32 M3ApproxEq(M3* x, M3* y);
B32 M3Eq(M3* x, M3* y);

///////////////////////////////////////////////////////////////////////////////
// NOTE: M4
///////////////////////////////////////////////////////////////////////////////

#define M4_IDENTITY (M4) {1, 0, 0, 0, \
                          0, 1, 0, 0, \
                          0, 0, 1, 0, \
                          0, 0, 0, 1}

M4  M4FromTransform(V3* pos, V4* rot, V3* scale);
M4  M4Perspective(F32 fov_y_rad, F32 aspect_ratio, F32 near_plane, F32 far_plane);
M4  M4Orthographic(F32 left, F32 right, F32 bottom, F32 top, F32 near_plane, F32 far_plane);
M4  M4LookAt(V3* eye, V3* target, V3* up);
M4* M4AddM4(M4* dest, M4* x, M4* y);
M4* M4SubM4(M4* dest, M4* x, M4* y);
M4* M4MultF32(M4* dest, F32 c,  M4* m);
M4* M4MultM4(M4* dest, M4* x, M4* y);
V4* M4MultV4(V4* dest, M4* x, V4* y);
M4* M4Transpose(M4* dest, M4* x);
F32 M4Det(M4* m);
M4* M4Invert(M4* dest, M4* x);
B32 M4ApproxEq(M4* x, M4* y);
B32 M4Eq(M4* x, M4* y);

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
F32 F32ToRad(F32 deg) { return deg * (F32_PI / 180.0f); }
F32 F32ToDeg(F32 rad) { return rad * (180.0f / F32_PI); }
F32 F32Ceil(F32 x) { return ceilf(x); }
F32 F32Floor(F32 x) { return floorf(x); }
F32 F32Round(F32 x) { return F32Floor(x + 0.5f); }
F32 F32MapRange(F32 x, F32 a_min, F32 a_max, F32 b_min, F32 b_max) { return b_min + ((x - a_min) / (a_max - a_min)) * (b_max - b_min); }
F32 F32Lerp(F32 x, F32 y, F32 t) { return x + ((y - x) * t); }
B32 F32IsInteger(F32 x) { return x == F32Floor(x); }

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
F64 F64MapRange(F64 x, F64 a_min, F64 a_max, F64 b_min, F64 b_max) { return b_min + ((x - a_min) / (a_max - a_min)) * (b_max - b_min); }
F64 F64Lerp(F64 x, F64 y, F64 t) { return x + ((y - x) * t); }
B32 F64IsInteger(F64 x) { return x == F64Floor(x); }

///////////////////////////////////////////////////////////////////////////////
// NOTE: V2 implementation
///////////////////////////////////////////////////////////////////////////////

V2 V2Assign(F32 x, F32 y) {
  V2 result;
  result.x = x;
  result.y = y;
  return result;
}

V2 V2Splat(F32 c) {
  V2 result;
  result.x = c;
  result.y = c;
  return result;
}

V2* V2AddF32(V2* dest, V2* x, F32 c) {
  dest->x = x->x + c;
  dest->y = x->y + c;
  return dest;
}

V2* V2SubF32(V2* dest, V2* x, F32 c) {
  dest->x = x->x - c;
  dest->y = x->y - c;
  return dest;
}

V2* V2MultF32(V2* dest, V2* x, F32 c) {
  dest->x = x->x * c;
  dest->y = x->y * c;
  return dest;
}

V2* V2DivF32(V2* dest, V2* x, F32 c) {
  DEBUG_ASSERT(c != 0);
  dest->x = x->x / c;
  dest->y = x->y / c;
  return dest;
}

V2* V2AddV2(V2* dest, V2* x, V2* y) {
  dest->x = x->x + y->x;
  dest->y = x->y + y->y;
  return dest;
}

V2* V2SubV2(V2* dest, V2* x, V2* y) {
  dest->x = x->x - y->x;
  dest->y = x->y - y->y;
  return dest;
}

V2* V2HadamardV2(V2* dest, V2* x, V2* y) {
  dest->x = x->x * y->x;
  dest->y = x->y * y->y;
  return dest;
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

V2* V2Abs(V2* dest, V2* src) {
  dest->x = F32Abs(src->x);
  dest->y = F32Abs(src->y);
  return dest;
}

V2* V2Normalize(V2* dest, V2* x) {
  return V2DivF32(dest, x, V2Length(x));
}

V2* V2Negate(V2* dest, V2* src) {
  return V2MultF32(dest, src, -1);
}

F32 V2MinValue(V2* x) {
  return MIN(x->x, x->y);
}

F32 V2MaxValue(V2* x) {
  return MAX(x->x, x->y);
}

V2* V2Clamp(V2* dest, V2* src, F32 min, F32 max) {
  dest->x = CLAMP(min, src->x, max);
  dest->y = CLAMP(min, src->y, max);
  return dest;
}

V2* V2Project(V2* dest, V2* x, V2* y) {
  F32 ratio = V2DotV2(x, y) / V2DotV2(y, y);
  return V2MultF32(dest, y, ratio);
}
 
V2* V2Rotate(V2* dest, V2* src, F32 angle_rad) {
  V2 result;
  F32 s = F32Sin(angle_rad);
  F32 c = F32Cos(angle_rad);
  result.x = src->x * c - src->y * s;
  result.y = src->x * c + src->y * s;
  *dest = result;
  return dest;
}

V2* V2Lerp(V2* dest, V2* x, V2* y, F32 t) {
  dest->x = F32Lerp(x->x, y->x, t);
  dest->y = F32Lerp(x->y, y->y, t);
  return dest;
}

///////////////////////////////////////////////////////////////////////////////
// NOTE: V3 implementation
///////////////////////////////////////////////////////////////////////////////

V3 V3Assign(F32 x, F32 y, F32 z) {
  V3 result;
  result.x = x;
  result.y = y;
  result.z = z;
  return result;
}

V3 V3Splat(F32 c) {
  V3 result;
  result.x = c;
  result.y = c;
  result.z = c;
  return result;
}

V3 V3FromV2(V2* src, F32 z) {
  V3 result;
  result.x = src->x;
  result.y = src->y;
  result.z = z;
  return result;
}

V3 V3FromHex(U32 x) {
  V3 result;
  result.r = ((x >> 16) & 0xff) / 255.0f;
  result.g = ((x >> 8)  & 0xff) / 255.0f;
  result.b = ((x >> 0)  & 0xff) / 255.0f;
  return result;
}

V3* V3AddF32(V3* dest, V3* x, F32 c) {
  dest->x = x->x + c;
  dest->y = x->y + c;
  dest->z = x->z + c;
  return dest;
}

V3* V3SubF32(V3* dest, V3* x, F32 c) {
  dest->x = x->x - c;
  dest->y = x->y - c;
  dest->z = x->z - c;
  return dest;
}

V3* V3MultF32(V3* dest, V3* x, F32 c) {
  dest->x = x->x * c;
  dest->y = x->y * c;
  dest->z = x->z * c;
  return dest;
}

V3* V3DivF32(V3* dest, V3* x, F32 c) {
  ASSERT(c != 0);
  dest->x = x->x / c;
  dest->y = x->y / c;
  dest->z = x->z / c;
  return dest;
}

V3* V3AddV3(V3* dest, V3* x, V3* y) {
  dest->x = x->x + y->x;
  dest->y = x->y + y->y;
  dest->z = x->z + y->z;
  return dest;
}

V3* V3SubV3(V3*dest, V3* x, V3* y) {
  dest->x = x->x - y->x;
  dest->y = x->y - y->y;
  dest->z = x->z - y->z;
  return dest;
}

V3* V3HadamardV3(V3* dest, V3* x, V3* y) {
  dest->x = x->x * y->x;
  dest->y = x->y * y->y;
  dest->z = x->z * y->z;
  return dest;
}

F32 V3InnerMultV3(V3* x, V3* y) {
  return V3DotV3(x, y);
}

F32 V3DotV3(V3* x, V3* y) {
  return (x->x * y->x) + (x->y * y->y) + (x->z * y->z);
}

V3* V3CrossV3(V3* dest, V3* x, V3* y) {
  V3 result;
  result.x = (x->y * y->z) - (x->z * y->y);
  result.y = (x->z * y->x) - (x->x * y->z);
  result.z = (x->x * y->y) - (x->y * y->x);
  *dest = result;
  return dest;
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

V3* V3Abs(V3* dest, V3* src) {
  dest->x = F32Abs(src->x);
  dest->y = F32Abs(src->y);
  dest->z = F32Abs(src->z);
  return dest;
}

V3* V3Normalize(V3* dest, V3* src) {
  return V3DivF32(dest, src, V3Length(src));
}

V3* V3Negate(V3* dest, V3* src) {
  return V3MultF32(dest, src, -1);
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
V3* V3Project(V3* dest, V3* x, V3* y) {
  F32 ratio = V3DotV3(x, y) / V3DotV3(y, y);
  return V3MultF32(dest, y, ratio);
}

V3* V3Clamp(V3* dest, V3* src, F32 min, F32 max) {
  dest->x = CLAMP(min, src->x, max);
  dest->y = CLAMP(min, src->y, max);
  dest->z = CLAMP(min, src->z, max);
  return dest;
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

V3* V3Lerp(V3* dest, V3* x, V3* y, F32 t) {
  dest->x = F32Lerp(x->x, y->x, t);
  dest->y = F32Lerp(x->y, y->y, t);
  dest->z = F32Lerp(x->z, y->z, t);
  return dest;
}

// https://stackoverflow.com/questions/69245724/rotate-a-vector-around-an-axis-in-3d-space
V3* V3RotateAroundAxis(V3* dest, V3* v, V3* axis, F32 angle_rad) {
  V3 cross, double_cross;
  V3CrossV3(&cross, axis, v);
  V3CrossV3(&double_cross, axis, &cross);
  V3 a, b;
  V3MultF32(&a, &cross, F32Sin(angle_rad));
  V3MultF32(&b, &double_cross, (1.0f - F32Cos(angle_rad)));
  dest->x = v->x + a.x + b.x;
  dest->y = v->y + a.y + b.y;
  dest->z = v->z + a.z + b.z;
  return dest;
}

///////////////////////////////////////////////////////////////////////////////
// NOTE: V4 implementation
///////////////////////////////////////////////////////////////////////////////

V4 V4Assign(F32 x, F32 y, F32 z, F32 w) {
  V4 result;
  result.x = x;
  result.y = y;
  result.z = z;
  result.w = w;
  return result;
}

V4 V4Splat(F32 c) {
  V4 result;
  result.x = c;
  result.y = c;
  result.z = c;
  result.w = c;
  return result;
}


V4 V4FromV2(V2* src, F32 z, F32 w) {
  V4 result;
  result.x = src->x;
  result.y = src->y;
  result.z = z;
  result.w = w;
  return result;
}

V4 V4FromV3(V3* src, F32 w) {
  V4 result;
  result.x = src->x;
  result.y = src->y;
  result.z = src->z;
  result.w = w;
  return result;
}

V4* V4AddV4(V4* dest, V4* x, V4* y) {
  dest->x = x->x + y->x;
  dest->y = x->y + y->y;
  dest->z = x->z + y->z;
  dest->w = x->w + y->w;
  return dest;
}

V4* V4AddF32(V4* dest, V4* x, F32 c) {
  dest->x = x->x + c;
  dest->y = x->y + c;
  dest->z = x->z + c;
  dest->w = x->w + c;
  return dest;
}

V4* V4MultF32(V4* dest, V4* x, F32 c) {
  dest->x = x->x * c;
  dest->y = x->y * c;
  dest->z = x->z * c;
  dest->w = x->w * c;
  return dest;
}

V4* V4DivF32(V4* dest, V4* x, F32 c) {
  dest->x = x->x / c;
  dest->y = x->y / c;
  dest->z = x->z / c;
  dest->w = x->w / c;
  return dest;
}

V4* V4SubF32(V4* dest, V4* x, F32 c) {
  dest->x = x->x - c;
  dest->y = x->y - c;
  dest->z = x->z - c;
  dest->w = x->w - c;
  return dest;
}

V4* V4SubV4(V4* dest, V4* x, V4* y) {
  dest->x = x->x - y->x;
  dest->y = x->y - y->y;
  dest->z = x->z - y->z;
  dest->w = x->w - y->w;
  return dest;
}

F32 V4LengthSq(V4* x) {
  return V4DotV4(x, x);
}

F32 V4Length(V4* x) {
  return F32Sqrt(V4LengthSq(x));
}

V4* V4Hadamard(V4* dest, V4* x, V4* y) {
  dest->x = x->x * y->x;
  dest->y = x->y * y->y;
  dest->z = x->z * y->z;
  dest->w = x->w * y->w;
  return dest;
}

V4* V4HadamardV4(V4* dest, V4* x, V4* y) {
  dest->x = x->x * y->x;
  dest->y = x->y * y->y;
  dest->z = x->z * y->z;
  dest->w = x->w * y->w;
  return dest;
}

// https://stackoverflow.com/questions/19956555/how-to-multiply-two-quaternions
V4* V4QuatMulV4(V4* dest, V4* x, V4* y) {
  dest->x = x->w*y->x + x->x*y->w + x->y*y->z - x->z*y->y;
  dest->y = x->w*y->y - x->x*y->z + x->y*y->w + x->z*y->x;
  dest->z = x->w*y->z + x->x*y->y - x->y*y->x + x->z*y->w;
  dest->w = x->w*y->w - x->x*y->x - x->y*y->y - x->z*y->z;
  return dest;
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

V4* V4Abs(V4* dest, V4* src) {
  dest->x = F32Abs(src->x);
  dest->y = F32Abs(src->y);
  dest->z = F32Abs(src->z);
  return dest;
}

V4* V4Normalize(V4* dest, V4* src) {
  return V4DivF32(dest, src, V4Length(src));
}

V4* V4Negate(V4* dest, V4* src) {
  return V4MultF32(dest, src, -1);
}

V4* V4Project(V4* dest, V4* x, V4* y) {
  F32 ratio = V4DotV4(x, y) / V4DotV4(y, y);
  return V4MultF32(dest, y, ratio);
}

V4* V4Clamp(V4* dest, V4* src, F32 min, F32 max) {
  dest->x = CLAMP(min, src->x, max);
  dest->y = CLAMP(min, src->y, max);
  dest->z = CLAMP(min, src->z, max);
  dest->w = CLAMP(min, src->w, max);
  return dest;
}

V4* V4Lerp(V4* dest, V4* a, V4* b, F32 t) {
  dest->x = F32Lerp(a->x, b->x, t);
  dest->y = F32Lerp(a->y, b->y, t);
  dest->z = F32Lerp(a->z, b->z, t);
  dest->w = F32Lerp(a->w, b->w, t);
  return dest;
}

V4* V4Slerp(V4* dest, V4* a, V4* b, F32 t) {
  V4 a_copy = *a;
  V4 b_copy = *b;
  F32 dot = V4DotV4(&a_copy, &b_copy);
  if (dot < 0) {
    dot = -dot;
    V4MultF32(&a_copy, a, -1);
  }
  if (UNLIKELY(F32ApproxEq(dot, 1))) {
    V4Lerp(dest, &a_copy, &b_copy, t);
    return dest;
  }
  F32 angle_rad = F32ArcCos(dot);
  F32 a_scalar = F32Sin((1.0f - t) * angle_rad) / F32Sin(angle_rad);
  F32 b_scalar = F32Sin(t * angle_rad) / F32Sin(angle_rad);
  V4MultF32(&a_copy, &a_copy, a_scalar);
  V4MultF32(&b_copy, &b_copy, b_scalar);
  V4AddV4(dest, &a_copy, &b_copy);
  return dest;
}

V4* V4RotateAroundAxis(V4* dest, V3* axis, F32 angle_rad) {
  F32 factor = F32Sin(angle_rad / 2.0f);
  dest->x = factor * axis->x;
  dest->y = factor * axis->y;
  dest->z = factor * axis->z;
  dest->w = F32Cos(angle_rad / 2.0f);
  return V4Normalize(dest, dest);
}

// https://stackoverflow.com/questions/12435671/quaternion-lookat-function
V4* V4LookInDir(V4* dest, V3* dir) {
  ASSERT(F32ApproxEq(V3LengthSq(dir), 1.0f));
  F32 dot = V3DotV3(&V3_Y_NEG, dir);
  if (F32ApproxEq(dot, -1)) {
    // TODO: this is probably wrong?
    *dest = (V4) {0, 0, 1, 0};
    return dest;
  } else if (F32ApproxEq(dot, +1)) {
    *dest = V4_QUAT_IDENT;
    return dest;
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
  return dest;
}

V4* V4LookAt(V4* dest, V3* pos, V3* target) {
  V3 dir;
  V3SubV3(&dir, target, pos);
  V3Normalize(&dir, &dir);
  V4LookInDir(dest, &dir);
  return dest;
}

///////////////////////////////////////////////////////////////////////////////
// NOTE: M2 implementation
///////////////////////////////////////////////////////////////////////////////

M2* M2AddM2(M2* dest, M2* x, M2* y) {
  dest->e[0][0] = x->e[0][0] + y->e[0][0];
  dest->e[0][1] = x->e[0][1] + y->e[0][1];
  dest->e[1][0] = x->e[1][0] + y->e[1][0];
  dest->e[1][1] = x->e[1][1] + y->e[1][1];
  return dest;
}

M2* M2SubM2(M2* dest, M2* x, M2* y) {
  dest->e[0][0] = x->e[0][0] - y->e[0][0];
  dest->e[0][1] = x->e[0][1] - y->e[0][1];
  dest->e[1][0] = x->e[1][0] - y->e[1][0];
  dest->e[1][1] = x->e[1][1] - y->e[1][1];
  return dest;
}

M2* M2MultF32(M2* dest, F32 c, M2* m) {
  dest->e[0][0] = m->e[0][0] * c;
  dest->e[0][1] = m->e[0][1] * c;
  dest->e[1][0] = m->e[1][0] * c;
  dest->e[1][1] = m->e[1][1] * c;
  return dest;
}

M2* M2MultM2(M2* dest, M2* x, M2* y) {
  M2 result;
  result.e[0][0] = (x->e[0][0] * y->e[0][0]) + (x->e[0][1] * y->e[1][0]);
  result.e[0][1] = (x->e[0][0] * y->e[0][1]) + (x->e[0][1] * y->e[1][1]);
  result.e[1][0] = (x->e[1][0] * y->e[0][0]) + (x->e[1][1] * y->e[1][0]);
  result.e[1][1] = (x->e[1][0] * y->e[0][1]) + (x->e[1][1] * y->e[1][1]);
  *dest = result;
  return dest;
}

V2* M2MultV2(V2* dest, M2* x, V2* y) {
  V2 result;
  result.e[0] = (x->e[0][0] * y->e[0]) + (x->e[0][1] * y->e[1]);
  result.e[1] = (x->e[1][0] * y->e[0]) + (x->e[1][1] * y->e[1]);
  *dest = result;
  return dest;
}

M2* M2Transpose(M2* dest, M2* x) {
  M2 result;
  result.e[0][0] = x->e[0][0];
  result.e[0][1] = x->e[1][0];
  result.e[1][0] = x->e[0][1];
  result.e[1][1] = x->e[1][1];
  *dest = result;
  return dest;
}

F32 M2Det(M2* m) {
  return (m->e[0][0] * m->e[1][1]) - (m->e[0][1] * m->e[1][0]);
}

M2* M2Invert(M2* dest, M2* x) {
  F32 det = M2Det(x);
  DEBUG_ASSERT(det != 0);
  F32 det_inv = 1.0f / det;
  dest->e[0][0] = det_inv * +x->e[1][1];
  dest->e[0][1] = det_inv * -x->e[0][1];
  dest->e[1][0] = det_inv * -x->e[1][0];
  dest->e[1][1] = det_inv * +x->e[0][0];
  return dest;
}

B32 M2ApproxEq(M2* x, M2* y) {
  return F32ApproxEq(x->e[0][0], y->e[0][0]) &&
         F32ApproxEq(x->e[0][1], y->e[0][1]) &&
         F32ApproxEq(x->e[1][0], y->e[1][0]) &&
         F32ApproxEq(x->e[1][1], y->e[1][1]);
}

B32 M2Eq(M2* x, M2* y) {
  return x->e[0][0] == y->e[0][0] &&
         x->e[0][1] == y->e[0][1] &&
         x->e[1][0] == y->e[1][0] &&
         x->e[1][1] == y->e[1][1];
}

///////////////////////////////////////////////////////////////////////////////
// NOTE: M3 implementation
///////////////////////////////////////////////////////////////////////////////

M3* M3AddM3(M3* dest, M3* x, M3* y) {
  dest->e[0][0] = x->e[0][0] + y->e[0][0];
  dest->e[0][1] = x->e[0][1] + y->e[0][1];
  dest->e[0][2] = x->e[0][2] + y->e[0][2];
  dest->e[1][0] = x->e[1][0] + y->e[1][0];
  dest->e[1][1] = x->e[1][1] + y->e[1][1];
  dest->e[1][2] = x->e[1][2] + y->e[1][2];
  dest->e[2][0] = x->e[2][0] + y->e[2][0];
  dest->e[2][1] = x->e[2][1] + y->e[2][1];
  dest->e[2][2] = x->e[2][2] + y->e[2][2];
  return dest;
}

M3* M3SubM3(M3* dest, M3* x, M3* y) {
  dest->e[0][0] = x->e[0][0] - y->e[0][0];
  dest->e[0][1] = x->e[0][1] - y->e[0][1];
  dest->e[0][2] = x->e[0][2] - y->e[0][2];
  dest->e[1][0] = x->e[1][0] - y->e[1][0];
  dest->e[1][1] = x->e[1][1] - y->e[1][1];
  dest->e[1][2] = x->e[1][2] - y->e[1][2];
  dest->e[2][0] = x->e[2][0] - y->e[2][0];
  dest->e[2][1] = x->e[2][1] - y->e[2][1];
  dest->e[2][2] = x->e[2][2] - y->e[2][2];
  return dest;
}

M3* M3MultF32(M3* dest, F32 c, M3* m) {
  dest->e[0][0] = m->e[0][0] * c;
  dest->e[0][1] = m->e[0][1] * c;
  dest->e[0][2] = m->e[0][2] * c;
  dest->e[1][0] = m->e[1][0] * c;
  dest->e[1][1] = m->e[1][1] * c;
  dest->e[1][2] = m->e[1][2] * c;
  dest->e[2][0] = m->e[2][0] * c;
  dest->e[2][1] = m->e[2][1] * c;
  dest->e[2][2] = m->e[2][2] * c;
  return dest;
}

M3* M3MultM3(M3* dest, M3* x, M3* y) {
  M3 result;
  result.e[0][0] = (x->e[0][0] * y->e[0][0]) + (x->e[0][1] * y->e[1][0]) + (x->e[0][2] * y->e[2][0]);
  result.e[0][1] = (x->e[0][0] * y->e[0][1]) + (x->e[0][1] * y->e[1][1]) + (x->e[0][2] * y->e[2][1]);
  result.e[0][2] = (x->e[0][0] * y->e[0][2]) + (x->e[0][1] * y->e[1][2]) + (x->e[0][2] * y->e[2][2]);
  result.e[1][0] = (x->e[1][0] * y->e[0][0]) + (x->e[1][1] * y->e[1][0]) + (x->e[1][2] * y->e[2][0]);
  result.e[1][1] = (x->e[1][0] * y->e[0][1]) + (x->e[1][1] * y->e[1][1]) + (x->e[1][2] * y->e[2][1]);
  result.e[1][2] = (x->e[1][0] * y->e[0][2]) + (x->e[1][1] * y->e[1][2]) + (x->e[1][2] * y->e[2][2]);
  result.e[2][0] = (x->e[2][0] * y->e[0][0]) + (x->e[2][1] * y->e[1][0]) + (x->e[2][2] * y->e[2][0]);
  result.e[2][1] = (x->e[2][0] * y->e[0][1]) + (x->e[2][1] * y->e[1][1]) + (x->e[2][2] * y->e[2][1]);
  result.e[2][2] = (x->e[2][0] * y->e[0][2]) + (x->e[2][1] * y->e[1][2]) + (x->e[2][2] * y->e[2][2]);
  *dest = result;
  return dest;
}

V3* M3MultV3(V3* dest, M3* x, V3* y) {
  V3 result;
  result.e[0] = (x->e[0][0] * y->e[0]) + (x->e[0][1] * y->e[1]) + (x->e[0][2] * y->e[2]);
  result.e[1] = (x->e[1][0] * y->e[0]) + (x->e[1][1] * y->e[1]) + (x->e[1][2] * y->e[2]);
  result.e[2] = (x->e[2][0] * y->e[0]) + (x->e[2][1] * y->e[1]) + (x->e[2][2] * y->e[2]);
  *dest = result;
  return dest;
}

M3* M3Transpose(M3* dest, M3* x) {
  M3 result;
  result.e[0][0] = x->e[0][0];
  result.e[0][1] = x->e[1][0];
  result.e[0][2] = x->e[2][0];
  result.e[1][0] = x->e[0][1];
  result.e[1][1] = x->e[1][1];
  result.e[1][2] = x->e[2][1];
  result.e[2][0] = x->e[0][2];
  result.e[2][1] = x->e[1][2];
  result.e[2][2] = x->e[2][2];
  *dest = result;
  return dest;
}

// https://www.chilimath.com/lessons/advanced-algebra/determinant-3x3-matrix/
F32 M3Det(M3* m) {
  F32 sub_0 = (m->e[1][1] * m->e[2][2]) - (m->e[1][2] * m->e[2][1]);
  F32 sub_1 = (m->e[1][0] * m->e[2][2]) - (m->e[1][2] * m->e[2][0]);
  F32 sub_2 = (m->e[1][0] * m->e[2][1]) - (m->e[1][1] * m->e[2][0]);
  return (m->e[0][0] * sub_0) - (m->e[0][1] * sub_1) + (m->e[0][2] * sub_2);
}

M3* M3Invert(M3* dest, M3* m) {
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
  return dest;
}

// https://automaticaddison.com/how-to-convert-a-quaternion-to-a-rotation-matrix/
M3 M3FromQuaternion(V4* q) {
  M3 result;
  result.e[0][0] = 2.0f*(q->w*q->w + q->x*q->x) - 1.0f;
  result.e[0][1] = 2.0f*(q->x*q->y - q->w*q->z);
  result.e[0][2] = 2.0f*(q->x*q->z + q->w*q->y);
  result.e[1][0] = 2.0f*(q->x*q->y + q->w*q->z);
  result.e[1][1] = 2.0f*(q->w*q->w + q->y*q->y) - 1.0f;
  result.e[1][2] = 2.0f*(q->y*q->z - q->w*q->x);
  result.e[2][0] = 2.0f*(q->x*q->z - q->w*q->y);
  result.e[2][1] = 2.0f*(q->y*q->z + q->w*q->x);
  result.e[2][2] = 2.0f*(q->w*q->w + q->z*q->z) - 1.0f;
  return result;
}

B32 M3ApproxEq(M3* x, M3* y) {
  return F32ApproxEq(x->e[0][0], y->e[0][0]) &&
         F32ApproxEq(x->e[0][1], y->e[0][1]) &&
         F32ApproxEq(x->e[0][2], y->e[0][2]) &&
         F32ApproxEq(x->e[1][0], y->e[1][0]) &&
         F32ApproxEq(x->e[1][1], y->e[1][1]) &&
         F32ApproxEq(x->e[1][2], y->e[1][2]) &&
         F32ApproxEq(x->e[2][0], y->e[2][0]) &&
         F32ApproxEq(x->e[2][1], y->e[2][1]) &&
         F32ApproxEq(x->e[2][2], y->e[2][2]);
}

B32 M3Eq(M3* x, M3* y) {
  return x->e[0][0] == y->e[0][0] &&
         x->e[0][1] == y->e[0][1] &&
         x->e[0][2] == y->e[0][2] &&
         x->e[1][0] == y->e[1][0] &&
         x->e[1][1] == y->e[1][1] &&
         x->e[1][2] == y->e[1][2] &&
         x->e[2][0] == y->e[2][0] &&
         x->e[2][1] == y->e[2][1] &&
         x->e[2][2] == y->e[2][2];
}

///////////////////////////////////////////////////////////////////////////////
// NOTE: M4 implementation
///////////////////////////////////////////////////////////////////////////////

// TODO: faster way to do this?
M4 M4FromTransform(V3* pos, V4* rot, V3* scale) {
  M3 scale_m;
  MEMORY_ZERO_STRUCT(&scale_m);
  scale_m.e[0][0] = scale->x;
  scale_m.e[1][1] = scale->y;
  scale_m.e[2][2] = scale->z;

  M3 rot_m = M3FromQuaternion(rot);

  M3 rot_scale;
  M3MultM3(&rot_scale, &rot_m, &scale_m);

  M4 result;
  MEMORY_ZERO_STRUCT(&result);
  result.e[0][0] = rot_scale.e[0][0];
  result.e[0][1] = rot_scale.e[0][1];
  result.e[0][2] = rot_scale.e[0][2];
  result.e[1][0] = rot_scale.e[1][0];
  result.e[1][1] = rot_scale.e[1][1];
  result.e[1][2] = rot_scale.e[1][2];
  result.e[2][0] = rot_scale.e[2][0];
  result.e[2][1] = rot_scale.e[2][1];
  result.e[2][2] = rot_scale.e[2][2];

  result.e[0][3] = pos->x;
  result.e[1][3] = pos->y;
  result.e[2][3] = pos->z;
  result.e[3][3] = 1;
  return result;
}

M4 M4Perspective(F32 fov_y_rad, F32 aspect_ratio, F32 near_plane, F32 far_plane) {
  F32 tan_half_fov_y = F32Tan(fov_y_rad / 2.0f);

  F32 a = 1.0f / (aspect_ratio * tan_half_fov_y);
  F32 b = 1.0f / (tan_half_fov_y);
  F32 c = - (far_plane + near_plane) / (far_plane - near_plane);
  F32 d = - (2 * far_plane * near_plane) / (far_plane - near_plane);

  M4 result = {
    a,  0,  0,  0,
    0,  b,  0,  0,
    0,  0,  c,  d,
    0,  0, -1,  0,
  };
  return result;
}

M4 M4Orthographic(F32 left, F32 right, F32 bottom, F32 top, F32 near_plane, F32 far_plane) {
  F32 a = 2 / (right - left);
  F32 b = 2 / (top - bottom);
  F32 c = -2 / (far_plane - near_plane);
  F32 d = -(right + left) / (right - left);
  F32 e = -(top + bottom) / (top - bottom);
  F32 f = -(far_plane + near_plane) / (far_plane - near_plane);

  M4 result = {
    a, 0, 0, d,
    0, b, 0, e,
    0, 0, c, f,
    0, 0, 0, 1,
  };
  return result;
}

M4 M4LookAt(V3* eye, V3* target, V3* up) {
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

  M4 result = {
    x.x, x.y, x.z, a,
    y.x, y.y, y.z, b,
    z.x, z.y, z.z, c,
      0,   0,   0, 1,
  };
  return result;
}

M4* M4AddM4(M4* dest, M4* x, M4* y) {
  dest->e[0][0] = x->e[0][0] + y->e[0][0];
  dest->e[0][1] = x->e[0][1] + y->e[0][1];
  dest->e[0][2] = x->e[0][2] + y->e[0][2];
  dest->e[0][3] = x->e[0][3] + y->e[0][3];
  dest->e[1][0] = x->e[1][0] + y->e[1][0];
  dest->e[1][1] = x->e[1][1] + y->e[1][1];
  dest->e[1][2] = x->e[1][2] + y->e[1][2];
  dest->e[1][3] = x->e[1][3] + y->e[1][3];
  dest->e[2][0] = x->e[2][0] + y->e[2][0];
  dest->e[2][1] = x->e[2][1] + y->e[2][1];
  dest->e[2][2] = x->e[2][2] + y->e[2][2];
  dest->e[2][3] = x->e[2][3] + y->e[2][3];
  dest->e[3][0] = x->e[3][0] + y->e[3][0];
  dest->e[3][1] = x->e[3][1] + y->e[3][1];
  dest->e[3][2] = x->e[3][2] + y->e[3][2];
  dest->e[3][3] = x->e[3][3] + y->e[3][3];
  return dest;
}

M4* M4SubM4(M4* dest, M4* x, M4* y) {
  dest->e[0][0] = x->e[0][0] - y->e[0][0];
  dest->e[0][1] = x->e[0][1] - y->e[0][1];
  dest->e[0][2] = x->e[0][2] - y->e[0][2];
  dest->e[0][3] = x->e[0][3] - y->e[0][3];
  dest->e[1][0] = x->e[1][0] - y->e[1][0];
  dest->e[1][1] = x->e[1][1] - y->e[1][1];
  dest->e[1][2] = x->e[1][2] - y->e[1][2];
  dest->e[1][3] = x->e[1][3] - y->e[1][3];
  dest->e[2][0] = x->e[2][0] - y->e[2][0];
  dest->e[2][1] = x->e[2][1] - y->e[2][1];
  dest->e[2][2] = x->e[2][2] - y->e[2][2];
  dest->e[2][3] = x->e[2][3] - y->e[2][3];
  dest->e[3][0] = x->e[3][0] - y->e[3][0];
  dest->e[3][1] = x->e[3][1] - y->e[3][1];
  dest->e[3][2] = x->e[3][2] - y->e[3][2];
  dest->e[3][3] = x->e[3][3] - y->e[3][3];
  return dest;
}

M4* M4MultF32(M4* dest, F32 c, M4* m) {
  dest->e[0][0] = m->e[0][0] * c;
  dest->e[0][1] = m->e[0][1] * c;
  dest->e[0][2] = m->e[0][2] * c;
  dest->e[0][3] = m->e[0][3] * c;
  dest->e[1][0] = m->e[1][0] * c;
  dest->e[1][1] = m->e[1][1] * c;
  dest->e[1][2] = m->e[1][2] * c;
  dest->e[1][3] = m->e[1][3] * c;
  dest->e[2][0] = m->e[2][0] * c;
  dest->e[2][1] = m->e[2][1] * c;
  dest->e[2][2] = m->e[2][2] * c;
  dest->e[2][3] = m->e[2][3] * c;
  dest->e[3][0] = m->e[3][0] * c;
  dest->e[3][1] = m->e[3][1] * c;
  dest->e[3][2] = m->e[3][2] * c;
  dest->e[3][3] = m->e[3][3] * c;
  return dest;
}

M4* M4MultM4(M4* dest, M4* x, M4* y) {
  M4 result;
  result.e[0][0] = (x->e[0][0] * y->e[0][0]) + (x->e[0][1] * y->e[1][0]) + (x->e[0][2] * y->e[2][0]) + (x->e[0][3] * y->e[3][0]);
  result.e[0][1] = (x->e[0][0] * y->e[0][1]) + (x->e[0][1] * y->e[1][1]) + (x->e[0][2] * y->e[2][1]) + (x->e[0][3] * y->e[3][1]);
  result.e[0][2] = (x->e[0][0] * y->e[0][2]) + (x->e[0][1] * y->e[1][2]) + (x->e[0][2] * y->e[2][2]) + (x->e[0][3] * y->e[3][2]);
  result.e[0][3] = (x->e[0][0] * y->e[0][3]) + (x->e[0][1] * y->e[1][3]) + (x->e[0][2] * y->e[2][3]) + (x->e[0][3] * y->e[3][3]);
  result.e[1][0] = (x->e[1][0] * y->e[0][0]) + (x->e[1][1] * y->e[1][0]) + (x->e[1][2] * y->e[2][0]) + (x->e[1][3] * y->e[3][0]);
  result.e[1][1] = (x->e[1][0] * y->e[0][1]) + (x->e[1][1] * y->e[1][1]) + (x->e[1][2] * y->e[2][1]) + (x->e[1][3] * y->e[3][1]);
  result.e[1][2] = (x->e[1][0] * y->e[0][2]) + (x->e[1][1] * y->e[1][2]) + (x->e[1][2] * y->e[2][2]) + (x->e[1][3] * y->e[3][2]);
  result.e[1][3] = (x->e[1][0] * y->e[0][3]) + (x->e[1][1] * y->e[1][3]) + (x->e[1][2] * y->e[2][3]) + (x->e[1][3] * y->e[3][3]);
  result.e[2][0] = (x->e[2][0] * y->e[0][0]) + (x->e[2][1] * y->e[1][0]) + (x->e[2][2] * y->e[2][0]) + (x->e[2][3] * y->e[3][0]);
  result.e[2][1] = (x->e[2][0] * y->e[0][1]) + (x->e[2][1] * y->e[1][1]) + (x->e[2][2] * y->e[2][1]) + (x->e[2][3] * y->e[3][1]);
  result.e[2][2] = (x->e[2][0] * y->e[0][2]) + (x->e[2][1] * y->e[1][2]) + (x->e[2][2] * y->e[2][2]) + (x->e[2][3] * y->e[3][2]);
  result.e[2][3] = (x->e[2][0] * y->e[0][3]) + (x->e[2][1] * y->e[1][3]) + (x->e[2][2] * y->e[2][3]) + (x->e[2][3] * y->e[3][3]);
  result.e[3][0] = (x->e[3][0] * y->e[0][0]) + (x->e[3][1] * y->e[1][0]) + (x->e[3][2] * y->e[2][0]) + (x->e[3][3] * y->e[3][0]);
  result.e[3][1] = (x->e[3][0] * y->e[0][1]) + (x->e[3][1] * y->e[1][1]) + (x->e[3][2] * y->e[2][1]) + (x->e[3][3] * y->e[3][1]);
  result.e[3][2] = (x->e[3][0] * y->e[0][2]) + (x->e[3][1] * y->e[1][2]) + (x->e[3][2] * y->e[2][2]) + (x->e[3][3] * y->e[3][2]);
  result.e[3][3] = (x->e[3][0] * y->e[0][3]) + (x->e[3][1] * y->e[1][3]) + (x->e[3][2] * y->e[2][3]) + (x->e[3][3] * y->e[3][3]);
  *dest = result;
  return dest;
}

V4* M4MultV4(V4* dest, M4* x, V4* y) {
  V4 result;
  result.e[0] = (x->e[0][0] * y->e[0]) + (x->e[0][1] * y->e[1]) + (x->e[0][2] * y->e[2]) + (x->e[0][3] * y->e[3]);
  result.e[1] = (x->e[1][0] * y->e[0]) + (x->e[1][1] * y->e[1]) + (x->e[1][2] * y->e[2]) + (x->e[1][3] * y->e[3]);
  result.e[2] = (x->e[2][0] * y->e[0]) + (x->e[2][1] * y->e[1]) + (x->e[2][2] * y->e[2]) + (x->e[2][3] * y->e[3]);
  result.e[3] = (x->e[3][0] * y->e[0]) + (x->e[3][1] * y->e[1]) + (x->e[3][2] * y->e[2]) + (x->e[3][3] * y->e[3]);
  *dest = result;
  return dest;
}

M4* M4Transpose(M4* dest, M4* x) {
  M4 result;
  result.e[0][0] = x->e[0][0];
  result.e[0][1] = x->e[1][0];
  result.e[0][2] = x->e[2][0];
  result.e[0][3] = x->e[3][0];
  result.e[1][0] = x->e[0][1];
  result.e[1][1] = x->e[1][1];
  result.e[1][2] = x->e[2][1];
  result.e[1][3] = x->e[3][1];
  result.e[2][0] = x->e[0][2];
  result.e[2][1] = x->e[1][2];
  result.e[2][2] = x->e[2][2];
  result.e[2][3] = x->e[3][2];
  result.e[3][0] = x->e[0][3];
  result.e[3][1] = x->e[1][3];
  result.e[3][2] = x->e[2][3];
  result.e[3][3] = x->e[3][3];
  *dest = result;
  return dest;
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
M4* M4Invert(M4* dest, M4* x) {
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
  return dest;
}

B32 M4ApproxEq(M4* x, M4* y) {
  return F32ApproxEq(x->e[0][0], y->e[0][0]) &&
         F32ApproxEq(x->e[0][1], y->e[0][1]) &&
         F32ApproxEq(x->e[0][2], y->e[0][2]) &&
         F32ApproxEq(x->e[0][3], y->e[0][3]) &&
         F32ApproxEq(x->e[1][0], y->e[1][0]) &&
         F32ApproxEq(x->e[1][1], y->e[1][1]) &&
         F32ApproxEq(x->e[1][2], y->e[1][2]) &&
         F32ApproxEq(x->e[1][3], y->e[1][3]) &&
         F32ApproxEq(x->e[2][0], y->e[2][0]) &&
         F32ApproxEq(x->e[2][1], y->e[2][1]) &&
         F32ApproxEq(x->e[2][2], y->e[2][2]) &&
         F32ApproxEq(x->e[2][3], y->e[2][3]) &&
         F32ApproxEq(x->e[3][0], y->e[3][0]) &&
         F32ApproxEq(x->e[3][1], y->e[3][1]) &&
         F32ApproxEq(x->e[3][2], y->e[3][2]) &&
         F32ApproxEq(x->e[3][3], y->e[3][3]);
}

B32 M4Eq(M4* x, M4* y) {
  return x->e[0][0] == y->e[0][0] &&
         x->e[0][1] == y->e[0][1] &&
         x->e[0][2] == y->e[0][2] &&
         x->e[0][2] == y->e[0][3] &&
         x->e[1][0] == y->e[1][0] &&
         x->e[1][1] == y->e[1][1] &&
         x->e[1][2] == y->e[1][2] &&
         x->e[1][3] == y->e[1][3] &&
         x->e[2][0] == y->e[2][0] &&
         x->e[2][1] == y->e[2][1] &&
         x->e[2][2] == y->e[2][2] &&
         x->e[2][3] == y->e[2][3] &&
         x->e[3][0] == y->e[3][0] &&
         x->e[3][1] == y->e[3][1] &&
         x->e[3][2] == y->e[3][2] &&
         x->e[3][3] == y->e[3][3];
}

#endif // CDEFAULT_MATH_IMPLEMENTATION
