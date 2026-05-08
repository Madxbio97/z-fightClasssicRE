#ifndef ZFIX_COMMON_H
#define ZFIX_COMMON_H

#include <windows.h>
#include <stdint.h>

#ifndef ARRAYSIZE
#define ARRAYSIZE(a) (sizeof(a) / sizeof((a)[0]))
#endif

#ifndef ZFIX_SKIP_STATE
#define ZFIX_SKIP_STATE (-1)
#endif

typedef struct DrawBounds {
  float min_x;
  float max_x;
  float min_y;
  float max_y;
  float min_z;
  float max_z;
  float min_rhw;
  float max_rhw;
  float width;
  float height;
  float area;
} DrawBounds;

typedef enum AlphaModelClass {
  ALPHA_MODEL_TRANSLUCENT = 0,
  ALPHA_MODEL_CUTOUT = 1
} AlphaModelClass;

static inline float AbsF(float v)
{
  return v < 0.0f ? -v : v;
}

static inline float ClampDepth(float z)
{
  if (z < 0.0f)
    return 0.0f;
  if (z > 1.0f)
    return 1.0f;
  return z;
}

static inline float Clamp01(float v)
{
  if (v < 0.0f)
    return 0.0f;
  if (v > 1.0f)
    return 1.0f;
  return v;
}

static inline float LerpF(float a, float b, float t)
{
  return a + ((b - a) * t);
}

static inline int NearF(float a, float b, float eps)
{
  return AbsF(a - b) <= eps;
}

static inline int ClampByteFromFloat(float v)
{
  if (v < 0.0f)
    return 0;
  if (v > 255.0f)
    return 255;
  return (int)(v + 0.5f);
}

static inline DWORD TriangleCount(DWORD primitive_type, DWORD vertex_or_index_count)
{
  switch (primitive_type)
  {
    case D3DPT_TRIANGLELIST:
      return vertex_or_index_count / 3;
    case D3DPT_TRIANGLESTRIP:
    case D3DPT_TRIANGLEFAN:
      return (vertex_or_index_count >= 3) ? (vertex_or_index_count - 2) : 0;
    default:
      return 0;
  }
}

#endif
