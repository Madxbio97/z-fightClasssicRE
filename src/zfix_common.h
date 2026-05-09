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

typedef enum ZfixTexturePageClass {
  ZFIX_TEXTURE_PAGE_NONE = 0,
  ZFIX_TEXTURE_PAGE_CLASSIC,
  ZFIX_TEXTURE_PAGE_HIRES
} ZfixTexturePageClass;

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

static inline int NearF(float a, float b, float eps)
{
  return AbsF(a - b) <= eps;
}

static inline ZfixTexturePageClass ZfixClassifyMaskTexturePage(
  DWORD width, DWORD height, DWORD hires_min_side,
  DWORD classic_min_side, DWORD classic_max_side)
{
  if (!width || !height)
    return ZFIX_TEXTURE_PAGE_NONE;

  const DWORD min_side = width < height ? width : height;
  const DWORD max_side = width > height ? width : height;
  if (hires_min_side && min_side >= hires_min_side)
    return ZFIX_TEXTURE_PAGE_HIRES;
  if (classic_min_side && min_side >= classic_min_side &&
      (!classic_max_side || max_side <= classic_max_side))
    return ZFIX_TEXTURE_PAGE_CLASSIC;
  return ZFIX_TEXTURE_PAGE_NONE;
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
