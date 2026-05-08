#ifndef ZFIX_PROFILES_H
#define ZFIX_PROFILES_H

#include <windows.h>

typedef enum ZfixDepthProfileMode {
  ZFIX_DEPTH_PROFILE_DEFAULT = 0,
  ZFIX_DEPTH_PROFILE_OFF = 1,
  ZFIX_DEPTH_PROFILE_NORMAL = 2,
  ZFIX_DEPTH_PROFILE_AGGRESSIVE = 3
} ZfixDepthProfileMode;

typedef struct ZfixCallsiteProfile {
  const char* name;
  DWORD caller_min;
  DWORD caller_max;
  ZfixDepthProfileMode depth_mode;
  DWORD color_z_func;
  float conflict_min_span;
  float conflict_max_span;
  float conflict_max_shift;
  float min_area;
  float max_area;
  float min_extent;
  float max_extent;
  float match_min_z_span;
  float match_max_z_span;
  float match_min_area;
  float match_max_area;
  float match_min_extent;
  float match_max_extent;
  DWORD texture_handle;
  DWORD texture_width;
  DWORD texture_height;
  float over_shift_guard_base;
  float over_shift_guard_z_scale;
  float over_shift_guard_max;
} ZfixCallsiteProfile;

typedef struct ZfixProfileMatchInfo {
  DWORD caller;
  float z_span;
  float area;
  float extent;
  DWORD texture_handle;
  DWORD texture_width;
  DWORD texture_height;
} ZfixProfileMatchInfo;

static inline int ZfixCallsiteProfileMatches(const ZfixCallsiteProfile* profile, DWORD caller)
{
  if (!profile || !caller)
    return 0;
  if (profile->caller_min && caller < profile->caller_min)
    return 0;
  if (profile->caller_max && caller > profile->caller_max)
    return 0;
  return 1;
}

static inline int ZfixCallsiteProfileHasDrawConstraints(const ZfixCallsiteProfile* profile)
{
  if (!profile)
    return 0;
  return profile->match_min_z_span > 0.0f ||
         profile->match_max_z_span > 0.0f ||
         profile->match_min_area > 0.0f ||
         profile->match_max_area > 0.0f ||
         profile->match_min_extent > 0.0f ||
         profile->match_max_extent > 0.0f ||
         profile->texture_handle != 0u ||
         profile->texture_width != 0u ||
         profile->texture_height != 0u;
}

static inline int ZfixCallsiteProfileMatchesDraw(const ZfixCallsiteProfile* profile,
                                                 const ZfixProfileMatchInfo* info)
{
  if (!profile || !info)
    return 0;
  if (!ZfixCallsiteProfileMatches(profile, info->caller))
    return 0;
  if (profile->match_min_z_span > 0.0f && info->z_span < profile->match_min_z_span)
    return 0;
  if (profile->match_max_z_span > 0.0f && info->z_span > profile->match_max_z_span)
    return 0;
  if (profile->match_min_area > 0.0f && info->area < profile->match_min_area)
    return 0;
  if (profile->match_max_area > 0.0f && info->area > profile->match_max_area)
    return 0;
  if (profile->match_min_extent > 0.0f && info->extent < profile->match_min_extent)
    return 0;
  if (profile->match_max_extent > 0.0f && info->extent > profile->match_max_extent)
    return 0;
  if (profile->texture_handle && profile->texture_handle != info->texture_handle)
    return 0;
  if (profile->texture_width && profile->texture_width != info->texture_width)
    return 0;
  if (profile->texture_height && profile->texture_height != info->texture_height)
    return 0;
  return 1;
}

static inline const ZfixCallsiteProfile* ZfixFindCallsiteProfile(
  const ZfixCallsiteProfile* profiles, DWORD profile_count, DWORD caller)
{
  if (!profiles || !caller)
    return NULL;
  for (DWORD i = 0; i < profile_count; i++)
  {
    if (ZfixCallsiteProfileHasDrawConstraints(&profiles[i]))
      continue;
    if (ZfixCallsiteProfileMatches(&profiles[i], caller))
      return &profiles[i];
  }
  return NULL;
}

static inline const ZfixCallsiteProfile* ZfixFindCallsiteProfileForDraw(
  const ZfixCallsiteProfile* profiles, DWORD profile_count, const ZfixProfileMatchInfo* info)
{
  if (!profiles || !info || !info->caller)
    return NULL;

  for (DWORD i = 0; i < profile_count; i++)
  {
    if (ZfixCallsiteProfileHasDrawConstraints(&profiles[i]) &&
        ZfixCallsiteProfileMatchesDraw(&profiles[i], info))
      return &profiles[i];
  }
  for (DWORD i = 0; i < profile_count; i++)
  {
    if (!ZfixCallsiteProfileHasDrawConstraints(&profiles[i]) &&
        ZfixCallsiteProfileMatchesDraw(&profiles[i], info))
      return &profiles[i];
  }
  return NULL;
}

static inline int ZfixProfileDepthEnabled(const ZfixCallsiteProfile* profile)
{
  return !profile || profile->depth_mode != ZFIX_DEPTH_PROFILE_OFF;
}

static inline int ZfixProfileIsAggressive(const ZfixCallsiteProfile* profile)
{
  return profile && profile->depth_mode == ZFIX_DEPTH_PROFILE_AGGRESSIVE;
}

static inline DWORD ZfixProfileColorZFunc(const ZfixCallsiteProfile* profile, DWORD fallback)
{
  return (profile && profile->color_z_func) ? profile->color_z_func : fallback;
}

static inline float ZfixFloatOrDefault(float value, float fallback)
{
  return value > 0.0f ? value : fallback;
}

static inline float ZfixProfileGuardedMaxShift(const ZfixCallsiteProfile* profile,
                                               float z_span, float fallback)
{
  float limit = fallback;
  if (!profile)
    return limit;
  if (profile->over_shift_guard_max > 0.0f &&
      (limit <= 0.0f || profile->over_shift_guard_max < limit))
    limit = profile->over_shift_guard_max;
  if (profile->over_shift_guard_base > 0.0f ||
      profile->over_shift_guard_z_scale > 0.0f)
  {
    const float scaled =
      profile->over_shift_guard_base + (z_span * profile->over_shift_guard_z_scale);
    if (scaled > 0.0f && (limit <= 0.0f || scaled < limit))
      limit = scaled;
  }
  return limit;
}

#endif
