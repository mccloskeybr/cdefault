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
F32 F32QuadBezier(F32 x, F32 y, F32 c, F32 t);
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
F64 F64QuadBezier(F64 x, F64 y, F64 c, F64 t);
B32 F64IsInteger(F64 x);

///////////////////////////////////////////////////////////////////////////////
// NOTE: Vec2
///////////////////////////////////////////////////////////////////////////////

#define V2_ZEROES (V2) { 0,  0 }
#define V2_ONES   (V2) { 1,  1 }
#define V2_MAX    (V2) { F32_MAX, F32_MAX }
#define V2_MAX    (V2) { F32_MAX, F32_MAX }

#define V2_X_POS  (V2) {+1,  0}
#define V2_Y_POS  (V2) { 0, +1}
#define V2_X_NEG  (V2) {-1,  0}
#define V2_Y_NEG  (V2) { 0, -1}

V2  V2Assign(F32 x, F32 y);
V2  V2Splat(F32 c);
V2  V2AddF32(V2 v, F32 c);
V2  V2SubF32(V2 v, F32 c);
V2  V2MultF32(V2 v, F32 c);
V2  V2DivF32(V2 v, F32 c);
V2  V2AddV2(V2 a, V2 b);
V2  V2SubV2(V2 a, V2 b);
V2  V2HadamardV2(V2 a, V2 b);
F32 V2DotV2(V2 a, V2 b);
F32 V2CrossV2(V2 a, V2 b);
F32 V2InnerMultV2(V2 a, V2 b);
B32 V2Eq(V2 a, V2 b);
B32 V2ApproxEq(V2 a, V2 b);
F32 V2LengthSq(V2 v);
F32 V2Length(V2 v);
V2  V2Abs(V2 v);
V2  V2Normalize(V2 v);
V2  V2Negate(V2 v);
F32 V2MinValue(V2 v);
F32 V2MaxValue(V2 v);
V2  V2Project(V2 a, V2 b); // NOTE: projects a onto b
V2  V2Clamp(V2 v, F32 min, F32 max);
V2  V2Rotate(V2 v, F32 angle_rad);
V2  V2RotateAroundV2(V2 v, V2 p, F32 angle_rad);
V2  V2RotateAroundV2Ex(V2 v, V2 p, F32 sin_value, F32 cos_value);
V2  V2Lerp(V2 a, V2 b, F32 t);
V2  V2QuadBezier(V2 a, V2 b, V2 control, F32 t);

///////////////////////////////////////////////////////////////////////////////
// NOTE: V3
///////////////////////////////////////////////////////////////////////////////

#define V3_ZEROES (V3) { 0,  0,  0}
#define V3_ONES   (V3) { 1,  1,  1}
#define V3_MIN    (V3) { F32_MIN, F32_MIN, F32_MIN }
#define V3_MAX    (V3) { F32_MAX, F32_MAX, F32_MAX }
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
V3  V3FromV2(V2 v, F32 z);
V3  V3FromHex(U32 x);
V3  V3AddF32(V3 v, F32 c);
V3  V3SubF32(V3 v, F32 c);
V3  V3MultF32(V3 v, F32 c);
V3  V3DivF32(V3 v, F32 c);
V3  V3AddV3(V3 a, V3 b);
V3  V3SubV3(V3 a, V3 b);
V3  V3HadamardV3(V3 a, V3 b);
F32 V3InnerMultV3(V3 a, V3 b);
F32 V3DotV3(V3 a, V3 b);
V3  V3CrossV3(V3 a, V3 b);
B32 V3ApproxEq(V3 a, V3 b);
B32 V3Eq(V3 a, V3 b);
F32 V3LengthSq(V3 v);
F32 V3Length(V3 v);
V3  V3Abs(V3 v);
V3  V3Normalize(V3 v);
V3  V3Negate(V3 v);
F32 V3MinValue(V3 v);
F32 V3MaxValue(V3 v);
B32 V3IsBetween(V3 v, V3 start, V3 end);
V3  V3Project(V3 a, V3 b); // NOTE: projects a onto b
V3  V3Clamp(V3 v, F32 min, F32 max);
B32 V3AreCollinear(V3 a, V3 b, V3 c);
F32 V3AngleBetween(V3 a, V3 b);
F32 V3AngleBetweenNorm(V3 a, V3 b, V3 normal);
V3  V3Lerp(V3 a, V3 b, F32 t);
V3  V3QuadBezier(V3 a, V3 b, V3 control, F32 t);
V3  V3RotateAroundAxis(V3 v, V3 axis, F32 angle_rad);
V3  V3RotateAroundV3Quat(V3 v, V3 p, V4 rot);
V3  V3RotateAroundV3M3(V3 v, V3 p, M3 rot);

///////////////////////////////////////////////////////////////////////////////
// NOTE: V4
///////////////////////////////////////////////////////////////////////////////

#define V4_QUAT_IDENT (V4) {0, 0, 0, 1}
#define V4_ONES   (V4) { 1,  1,  1,  1}
#define V4_ZEROES (V4) { 0,  0,  0,  0}
#define V4_MIN    (V4) { F32_MIN, F32_MIN, F32_MIN, F32_MIN }
#define V4_MAX    (V4) { F32_MAX, F32_MAX, F32_MAX, F32_MAX }
#define V4_X_POS  (V4) {+1,  0,  0,  0}
#define V4_Y_POS  (V4) { 0, +1,  0,  0}
#define V4_Z_POS  (V4) { 0,  0, +1,  0}
#define V4_W_POS  (V4) { 0,  0,  0, +1}
#define V4_X_NEG  (V4) {-1,  0,  0,  0}
#define V4_Y_NEG  (V4) { 0, -1,  0,  0}
#define V4_Z_NEG  (V4) { 0,  0, -1,  0}
#define V4_W_NEG  (V4) { 0,  0,  0, -1}

#define V4_TRANS  (V4) {0, 0, 0, 0}
#define V4_BLACK  (V4) {0, 0, 0, 1}
#define V4_WHITE  (V4) {1, 1, 1, 1}
#define V4_RED    (V4) {1, 0, 0, 1}
#define V4_GREEN  (V4) {0, 1, 0, 1}
#define V4_BLUE   (V4) {0, 0, 1, 1}

V4  V4Assign(F32 x, F32 y, F32 z, F32 w);
V4  V4Splat(F32 c);
V4  V4FromV2(V2 v, F32 z, F32 w);
V4  V4FromV3(V3 v, F32 w);
V4  V4AddF32(V4 v, F32 c);
V4  V4SubF32(V4 v, F32 c);
V4  V4MultF32(V4 v, F32 c);
V4  V4DivF32(V4 v, F32 c);
V4  V4AddV4(V4 a, V4 b);
V4  V4SubV4(V4 a, V4 b);
V4  V4HadamardV4(V4 a, V4 b);
V4  V4QuatMulV4(V4 a, V4 b);
F32 V4InnerMultV4(V4 a, V4 b);
F32 V4DotV4(V4 a, V4 b);
B32 V4ApproxEq(V4 a, V4 b);
B32 V4Eq(V4 a , V4 b);
F32 V4LengthSq(V4 v);
F32 V4Length(V4 v);
V4  V4Abs(V4 v);
V4  V4Normalize(V4 v);
V4  V4Negate(V4 v);
V4  V4Project(V4 a, V4 b); // NOTE: projects a onto b
V4  V4Clamp(V4 v, F32 min, F32 max);
V4  V4Lerp(V4 a, V4 b, F32 t);
V4  V4QuadBezier(V4 a, V4 b, V4 control, F32 t);
V4  V4Slerp(V4 a, V4 b, F32 t);
V4  V4RotateAroundAxis(V3 axis, F32 angle_rad);
V4  V4LookInDir(V3 dir);
V4  V4LookAt(V3 pos, V3 target);

///////////////////////////////////////////////////////////////////////////////
// NOTE: M2
///////////////////////////////////////////////////////////////////////////////

#define M2_IDENTITY (M2) {1, 0, 0, \
                          0, 1, 0, \
                          0, 0, 1 }

M2  M2AddM2(M2 a, M2 b);
M2  M2SubM2(M2 a, M2 b);
M2  M2MultF32(M2 m, F32 c);
M2  M2MultM2(M2 a, M2 b);
V2  M2MultV2(M2 m, V2 v);
M2  M2Transpose(M2 m);
F32 M2Det(M2 m);
M2  M2Invert(M2 m);
B32 M2ApproxEq(M2 a, M2 b);
B32 M2Eq(M2 a, M2 b);

///////////////////////////////////////////////////////////////////////////////
// NOTE: M3
///////////////////////////////////////////////////////////////////////////////

#define M3_IDENTITY (M3) {1, 0, 0, \
                          0, 1, 0, \
                          0, 0, 1 }

M3  M3FromQuaternion(V4 q);
M3  M3AddM3(M3 a, M3 b);
M3  M3SubM3(M3 a, M3 b);
M3  M3MultF32(M3 m, F32 c);
M3  M3MultM3(M3 a, M3 b);
V3  M3MultV3(M3 m, V3 v);
M3  M3Transpose(M3 m);
F32 M3Det(M3 m);
M3  M3Invert(M3 m);
B32 M3ApproxEq(M3 a, M3 b);
B32 M3Eq(M3 a, M3 b);

///////////////////////////////////////////////////////////////////////////////
// NOTE: M4
///////////////////////////////////////////////////////////////////////////////

#define M4_IDENTITY (M4) {1, 0, 0, 0, \
                          0, 1, 0, 0, \
                          0, 0, 1, 0, \
                          0, 0, 0, 1}

M4  M4FromTransform(V3 pos, V4 rot, V3 scale);
M4  M4Perspective(F32 fov_y_rad, F32 aspect_ratio, F32 near_plane, F32 far_plane);
M4  M4Orthographic(F32 left, F32 right, F32 bottom, F32 top, F32 near_plane, F32 far_plane);
M4  M4LookAt(V3 eye, V3 target, V3 up);
M4  M4AddM4(M4 a, M4 b);
M4  M4SubM4(M4 a, M4 b);
M4  M4MultF32(M4 m, F32 c);
M4  M4MultM4(M4 a, M4 b);
V4  M4MultV4(M4 a, V4 b);
M4  M4Transpose(M4 m);
F32 M4Det(M4 m);
M4  M4Invert(M4 m);
B32 M4ApproxEq(M4 a, M4 b);
B32 M4Eq(M4 a, M4 b);

#endif // CDEFAULT_MATH_H_

#ifdef CDEFAULT_MATH_IMPLEMENTATION
#undef CDEFAULT_MATH_IMPLEMENTATION

///////////////////////////////////////////////////////////////////////////////
// NOTE: Num implementation
///////////////////////////////////////////////////////////////////////////////

B32 F32ApproxEq(F32 x, F32 y) {
  return F32Abs(x - y) < 0.00001f;
}

F32 F32Abs(F32 x) {
  return fabsf(x);
}

F32 F32Pow(F32 x, F32 y) {
  return powf(x, y);
}

F32 F32Sqrt(F32 x) {
  return sqrtf(x);
}

F32 F32Sin(F32 x) {
  return sinf(x);
}

F32 F32Cos(F32 x) {
  return cosf(x);
}

F32 F32Tan(F32 x) {
  return tanf(x);
}

F32 F32ArcSin(F32 x) {
  return asinf(x);
}

F32 F32ArcCos(F32 x) {
  return acosf(x);
}

F32 F32ArcTan(F32 x) {
  return atanf(x);
}

F32 F32ArcTan2(F32 y, F32 x) {
  return atan2f(y, x);
}

F32 F32ToRad(F32 deg) {
  return deg * (F32_PI / 180.0f);
}

F32 F32ToDeg(F32 rad) {
  return rad * (180.0f / F32_PI);
}

F32 F32Ceil(F32 x) {
  return ceilf(x);
}

F32 F32Floor(F32 x) {
  return floorf(x);
}

F32 F32Round(F32 x) {
  return F32Floor(x + 0.5f);
}

F32 F32MapRange(F32 x, F32 a_min, F32 a_max, F32 b_min, F32 b_max) {
  return b_min + ((x - a_min) / (a_max - a_min)) * (b_max - b_min);
}

F32 F32Lerp(F32 x, F32 y, F32 t) {
  return x + ((y - x) * t);
}

F32 F32QuadBezier(F32 x, F32 y, F32 c, F32 t) {
  F32 x_1 = F32Lerp(x, c, t);
  F32 x_2 = F32Lerp(c, y, t);
  return F32Lerp(x_1, x_2, t);
}

B32 F32IsInteger(F32 x) {
  return x == F32Floor(x);
}

B64 F64ApproxEq(F64 x, F64 y) {
  return F64Abs(x - y) < 0.00001;
}

F64 F64Abs(F64 x) {
  return fabs(x);
}

F64 F64Pow(F64 x, F64 y) {
  return pow(x, y);
}

F64 F64Sqrt(F64 x) {
  return sqrt(x);
}

F64 F64Sin(F64 x) {
  return sin(x);
}

F64 F64Cos(F64 x) {
  return cos(x);
}

F64 F64Tan(F64 x) {
  return tan(x);
}

F64 F64ArcSin(F64 x) {
  return asin(x);
}

F64 F64ArcCos(F64 x) {
  return acos(x);
}

F64 F64ArcTan(F64 x) {
  return atan(x);
}

F64 F64ArcTan2(F64 y, F64 x) {
  return atan2(y, x);
}

F64 F64ToRad(F64 deg) {
  return deg * (F64_PI / 180.0);
}

F64 F64ToDeg(F64 rad) {
  return rad * (180.0 / F64_PI);
}

F64 F64Ceil(F64 x) {
  return ceil(x);
}

F64 F64Floor(F64 x) {
  return floor(x);
}

F64 F64Round(F64 x) {
  return F64Floor(x + 0.5);
}

F64 F64MapRange(F64 x, F64 a_min, F64 a_max, F64 b_min, F64 b_max) {
  return b_min + ((x - a_min) / (a_max - a_min)) * (b_max - b_min);
}

F64 F64Lerp(F64 x, F64 y, F64 t) {
  return x + ((y - x) * t);
}

F64 F64QuadBezier(F64 x, F64 y, F64 c, F64 t) {
  F64 x_1 = F64Lerp(x, c, t);
  F64 x_2 = F64Lerp(c, y, t);
  return F64Lerp(x_1, x_2, t);
}

B32 F64IsInteger(F64 x) {
  return x == F64Floor(x);
}

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

V2 V2AddF32(V2 v, F32 c) {
  V2 result;
  result.x = v.x + c;
  result.y = v.y + c;
  return result;
}

V2 V2SubF32(V2 v, F32 c) {
  V2 result;
  result.x = v.x - c;
  result.y = v.y - c;
  return result;
}

V2 V2MultF32(V2 v, F32 c) {
  V2 result;
  result.x = v.x * c;
  result.y = v.y * c;
  return result;
}

V2 V2DivF32(V2 v, F32 c) {
  DEBUG_ASSERT(c != 0);
  V2 result;
  result.x = v.x / c;
  result.y = v.y / c;
  return result;
}

V2 V2AddV2(V2 a, V2 b) {
  V2 result;
  result.x = a.x + b.x;
  result.y = a.y + b.y;
  return result;
}

V2 V2SubV2(V2 a, V2 b) {
  V2 result;
  result.x = a.x - b.x;
  result.y = a.y - b.y;
  return result;
}

V2 V2HadamardV2(V2 a, V2 b) {
  V2 result;
  result.x = a.x * b.x;
  result.y = a.y * b.y;
  return result;
}

F32 V2DotV2(V2 a, V2 b) {
  return (a.x * b.x) + (a.y * b.y);
}

F32 V2CrossV2(V2 a, V2 b) {
  return (a.x * b.y) - (a.y * b.x);
}

F32 V2InnerMultV2(V2 a, V2 b) {
  return V2DotV2(a, b);
}

B32 V2Eq(V2 a, V2 b) {
  return (a.x == b.x) &&
         (a.y == b.y);
}

B32 V2ApproxEq(V2 a, V2 b) {
  return F32ApproxEq(a.x, b.x) &&
         F32ApproxEq(a.y, b.y);
}

F32 V2LengthSq(V2 v) {
  return V2DotV2(v, v);
}

F32 V2Length(V2 v) {
  return F32Sqrt(V2LengthSq(v));
}

V2 V2Abs(V2 v) {
  V2 result;
  result.x = F32Abs(v.x);
  result.y = F32Abs(v.y);
  return result;
}

V2 V2Normalize(V2 v) {
  return V2DivF32(v, V2Length(v));
}

V2 V2Negate(V2 v) {
  return V2MultF32(v, -1);
}

F32 V2MinValue(V2 v) {
  return MIN(v.x, v.y);
}

F32 V2MaxValue(V2 v) {
  return MAX(v.x, v.y);
}

V2 V2Clamp(V2 v, F32 min, F32 max) {
  V2 result;
  result.x = CLAMP(min, v.x, max);
  result.y = CLAMP(min, v.y, max);
  return result;
}

V2 V2Project(V2 a, V2 b) {
  return V2MultF32(b, V2DotV2(a, b) / V2DotV2(b, b));
}
 
V2 V2Rotate(V2 v, F32 angle_rad) {
  V2 result;
  F32 s = F32Sin(angle_rad);
  F32 c = F32Cos(angle_rad);
  result.x = v.x * c - v.y * s;
  result.y = v.x * c + v.y * s;
  return result;
}

V2 V2RotateAroundV2(V2 v, V2 p, F32 angle_rad) {
  F32 sin_value = F32Sin(angle_rad);
  F32 cos_value = F32Cos(angle_rad);
  return V2RotateAroundV2Ex(v, p, sin_value, cos_value);
}

V2 V2RotateAroundV2Ex(V2 v, V2 p, F32 sin_value, F32 cos_value) {
  V2 result;
  V2 rel_v = V2SubV2(v, p);
  result.x = rel_v.x * cos_value - rel_v.y * sin_value;
  result.y = rel_v.x * sin_value + rel_v.y * cos_value;
  result = V2AddV2(result, p);
  return result;
}

V2 V2Lerp(V2 a, V2 b, F32 t) {
  V2 result;
  result.x = F32Lerp(a.x, b.x, t);
  result.y = F32Lerp(a.y, b.y, t);
  return result;
}

V2 V2QuadBezier(V2 a, V2 b, V2 control, F32 t) {
  V2 result;
  result.x = F32QuadBezier(a.x, b.x, control.x, t);
  result.y = F32QuadBezier(a.y, b.y, control.y, t);
  return result;
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

V3 V3FromV2(V2 v, F32 z) {
  V3 result;
  result.x = v.x;
  result.y = v.y;
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

V3 V3AddF32(V3 v, F32 c) {
  V3 result;
  result.x = v.x + c;
  result.y = v.y + c;
  result.z = v.z + c;
  return result;
}

V3 V3SubF32(V3 v, F32 c) {
  V3 result;
  result.x = v.x - c;
  result.y = v.y - c;
  result.z = v.z - c;
  return result;
}

V3 V3MultF32(V3 v, F32 c) {
  V3 result;
  result.x = v.x * c;
  result.y = v.y * c;
  result.z = v.z * c;
  return result;
}

V3 V3DivF32(V3 v, F32 c) {
  DEBUG_ASSERT(c != 0);
  V3 result;
  result.x = v.x / c;
  result.y = v.y / c;
  result.z = v.z / c;
  return result;
}

V3 V3AddV3(V3 a, V3 b) {
  V3 result;
  result.x = a.x + b.x;
  result.y = a.y + b.y;
  result.z = a.z + b.z;
  return result;
}

V3 V3SubV3(V3 a, V3 b) {
  V3 result;
  result.x = a.x - b.x;
  result.y = a.y - b.y;
  result.z = a.z - b.z;
  return result;
}

V3 V3HadamardV3(V3 a, V3 b) {
  V3 result;
  result.x = a.x * b.x;
  result.y = a.y * b.y;
  result.z = a.z * b.z;
  return result;
}

F32 V3InnerMultV3(V3 a, V3 b) {
  return V3DotV3(a, b);
}

F32 V3DotV3(V3 a, V3 b) {
  return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

V3 V3CrossV3(V3 a, V3 b) {
  V3 result;
  result.x = (a.y * b.z) - (a.z * b.y);
  result.y = (a.z * b.x) - (a.x * b.z);
  result.z = (a.x * b.y) - (a.y * b.x);
  return result;
}

B32 V3ApproxEq(V3 a, V3 b) {
  return F32ApproxEq(a.x, b.x) &&
         F32ApproxEq(a.y, b.y) &&
         F32ApproxEq(a.z, b.z);
}

B32 V3Eq(V3 a, V3 b) {
  return a.x == b.x &&
         a.y == b.y &&
         a.z == b.z;
}

F32 V3LengthSq(V3 v) {
  return V3DotV3(v, v);
}

F32 V3Length(V3 v) {
  return F32Sqrt(V3LengthSq(v));
}

V3 V3Abs(V3 v) {
  V3 result;
  result.x = F32Abs(v.x);
  result.y = F32Abs(v.y);
  result.z = F32Abs(v.z);
  return result;
}

V3 V3Normalize(V3 v) {
  return V3DivF32(v, V3Length(v));
}

V3 V3Negate(V3 v) {
  return V3MultF32(v, -1);
}

F32 V3MinValue(V3 x) {
  return MIN3(x.x, x.y, x.z);
}

F32 V3MaxValue(V3 x) {
  return MAX3(x.x, x.y, x.z);
}

B32 V3IsBetween(V3 v, V3 start, V3 end) {
  if (!V3AreCollinear(v, start, end)) { return false; }
  V3 ij         = V3SubV3(end, start);
  V3 ik         = V3SubV3(v, start);
  F32 ij_dot_ik = V3DotV3(ij, ik);
  F32 ij_dot_ij = V3DotV3(ij, ij);
  return 0 < ij_dot_ik && ij_dot_ik < ij_dot_ij;
}

// https://en.wikibooks.org/wiki/Linear_Algebra/Orthogonal_Projection_Onto_a_Line
V3 V3Project(V3 a, V3 b) {
  return V3MultF32(b, V3DotV3(a, b) / V3DotV3(b, b));
}

V3 V3Clamp(V3 v, F32 min, F32 max) {
  V3 result;
  result.x = CLAMP(min, v.x, max);
  result.y = CLAMP(min, v.y, max);
  result.z = CLAMP(min, v.z, max);
  return result;
}

B32 V3AreCollinear(V3 a, V3 b, V3 c) {
  V3 ab = V3SubV3(b, a);
  V3 ac = V3SubV3(c, a);
  return V3Eq(V3CrossV3(ab, ac), V3_ZEROES);
}

F32 V3AngleBetween(V3 a, V3 b) {
  return F32ArcCos(V3DotV3(a, b));
}

F32 V3AngleBetweenNorm(V3 a, V3 b, V3 normal) {
  V3  cross = V3CrossV3(a, b);
  F32 dot   = V3DotV3(a, b);
  F32 angle = F32ArcTan2(V3Length(cross), dot);
  if (V3DotV3(normal, cross) < 0) { angle = -angle; }
  return angle;
}

V3 V3Lerp(V3 a, V3 b, F32 t) {
  V3 result;
  result.x = F32Lerp(a.x, b.x, t);
  result.y = F32Lerp(a.y, b.y, t);
  result.z = F32Lerp(a.z, b.z, t);
  return result;
}

V3 V3QuadBezier(V3 a, V3 b, V3 control, F32 t) {
  V3 result;
  result.x = F32QuadBezier(a.x, b.x, control.x, t);
  result.y = F32QuadBezier(a.y, b.y, control.y, t);
  result.z = F32QuadBezier(a.z, b.z, control.z, t);
  return result;
}

// https://stackoverflow.com/questions/69245724/rotate-a-vector-around-an-axis-in-3d-space
V3 V3RotateAroundAxis(V3 v, V3 axis, F32 angle_rad) {
  V3 cross        = V3CrossV3(axis, v);
  V3 double_cross = V3CrossV3(axis, cross);
  V3 a            = V3MultF32(cross, F32Sin(angle_rad));
  V3 b            = V3MultF32(double_cross, (1.0f - F32Cos(angle_rad)));
  V3 result;
  result.x = v.x + a.x + b.x;
  result.y = v.y + a.y + b.y;
  result.z = v.z + a.z + b.z;
  return result;
}

V3 V3RotateAroundV3V4(V3 v, V3 p, V4 rot) {
  M3 rot_mat = M3FromQuaternion(rot);
  return V3RotateAroundV3M3(v, p, rot_mat);
}

V3 V3RotateAroundV3M3(V3 v, V3 p, M3 rot) {
  v = V3SubV3(v, p);
  v = M3MultV3(rot, v);
  v = V3AddV3(v, p);
  return v;
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


V4 V4FromV2(V2 v, F32 z, F32 w) {
  V4 result;
  result.x = v.x;
  result.y = v.y;
  result.z = z;
  result.w = w;
  return result;
}

V4 V4FromV3(V3 v, F32 w) {
  V4 result;
  result.x = v.x;
  result.y = v.y;
  result.z = v.z;
  result.w = w;
  return result;
}

V4 V4AddV4(V4 a, V4 b) {
  V4 result;
  result.x = a.x + b.x;
  result.y = a.y + b.y;
  result.z = a.z + b.z;
  result.w = a.w + b.w;
  return result;
}

V4 V4AddF32(V4 v, F32 c) {
  V4 result;
  result.x = v.x + c;
  result.y = v.y + c;
  result.z = v.z + c;
  result.w = v.w + c;
  return result;
}

V4 V4MultF32(V4 v, F32 c) {
  V4 result;
  result.x = v.x * c;
  result.y = v.y * c;
  result.z = v.z * c;
  result.w = v.w * c;
  return result;
}

V4 V4DivF32(V4 v, F32 c) {
  V4 result;
  result.x = v.x / c;
  result.y = v.y / c;
  result.z = v.z / c;
  result.w = v.w / c;
  return result;
}

V4 V4SubF32(V4 v, F32 c) {
  V4 result;
  result.x = v.x - c;
  result.y = v.y - c;
  result.z = v.z - c;
  result.w = v.w - c;
  return result;
}

V4 V4SubV4(V4 a, V4 b) {
  V4 result;
  result.x = a.x - b.x;
  result.y = a.y - b.y;
  result.z = a.z - b.z;
  result.w = a.w - b.w;
  return result;
}

F32 V4LengthSq(V4 v) {
  return V4DotV4(v, v);
}

F32 V4Length(V4 v) {
  return F32Sqrt(V4LengthSq(v));
}

V4 V4Hadamard(V4 a, V4 b) {
  V4 result;
  result.x = a.x * b.x;
  result.y = a.y * b.y;
  result.z = a.z * b.z;
  result.w = a.w * b.w;
  return result;
}

V4 V4HadamardV4(V4 a, V4 b) {
  V4 result;
  result.x = a.x * b.x;
  result.y = a.y * b.y;
  result.z = a.z * b.z;
  result.w = a.w * b.w;
  return result;
}

// https://stackoverflow.com/questions/19956555/how-to-multiply-two-quaternions
V4 V4QuatMulV4(V4 a, V4 b) {
  V4 result;
  result.x = (a.w * b.x) + (a.x * b.w) + (a.y * b.z) - (a.z * b.y);
  result.y = (a.w * b.y) - (a.x * b.z) + (a.y * b.w) + (a.z * b.x);
  result.z = (a.w * b.z) + (a.x * b.y) - (a.y * b.x) + (a.z * b.w);
  result.w = (a.w * b.w) - (a.x * b.x) - (a.y * b.y) - (a.z * b.z);
  return result;
}

F32 V4InnerMultV4(V4 a, V4 b) {
  return V4DotV4(a, b);
}

F32 V4DotV4(V4 a, V4 b) {
  return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}

B32 V4Eq(V4 a , V4 b) {
  return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
}

B32 V4ApproxEq(V4 a, V4 b) {
  return F32ApproxEq(a.x, b.x) &&
         F32ApproxEq(a.y, b.y) &&
         F32ApproxEq(a.z, b.z) &&
         F32ApproxEq(a.w, b.w);
}

V4 V4Abs(V4 v) {
  V4 result;
  result.x = F32Abs(v.x);
  result.y = F32Abs(v.y);
  result.z = F32Abs(v.z);
  return result;
}

V4 V4Normalize(V4 v) {
  return V4DivF32(v, V4Length(v));
}

V4 V4Negate(V4 v) {
  return V4MultF32(v, -1);
}

V4 V4Project(V4 a, V4 b) {
  return V4MultF32(b, V4DotV4(a, b) / V4DotV4(b, b));
}

V4 V4Clamp(V4 v, F32 min, F32 max) {
  V4 result;
  result.x = CLAMP(min, v.x, max);
  result.y = CLAMP(min, v.y, max);
  result.z = CLAMP(min, v.z, max);
  result.w = CLAMP(min, v.w, max);
  return result;
}

V4 V4Lerp(V4 a, V4 b, F32 t) {
  V4 result;
  result.x = F32Lerp(a.x, b.x, t);
  result.y = F32Lerp(a.y, b.y, t);
  result.z = F32Lerp(a.z, b.z, t);
  result.w = F32Lerp(a.w, b.w, t);
  return result;
}

V4 V4QuadBezier(V4 a, V4 b, V4 control, F32 t) {
  V4 result;
  result.x = F32QuadBezier(a.x, b.x, control.x, t);
  result.y = F32QuadBezier(a.y, b.y, control.y, t);
  result.z = F32QuadBezier(a.z, b.z, control.z, t);
  result.w = F32QuadBezier(a.w, b.w, control.w, t);
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
  F32 angle_rad = F32ArcCos(dot);
  F32 a_scalar = F32Sin((1.0f - t) * angle_rad) / F32Sin(angle_rad);
  F32 b_scalar = F32Sin(t * angle_rad) / F32Sin(angle_rad);
  return V4AddV4(V4MultF32(a, a_scalar), V4MultF32(b, b_scalar));
}

V4 V4RotateAroundAxis(V3 axis, F32 angle_rad) {
  F32 factor = F32Sin(angle_rad / 2.0f);
  V4 result;
  result.x = factor * axis.x;
  result.y = factor * axis.y;
  result.z = factor * axis.z;
  result.w = F32Cos(angle_rad / 2.0f);
  return V4Normalize(result);
}

// https://stackoverflow.com/questions/12435671/quaternion-lookat-function
V4 V4LookInDir(V3 dir) {
  ASSERT(F32ApproxEq(V3LengthSq(dir), 1.0f));
  F32 dot = V3DotV3(V3_Y_NEG, dir);
  if (F32ApproxEq(dot, -1)) {
    // TODO: this is probably wrong?
    return V4Assign(0, 0, 1, 0);
  } else if (F32ApproxEq(dot, +1)) {
    return V4_QUAT_IDENT;
  }

  V3 axis = V3Normalize(V3CrossV3(V3_Y_NEG, dir));

  F32 angle = F32ArcCos(dot);
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
// NOTE: M2 implementation
///////////////////////////////////////////////////////////////////////////////

M2 M2AddM2(M2 a, M2 b) {
  M2 result;
  result.e[0][0] = a.e[0][0] + b.e[0][0];
  result.e[0][1] = a.e[0][1] + b.e[0][1];
  result.e[1][0] = a.e[1][0] + b.e[1][0];
  result.e[1][1] = a.e[1][1] + b.e[1][1];
  return result;
}

M2 M2SubM2(M2 a, M2 b) {
  M2 result;
  result.e[0][0] = a.e[0][0] - b.e[0][0];
  result.e[0][1] = a.e[0][1] - b.e[0][1];
  result.e[1][0] = a.e[1][0] - b.e[1][0];
  result.e[1][1] = a.e[1][1] - b.e[1][1];
  return result;
}

M2 M2MultF32(M2 m, F32 c) {
  M2 result;
  result.e[0][0] = m.e[0][0] * c;
  result.e[0][1] = m.e[0][1] * c;
  result.e[1][0] = m.e[1][0] * c;
  result.e[1][1] = m.e[1][1] * c;
  return result;
}

M2 M2MultM2(M2 a, M2 b) {
  M2 result;
  result.e[0][0] = (a.e[0][0] * b.e[0][0]) + (a.e[0][1] * b.e[1][0]);
  result.e[0][1] = (a.e[0][0] * b.e[0][1]) + (a.e[0][1] * b.e[1][1]);
  result.e[1][0] = (a.e[1][0] * b.e[0][0]) + (a.e[1][1] * b.e[1][0]);
  result.e[1][1] = (a.e[1][0] * b.e[0][1]) + (a.e[1][1] * b.e[1][1]);
  return result;
}

V2 M2MultV2(M2 m, V2 v) {
  V2 result;
  result.e[0] = (m.e[0][0] * v.e[0]) + (m.e[0][1] * v.e[1]);
  result.e[1] = (m.e[1][0] * v.e[0]) + (m.e[1][1] * v.e[1]);
  return result;
}

M2 M2Transpose(M2 m) {
  M2 result;
  result.e[0][0] = m.e[0][0];
  result.e[0][1] = m.e[1][0];
  result.e[1][0] = m.e[0][1];
  result.e[1][1] = m.e[1][1];
  return result;
}

F32 M2Det(M2 m) {
  return (m.e[0][0] * m.e[1][1]) - (m.e[0][1] * m.e[1][0]);
}

M2 M2Invert(M2 m) {
  F32 det = M2Det(m);
  DEBUG_ASSERT(det != 0);
  F32 det_inv = 1.0f / det;
  M2 result;
  result.e[0][0] = det_inv * +m.e[1][1];
  result.e[0][1] = det_inv * -m.e[0][1];
  result.e[1][0] = det_inv * -m.e[1][0];
  result.e[1][1] = det_inv * +m.e[0][0];
  return result;
}

B32 M2ApproxEq(M2 a, M2 b) {
  return F32ApproxEq(a.e[0][0], b.e[0][0]) &&
         F32ApproxEq(a.e[0][1], b.e[0][1]) &&
         F32ApproxEq(a.e[1][0], b.e[1][0]) &&
         F32ApproxEq(a.e[1][1], b.e[1][1]);
}

B32 M2Eq(M2 a, M2 b) {
  return a.e[0][0] == b.e[0][0] &&
         a.e[0][1] == b.e[0][1] &&
         a.e[1][0] == b.e[1][0] &&
         a.e[1][1] == b.e[1][1];
}

///////////////////////////////////////////////////////////////////////////////
// NOTE: M3 implementation
///////////////////////////////////////////////////////////////////////////////

M3 M3AddM3(M3 a, M3 b) {
  M3 result;
  result.e[0][0] = a.e[0][0] + b.e[0][0];
  result.e[0][1] = a.e[0][1] + b.e[0][1];
  result.e[0][2] = a.e[0][2] + b.e[0][2];
  result.e[1][0] = a.e[1][0] + b.e[1][0];
  result.e[1][1] = a.e[1][1] + b.e[1][1];
  result.e[1][2] = a.e[1][2] + b.e[1][2];
  result.e[2][0] = a.e[2][0] + b.e[2][0];
  result.e[2][1] = a.e[2][1] + b.e[2][1];
  result.e[2][2] = a.e[2][2] + b.e[2][2];
  return result;
}

M3 M3SubM3(M3 a, M3 b) {
  M3 result;
  result.e[0][0] = a.e[0][0] - b.e[0][0];
  result.e[0][1] = a.e[0][1] - b.e[0][1];
  result.e[0][2] = a.e[0][2] - b.e[0][2];
  result.e[1][0] = a.e[1][0] - b.e[1][0];
  result.e[1][1] = a.e[1][1] - b.e[1][1];
  result.e[1][2] = a.e[1][2] - b.e[1][2];
  result.e[2][0] = a.e[2][0] - b.e[2][0];
  result.e[2][1] = a.e[2][1] - b.e[2][1];
  result.e[2][2] = a.e[2][2] - b.e[2][2];
  return result;
}

M3 M3MultF32(M3 m, F32 c) {
  M3 result;
  result.e[0][0] = m.e[0][0] * c;
  result.e[0][1] = m.e[0][1] * c;
  result.e[0][2] = m.e[0][2] * c;
  result.e[1][0] = m.e[1][0] * c;
  result.e[1][1] = m.e[1][1] * c;
  result.e[1][2] = m.e[1][2] * c;
  result.e[2][0] = m.e[2][0] * c;
  result.e[2][1] = m.e[2][1] * c;
  result.e[2][2] = m.e[2][2] * c;
  return result;
}

M3 M3MultM3(M3 a, M3 b) {
  M3 result;
  result.e[0][0] = (a.e[0][0] * b.e[0][0]) + (a.e[0][1] * b.e[1][0]) + (a.e[0][2] * b.e[2][0]);
  result.e[0][1] = (a.e[0][0] * b.e[0][1]) + (a.e[0][1] * b.e[1][1]) + (a.e[0][2] * b.e[2][1]);
  result.e[0][2] = (a.e[0][0] * b.e[0][2]) + (a.e[0][1] * b.e[1][2]) + (a.e[0][2] * b.e[2][2]);
  result.e[1][0] = (a.e[1][0] * b.e[0][0]) + (a.e[1][1] * b.e[1][0]) + (a.e[1][2] * b.e[2][0]);
  result.e[1][1] = (a.e[1][0] * b.e[0][1]) + (a.e[1][1] * b.e[1][1]) + (a.e[1][2] * b.e[2][1]);
  result.e[1][2] = (a.e[1][0] * b.e[0][2]) + (a.e[1][1] * b.e[1][2]) + (a.e[1][2] * b.e[2][2]);
  result.e[2][0] = (a.e[2][0] * b.e[0][0]) + (a.e[2][1] * b.e[1][0]) + (a.e[2][2] * b.e[2][0]);
  result.e[2][1] = (a.e[2][0] * b.e[0][1]) + (a.e[2][1] * b.e[1][1]) + (a.e[2][2] * b.e[2][1]);
  result.e[2][2] = (a.e[2][0] * b.e[0][2]) + (a.e[2][1] * b.e[1][2]) + (a.e[2][2] * b.e[2][2]);
  return result;
}

V3 M3MultV3(M3 m, V3 v) {
  V3 result;
  result.e[0] = (m.e[0][0] * v.e[0]) + (m.e[0][1] * v.e[1]) + (m.e[0][2] * v.e[2]);
  result.e[1] = (m.e[1][0] * v.e[0]) + (m.e[1][1] * v.e[1]) + (m.e[1][2] * v.e[2]);
  result.e[2] = (m.e[2][0] * v.e[0]) + (m.e[2][1] * v.e[1]) + (m.e[2][2] * v.e[2]);
  return result;
}

M3 M3Transpose(M3 m) {
  M3 result;
  result.e[0][0] = m.e[0][0];
  result.e[0][1] = m.e[1][0];
  result.e[0][2] = m.e[2][0];
  result.e[1][0] = m.e[0][1];
  result.e[1][1] = m.e[1][1];
  result.e[1][2] = m.e[2][1];
  result.e[2][0] = m.e[0][2];
  result.e[2][1] = m.e[1][2];
  result.e[2][2] = m.e[2][2];
  return result;
}

// https://www.chilimath.com/lessons/advanced-algebra/determinant-3x3-matrix/
F32 M3Det(M3 m) {
  F32 sub_0 = (m.e[1][1] * m.e[2][2]) - (m.e[1][2] * m.e[2][1]);
  F32 sub_1 = (m.e[1][0] * m.e[2][2]) - (m.e[1][2] * m.e[2][0]);
  F32 sub_2 = (m.e[1][0] * m.e[2][1]) - (m.e[1][1] * m.e[2][0]);
  return (m.e[0][0] * sub_0) - (m.e[0][1] * sub_1) + (m.e[0][2] * sub_2);
}

M3 M3Invert(M3 m) {
  M3 adj;
  adj.e[0][0] = +((m.e[1][1] * m.e[2][2]) - (m.e[1][2] * m.e[2][1]));
  adj.e[1][0] = -((m.e[1][0] * m.e[2][2]) - (m.e[1][2] * m.e[2][0]));
  adj.e[2][0] = +((m.e[1][0] * m.e[2][1]) - (m.e[1][1] * m.e[2][0]));
  adj.e[0][1] = -((m.e[0][1] * m.e[2][2]) - (m.e[0][2] * m.e[2][1]));
  adj.e[1][1] = +((m.e[0][0] * m.e[2][2]) - (m.e[0][2] * m.e[2][0]));
  adj.e[2][1] = -((m.e[0][0] * m.e[2][1]) - (m.e[0][1] * m.e[2][0]));
  adj.e[0][2] = +((m.e[0][1] * m.e[1][2]) - (m.e[0][2] * m.e[1][1]));
  adj.e[1][2] = -((m.e[0][0] * m.e[1][2]) - (m.e[0][2] * m.e[1][0]));
  adj.e[2][2] = +((m.e[0][0] * m.e[1][1]) - (m.e[0][1] * m.e[1][0]));

  F32 det = (m.e[0][0] * adj.e[0][0]) + (m.e[0][1] * adj.e[1][0]) + (m.e[0][2] * adj.e[2][0]);
  DEBUG_ASSERT(det != 0);
  F32 inv_det = 1.0f / det;

  M3 result;
  for (S32 i = 0; i < 9; i++) { result.i[i] = inv_det * adj.i[i]; }
  return result;
}

// https://automaticaddison.com/how-to-convert-a-quaternion-to-a-rotation-matrix/
M3 M3FromQuaternion(V4 q) {
  M3 result;
  result.e[0][0] = (2.0f * ((q.w * q.w) + (q.x * q.x))) - 1.0f;
  result.e[0][1] = (2.0f * ((q.x * q.y) - (q.w * q.z)));
  result.e[0][2] = (2.0f * ((q.x * q.z) + (q.w * q.y)));
  result.e[1][0] = (2.0f * ((q.x * q.y) + (q.w * q.z)));
  result.e[1][1] = (2.0f * ((q.w * q.w) + (q.y * q.y))) - 1.0f;
  result.e[1][2] = (2.0f * ((q.y * q.z) - (q.w * q.x)));
  result.e[2][0] = (2.0f * ((q.x * q.z) - (q.w * q.y)));
  result.e[2][1] = (2.0f * ((q.y * q.z) + (q.w * q.x)));
  result.e[2][2] = (2.0f * ((q.w * q.w) + (q.z * q.z))) - 1.0f;
  return result;
}

B32 M3ApproxEq(M3 a, M3 b) {
  return F32ApproxEq(a.e[0][0], b.e[0][0]) &&
         F32ApproxEq(a.e[0][1], b.e[0][1]) &&
         F32ApproxEq(a.e[0][2], b.e[0][2]) &&
         F32ApproxEq(a.e[1][0], b.e[1][0]) &&
         F32ApproxEq(a.e[1][1], b.e[1][1]) &&
         F32ApproxEq(a.e[1][2], b.e[1][2]) &&
         F32ApproxEq(a.e[2][0], b.e[2][0]) &&
         F32ApproxEq(a.e[2][1], b.e[2][1]) &&
         F32ApproxEq(a.e[2][2], b.e[2][2]);
}

B32 M3Eq(M3 a, M3 b) {
  return a.e[0][0] == b.e[0][0] &&
         a.e[0][1] == b.e[0][1] &&
         a.e[0][2] == b.e[0][2] &&
         a.e[1][0] == b.e[1][0] &&
         a.e[1][1] == b.e[1][1] &&
         a.e[1][2] == b.e[1][2] &&
         a.e[2][0] == b.e[2][0] &&
         a.e[2][1] == b.e[2][1] &&
         a.e[2][2] == b.e[2][2];
}

///////////////////////////////////////////////////////////////////////////////
// NOTE: M4 implementation
///////////////////////////////////////////////////////////////////////////////

// TODO: faster way to do this / check compiler output?
M4 M4FromTransform(V3 pos, V4 rot, V3 scale) {
  M3 scale_m;
  MEMORY_ZERO_STRUCT(&scale_m);
  scale_m.e[0][0] = scale.x;
  scale_m.e[1][1] = scale.y;
  scale_m.e[2][2] = scale.z;

  M3 rot_m     = M3FromQuaternion(rot);
  M3 rot_scale = M3MultM3(rot_m, scale_m);

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

  result.e[0][3] = pos.x;
  result.e[1][3] = pos.y;
  result.e[2][3] = pos.z;
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

M4 M4LookAt(V3 eye, V3 target, V3 up) {
  V3 z = V3Normalize(V3SubV3(eye, target));
  V3 y = up;
  V3 x = V3CrossV3(y, z);

  y = V3CrossV3(z, x);
  x = V3Normalize(x);
  y = V3Normalize(y);

  F32 a = -V3DotV3(x, eye);
  F32 b = -V3DotV3(y, eye);
  F32 c = -V3DotV3(z, eye);

  M4 result = {
    x.x, x.y, x.z, a,
    y.x, y.y, y.z, b,
    z.x, z.y, z.z, c,
      0,   0,   0, 1,
  };
  return result;
}

M4 M4AddM4(M4 a, M4 b) {
  M4 result;
  result.e[0][0] = a.e[0][0] + b.e[0][0];
  result.e[0][1] = a.e[0][1] + b.e[0][1];
  result.e[0][2] = a.e[0][2] + b.e[0][2];
  result.e[0][3] = a.e[0][3] + b.e[0][3];
  result.e[1][0] = a.e[1][0] + b.e[1][0];
  result.e[1][1] = a.e[1][1] + b.e[1][1];
  result.e[1][2] = a.e[1][2] + b.e[1][2];
  result.e[1][3] = a.e[1][3] + b.e[1][3];
  result.e[2][0] = a.e[2][0] + b.e[2][0];
  result.e[2][1] = a.e[2][1] + b.e[2][1];
  result.e[2][2] = a.e[2][2] + b.e[2][2];
  result.e[2][3] = a.e[2][3] + b.e[2][3];
  result.e[3][0] = a.e[3][0] + b.e[3][0];
  result.e[3][1] = a.e[3][1] + b.e[3][1];
  result.e[3][2] = a.e[3][2] + b.e[3][2];
  result.e[3][3] = a.e[3][3] + b.e[3][3];
  return result;
}

M4 M4SubM4(M4 a, M4 b) {
  M4 result;
  result.e[0][0] = a.e[0][0] - b.e[0][0];
  result.e[0][1] = a.e[0][1] - b.e[0][1];
  result.e[0][2] = a.e[0][2] - b.e[0][2];
  result.e[0][3] = a.e[0][3] - b.e[0][3];
  result.e[1][0] = a.e[1][0] - b.e[1][0];
  result.e[1][1] = a.e[1][1] - b.e[1][1];
  result.e[1][2] = a.e[1][2] - b.e[1][2];
  result.e[1][3] = a.e[1][3] - b.e[1][3];
  result.e[2][0] = a.e[2][0] - b.e[2][0];
  result.e[2][1] = a.e[2][1] - b.e[2][1];
  result.e[2][2] = a.e[2][2] - b.e[2][2];
  result.e[2][3] = a.e[2][3] - b.e[2][3];
  result.e[3][0] = a.e[3][0] - b.e[3][0];
  result.e[3][1] = a.e[3][1] - b.e[3][1];
  result.e[3][2] = a.e[3][2] - b.e[3][2];
  result.e[3][3] = a.e[3][3] - b.e[3][3];
  return result;
}

M4 M4MultF32(M4 m, F32 c) {
  M4 result;
  result.e[0][0] = m.e[0][0] * c;
  result.e[0][1] = m.e[0][1] * c;
  result.e[0][2] = m.e[0][2] * c;
  result.e[0][3] = m.e[0][3] * c;
  result.e[1][0] = m.e[1][0] * c;
  result.e[1][1] = m.e[1][1] * c;
  result.e[1][2] = m.e[1][2] * c;
  result.e[1][3] = m.e[1][3] * c;
  result.e[2][0] = m.e[2][0] * c;
  result.e[2][1] = m.e[2][1] * c;
  result.e[2][2] = m.e[2][2] * c;
  result.e[2][3] = m.e[2][3] * c;
  result.e[3][0] = m.e[3][0] * c;
  result.e[3][1] = m.e[3][1] * c;
  result.e[3][2] = m.e[3][2] * c;
  result.e[3][3] = m.e[3][3] * c;
  return result;
}

M4 M4MultM4(M4 a, M4 b) {
  M4 result;
  result.e[0][0] = (a.e[0][0] * b.e[0][0]) + (a.e[0][1] * b.e[1][0]) + (a.e[0][2] * b.e[2][0]) + (a.e[0][3] * b.e[3][0]);
  result.e[0][1] = (a.e[0][0] * b.e[0][1]) + (a.e[0][1] * b.e[1][1]) + (a.e[0][2] * b.e[2][1]) + (a.e[0][3] * b.e[3][1]);
  result.e[0][2] = (a.e[0][0] * b.e[0][2]) + (a.e[0][1] * b.e[1][2]) + (a.e[0][2] * b.e[2][2]) + (a.e[0][3] * b.e[3][2]);
  result.e[0][3] = (a.e[0][0] * b.e[0][3]) + (a.e[0][1] * b.e[1][3]) + (a.e[0][2] * b.e[2][3]) + (a.e[0][3] * b.e[3][3]);
  result.e[1][0] = (a.e[1][0] * b.e[0][0]) + (a.e[1][1] * b.e[1][0]) + (a.e[1][2] * b.e[2][0]) + (a.e[1][3] * b.e[3][0]);
  result.e[1][1] = (a.e[1][0] * b.e[0][1]) + (a.e[1][1] * b.e[1][1]) + (a.e[1][2] * b.e[2][1]) + (a.e[1][3] * b.e[3][1]);
  result.e[1][2] = (a.e[1][0] * b.e[0][2]) + (a.e[1][1] * b.e[1][2]) + (a.e[1][2] * b.e[2][2]) + (a.e[1][3] * b.e[3][2]);
  result.e[1][3] = (a.e[1][0] * b.e[0][3]) + (a.e[1][1] * b.e[1][3]) + (a.e[1][2] * b.e[2][3]) + (a.e[1][3] * b.e[3][3]);
  result.e[2][0] = (a.e[2][0] * b.e[0][0]) + (a.e[2][1] * b.e[1][0]) + (a.e[2][2] * b.e[2][0]) + (a.e[2][3] * b.e[3][0]);
  result.e[2][1] = (a.e[2][0] * b.e[0][1]) + (a.e[2][1] * b.e[1][1]) + (a.e[2][2] * b.e[2][1]) + (a.e[2][3] * b.e[3][1]);
  result.e[2][2] = (a.e[2][0] * b.e[0][2]) + (a.e[2][1] * b.e[1][2]) + (a.e[2][2] * b.e[2][2]) + (a.e[2][3] * b.e[3][2]);
  result.e[2][3] = (a.e[2][0] * b.e[0][3]) + (a.e[2][1] * b.e[1][3]) + (a.e[2][2] * b.e[2][3]) + (a.e[2][3] * b.e[3][3]);
  result.e[3][0] = (a.e[3][0] * b.e[0][0]) + (a.e[3][1] * b.e[1][0]) + (a.e[3][2] * b.e[2][0]) + (a.e[3][3] * b.e[3][0]);
  result.e[3][1] = (a.e[3][0] * b.e[0][1]) + (a.e[3][1] * b.e[1][1]) + (a.e[3][2] * b.e[2][1]) + (a.e[3][3] * b.e[3][1]);
  result.e[3][2] = (a.e[3][0] * b.e[0][2]) + (a.e[3][1] * b.e[1][2]) + (a.e[3][2] * b.e[2][2]) + (a.e[3][3] * b.e[3][2]);
  result.e[3][3] = (a.e[3][0] * b.e[0][3]) + (a.e[3][1] * b.e[1][3]) + (a.e[3][2] * b.e[2][3]) + (a.e[3][3] * b.e[3][3]);
  return result;
}

V4 M4MultV4(M4 m, V4 v) {
  V4 result;
  result.e[0] = (m.e[0][0] * v.e[0]) + (m.e[0][1] * v.e[1]) + (m.e[0][2] * v.e[2]) + (m.e[0][3] * v.e[3]);
  result.e[1] = (m.e[1][0] * v.e[0]) + (m.e[1][1] * v.e[1]) + (m.e[1][2] * v.e[2]) + (m.e[1][3] * v.e[3]);
  result.e[2] = (m.e[2][0] * v.e[0]) + (m.e[2][1] * v.e[1]) + (m.e[2][2] * v.e[2]) + (m.e[2][3] * v.e[3]);
  result.e[3] = (m.e[3][0] * v.e[0]) + (m.e[3][1] * v.e[1]) + (m.e[3][2] * v.e[2]) + (m.e[3][3] * v.e[3]);
  return result;
}

M4 M4Transpose(M4 m) {
  M4 result;
  result.e[0][0] = m.e[0][0];
  result.e[0][1] = m.e[1][0];
  result.e[0][2] = m.e[2][0];
  result.e[0][3] = m.e[3][0];
  result.e[1][0] = m.e[0][1];
  result.e[1][1] = m.e[1][1];
  result.e[1][2] = m.e[2][1];
  result.e[1][3] = m.e[3][1];
  result.e[2][0] = m.e[0][2];
  result.e[2][1] = m.e[1][2];
  result.e[2][2] = m.e[2][2];
  result.e[2][3] = m.e[3][2];
  result.e[3][0] = m.e[0][3];
  result.e[3][1] = m.e[1][3];
  result.e[3][2] = m.e[2][3];
  result.e[3][3] = m.e[3][3];
  return result;
}

// https://byjus.com/maths/determinant-of-4x4-matrix/
F32 M4Det(M4 m) {
  F32 sub_00 = (m.e[2][2] * m.e[3][3]) - (m.e[2][3] * m.e[3][2]);
  F32 sub_01 = (m.e[2][1] * m.e[3][3]) - (m.e[2][3] * m.e[3][1]);
  F32 sub_02 = (m.e[2][1] * m.e[3][2]) - (m.e[2][2] * m.e[3][1]);
  F32 det_0  = (m.e[1][1] * sub_00) - (m.e[1][2] * sub_01) + (m.e[1][3] * sub_02);
  F32 sub_10 = (m.e[2][2] * m.e[3][3]) - (m.e[2][3] * m.e[3][2]);
  F32 sub_11 = (m.e[2][0] * m.e[3][3]) - (m.e[2][3] * m.e[3][0]);
  F32 sub_12 = (m.e[2][0] * m.e[3][2]) - (m.e[2][2] * m.e[3][0]);
  F32 det_1  = (m.e[1][0] * sub_10) - (m.e[1][2] * sub_11) + (m.e[1][3] * sub_12);
  F32 sub_20 = (m.e[2][1] * m.e[3][3]) - (m.e[2][3] * m.e[3][1]);
  F32 sub_21 = (m.e[2][0] * m.e[3][3]) - (m.e[2][3] * m.e[3][0]);
  F32 sub_22 = (m.e[2][0] * m.e[3][1]) - (m.e[2][1] * m.e[3][0]);
  F32 det_2  = (m.e[1][0] * sub_20) - (m.e[1][1] * sub_21) + (m.e[1][3] * sub_22);
  F32 sub_30 = (m.e[2][1] * m.e[3][2]) - (m.e[2][2] * m.e[3][1]);
  F32 sub_31 = (m.e[2][0] * m.e[3][2]) - (m.e[2][2] * m.e[3][0]);
  F32 sub_32 = (m.e[2][0] * m.e[3][1]) - (m.e[2][1] * m.e[3][0]);
  F32 det_3  = (m.e[1][0] * sub_30) - (m.e[1][1] * sub_31) + (m.e[1][2] * sub_32);
  return ((m.e[0][0] * det_0) - (m.e[0][1] * det_1) + (m.e[0][2] * det_2) - (m.e[0][3] * det_3));
}

// http://rodolphe-vaillant.fr/entry/7/c-code-for-4x4-matrix-inversion
M4 M4Invert(M4 m) {
  M4 t;
  t.i[ 0] =  m.i[5] * m.i[10] * m.i[15] - m.i[5] * m.i[14] * m.i[11] - m.i[6] * m.i[9] * m.i[15] + m.i[6] * m.i[13] * m.i[11] + m.i[7] * m.i[9] * m.i[14] - m.i[7] * m.i[13] * m.i[10];
  t.i[ 1] = -m.i[1] * m.i[10] * m.i[15] + m.i[1] * m.i[14] * m.i[11] + m.i[2] * m.i[9] * m.i[15] - m.i[2] * m.i[13] * m.i[11] - m.i[3] * m.i[9] * m.i[14] + m.i[3] * m.i[13] * m.i[10];
  t.i[ 2] =  m.i[1] * m.i[ 6] * m.i[15] - m.i[1] * m.i[14] * m.i[ 7] - m.i[2] * m.i[5] * m.i[15] + m.i[2] * m.i[13] * m.i[ 7] + m.i[3] * m.i[5] * m.i[14] - m.i[3] * m.i[13] * m.i[ 6];
  t.i[ 3] = -m.i[1] * m.i[ 6] * m.i[11] + m.i[1] * m.i[10] * m.i[ 7] + m.i[2] * m.i[5] * m.i[11] - m.i[2] * m.i[ 9] * m.i[ 7] - m.i[3] * m.i[5] * m.i[10] + m.i[3] * m.i[ 9] * m.i[ 6];
  t.i[ 4] = -m.i[4] * m.i[10] * m.i[15] + m.i[4] * m.i[14] * m.i[11] + m.i[6] * m.i[8] * m.i[15] - m.i[6] * m.i[12] * m.i[11] - m.i[7] * m.i[8] * m.i[14] + m.i[7] * m.i[12] * m.i[10];
  t.i[ 5] =  m.i[0] * m.i[10] * m.i[15] - m.i[0] * m.i[14] * m.i[11] - m.i[2] * m.i[8] * m.i[15] + m.i[2] * m.i[12] * m.i[11] + m.i[3] * m.i[8] * m.i[14] - m.i[3] * m.i[12] * m.i[10];
  t.i[ 6] = -m.i[0] * m.i[ 6] * m.i[15] + m.i[0] * m.i[14] * m.i[ 7] + m.i[2] * m.i[4] * m.i[15] - m.i[2] * m.i[12] * m.i[ 7] - m.i[3] * m.i[4] * m.i[14] + m.i[3] * m.i[12] * m.i[ 6];
  t.i[ 7] =  m.i[0] * m.i[ 6] * m.i[11] - m.i[0] * m.i[10] * m.i[ 7] - m.i[2] * m.i[4] * m.i[11] + m.i[2] * m.i[ 8] * m.i[ 7] + m.i[3] * m.i[4] * m.i[10] - m.i[3] * m.i[ 8] * m.i[ 6];
  t.i[ 8] =  m.i[4] * m.i[ 9] * m.i[15] - m.i[4] * m.i[13] * m.i[11] - m.i[5] * m.i[8] * m.i[15] + m.i[5] * m.i[12] * m.i[11] + m.i[7] * m.i[8] * m.i[13] - m.i[7] * m.i[12] * m.i[ 9];
  t.i[ 9] = -m.i[0] * m.i[ 9] * m.i[15] + m.i[0] * m.i[13] * m.i[11] + m.i[1] * m.i[8] * m.i[15] - m.i[1] * m.i[12] * m.i[11] - m.i[3] * m.i[8] * m.i[13] + m.i[3] * m.i[12] * m.i[ 9];
  t.i[10] =  m.i[0] * m.i[ 5] * m.i[15] - m.i[0] * m.i[13] * m.i[ 7] - m.i[1] * m.i[4] * m.i[15] + m.i[1] * m.i[12] * m.i[ 7] + m.i[3] * m.i[4] * m.i[13] - m.i[3] * m.i[12] * m.i[ 5];
  t.i[11] = -m.i[0] * m.i[ 5] * m.i[11] + m.i[0] * m.i[ 9] * m.i[ 7] + m.i[1] * m.i[4] * m.i[11] - m.i[1] * m.i[ 8] * m.i[ 7] - m.i[3] * m.i[4] * m.i[ 9] + m.i[3] * m.i[ 8] * m.i[ 5];
  t.i[12] = -m.i[4] * m.i[ 9] * m.i[14] + m.i[4] * m.i[13] * m.i[10] + m.i[5] * m.i[8] * m.i[14] - m.i[5] * m.i[12] * m.i[10] - m.i[6] * m.i[8] * m.i[13] + m.i[6] * m.i[12] * m.i[ 9];
  t.i[13] =  m.i[0] * m.i[ 9] * m.i[14] - m.i[0] * m.i[13] * m.i[10] - m.i[1] * m.i[8] * m.i[14] + m.i[1] * m.i[12] * m.i[10] + m.i[2] * m.i[8] * m.i[13] - m.i[2] * m.i[12] * m.i[ 9];
  t.i[14] = -m.i[0] * m.i[ 5] * m.i[14] + m.i[0] * m.i[13] * m.i[ 6] + m.i[1] * m.i[4] * m.i[14] - m.i[1] * m.i[12] * m.i[ 6] - m.i[2] * m.i[4] * m.i[13] + m.i[2] * m.i[12] * m.i[ 5];
  t.i[15] =  m.i[0] * m.i[ 5] * m.i[10] - m.i[0] * m.i[ 9] * m.i[ 6] - m.i[1] * m.i[4] * m.i[10] + m.i[1] * m.i[ 8] * m.i[ 6] + m.i[2] * m.i[4] * m.i[ 9] - m.i[2] * m.i[ 8] * m.i[ 5];
  F32 det = m.i[0] * t.i[0] + m.i[4] * t.i[1] + m.i[8] * t.i[2] + m.i[12] * t.i[3];

  DEBUG_ASSERT(det != 0);
  F32 det_inv = 1 / det;

  M4 result;
  for(U32 j = 0; j < 16; j++) { result.i[j] = det_inv * t.i[j]; }
  return result;
}

B32 M4ApproxEq(M4 a, M4 b) {
  return F32ApproxEq(a.e[0][0], b.e[0][0]) &&
         F32ApproxEq(a.e[0][1], b.e[0][1]) &&
         F32ApproxEq(a.e[0][2], b.e[0][2]) &&
         F32ApproxEq(a.e[0][3], b.e[0][3]) &&
         F32ApproxEq(a.e[1][0], b.e[1][0]) &&
         F32ApproxEq(a.e[1][1], b.e[1][1]) &&
         F32ApproxEq(a.e[1][2], b.e[1][2]) &&
         F32ApproxEq(a.e[1][3], b.e[1][3]) &&
         F32ApproxEq(a.e[2][0], b.e[2][0]) &&
         F32ApproxEq(a.e[2][1], b.e[2][1]) &&
         F32ApproxEq(a.e[2][2], b.e[2][2]) &&
         F32ApproxEq(a.e[2][3], b.e[2][3]) &&
         F32ApproxEq(a.e[3][0], b.e[3][0]) &&
         F32ApproxEq(a.e[3][1], b.e[3][1]) &&
         F32ApproxEq(a.e[3][2], b.e[3][2]) &&
         F32ApproxEq(a.e[3][3], b.e[3][3]);
}

B32 M4Eq(M4 a, M4 b) {
  return a.e[0][0] == b.e[0][0] &&
         a.e[0][1] == b.e[0][1] &&
         a.e[0][2] == b.e[0][2] &&
         a.e[0][2] == b.e[0][3] &&
         a.e[1][0] == b.e[1][0] &&
         a.e[1][1] == b.e[1][1] &&
         a.e[1][2] == b.e[1][2] &&
         a.e[1][3] == b.e[1][3] &&
         a.e[2][0] == b.e[2][0] &&
         a.e[2][1] == b.e[2][1] &&
         a.e[2][2] == b.e[2][2] &&
         a.e[2][3] == b.e[2][3] &&
         a.e[3][0] == b.e[3][0] &&
         a.e[3][1] == b.e[3][1] &&
         a.e[3][2] == b.e[3][2] &&
         a.e[3][3] == b.e[3][3];
}

#endif // CDEFAULT_MATH_IMPLEMENTATION
