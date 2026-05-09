#include <windows.h>
#include <tlhelp32.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <intrin.h>

#if defined(_MSC_VER)
#pragma intrinsic(_ReturnAddress)
#endif

#define D3D_OK 0
#define D3DPT_TRIANGLELIST 4
#define D3DPT_TRIANGLESTRIP 5
#define D3DPT_TRIANGLEFAN 6
#define D3DCMP_LESS 2
#define D3DCMP_EQUAL 3
#define D3DCMP_LESSEQUAL 4
#define D3DCMP_GREATER 5
#define D3DRS_ZENABLE 7
#define D3DRS_SHADEMODE 9
#define D3DRS_ZWRITEENABLE 14
#define D3DRS_ALPHATESTENABLE 15
#define D3DRS_SRCBLEND 19
#define D3DRS_DESTBLEND 20
#define D3DRS_CULLMODE 22
#define D3DRS_ZFUNC 23
#define D3DRS_ALPHAREF 24
#define D3DRS_ALPHAFUNC 25
#define D3DRS_DITHERENABLE 26
#define D3DRS_ALPHABLENDENABLE 27
#define D3DRS_COLORWRITEENABLE 168
#define D3D_COLORWRITE_ALL 0x0000000Fu
#define D3DBLEND_SRCALPHA 5
#define D3DBLEND_INVSRCALPHA 6
#define D3DCULL_NONE 1
#define D3DCLEAR_ZBUFFER 0x00000002u
#define D3DSHADE_GOURAUD 2
#define D3DBACKBUFFER_TYPE_MONO 0
#define D3DMULTISAMPLE_NONE 0
#define D3DFMT_INDEX16 101
#define D3DFMT_INDEX32 102
#define D3DFMT_D32 71
#define D3DFMT_D24S8 75
#define D3DFMT_D24X8 77
#define D3DFMT_D16 80
#define D3DFMT_D32F_LOCKABLE 82
#define D3DRTYPE_TEXTURE 3

#include "zfix_common.h"
#include "zfix_hooks.h"
#include "zfix_log.h"
#include "zfix_profiles.h"

typedef void*(WINAPI* Direct3DCreate9Proc)(UINT sdk_version);
typedef HRESULT(STDMETHODCALLTYPE* D3D9CreateDeviceProc)(void* self, UINT adapter, DWORD device_type, HWND focus,
                                                        DWORD behavior_flags, void* presentation_parameters,
                                                        void** returned_device);
typedef HRESULT(STDMETHODCALLTYPE* D3D9ResetProc)(void* self, void* presentation_parameters);
typedef HRESULT(STDMETHODCALLTYPE* D3D9BeginSceneProc)(void* self);
typedef HRESULT(STDMETHODCALLTYPE* D3D9EndSceneProc)(void* self);
typedef ULONG(STDMETHODCALLTYPE* ComReleaseProc)(void* self);
typedef HRESULT(STDMETHODCALLTYPE* D3D9GetBackBufferProc)(void* self, UINT swap_chain, UINT back_buffer,
                                                         DWORD type, void** surface);
typedef HRESULT(STDMETHODCALLTYPE* D3D9CreateDepthStencilSurfaceProc)(void* self, UINT width, UINT height,
                                                                     DWORD format, DWORD multisample_type,
                                                                     DWORD multisample_quality, BOOL discard,
                                                                     void** surface, HANDLE* shared_handle);
typedef HRESULT(STDMETHODCALLTYPE* D3D9SetDepthStencilSurfaceProc)(void* self, void* surface);
typedef HRESULT(STDMETHODCALLTYPE* D3D9GetDepthStencilSurfaceProc)(void* self, void** surface);
typedef HRESULT(STDMETHODCALLTYPE* D3D9SurfaceGetDescProc)(void* self, void* desc);
typedef HRESULT(STDMETHODCALLTYPE* D3D9GetRenderStateProc)(void* self, DWORD state, DWORD* value);
typedef HRESULT(STDMETHODCALLTYPE* D3D9SetRenderStateProc)(void* self, DWORD state, DWORD value);
typedef HRESULT(STDMETHODCALLTYPE* D3D9SetTextureProc)(void* self, DWORD stage, void* texture);
typedef DWORD(STDMETHODCALLTYPE* D3D9ResourceGetTypeProc)(void* self);
typedef HRESULT(STDMETHODCALLTYPE* D3D9TextureGetLevelDescProc)(void* self, UINT level, void* desc);
typedef HRESULT(STDMETHODCALLTYPE* D3D9SetFVFProc)(void* self, DWORD fvf);
typedef HRESULT(STDMETHODCALLTYPE* D3D9ClearProc)(void* self, DWORD count, const void* rects,
                                                 DWORD flags, DWORD color, float z, DWORD stencil);
typedef HRESULT(STDMETHODCALLTYPE* D3D9DrawPrimitiveProc)(void* self, DWORD primitive_type,
                                                         UINT start_vertex, UINT primitive_count);
typedef HRESULT(STDMETHODCALLTYPE* D3D9DrawIndexedPrimitiveProc)(void* self, DWORD primitive_type,
                                                                INT base_vertex_index, UINT min_vertex_index,
                                                                UINT num_vertices, UINT start_index,
                                                                UINT primitive_count);
typedef HRESULT(STDMETHODCALLTYPE* D3D9DrawPrimitiveUPProc)(void* self, DWORD primitive_type, UINT primitive_count,
                                                           const void* vertex_data, UINT vertex_stride);
typedef HRESULT(STDMETHODCALLTYPE* D3D9DrawIndexedPrimitiveUPProc)(void* self, DWORD primitive_type,
                                                                  UINT min_vertex_index, UINT num_vertices,
                                                                  UINT primitive_count, const void* index_data,
                                                                  DWORD index_format, const void* vertex_data,
                                                                  UINT vertex_stride);

typedef struct D3DPRESENT_PARAMETERS_COMPAT {
  UINT BackBufferWidth;
  UINT BackBufferHeight;
  DWORD BackBufferFormat;
  UINT BackBufferCount;
  DWORD MultiSampleType;
  DWORD MultiSampleQuality;
  DWORD SwapEffect;
  HWND hDeviceWindow;
  BOOL Windowed;
  BOOL EnableAutoDepthStencil;
  DWORD AutoDepthStencilFormat;
  DWORD Flags;
  UINT FullScreen_RefreshRateInHz;
  UINT PresentationInterval;
} D3DPRESENT_PARAMETERS_COMPAT;

typedef struct D3DSURFACE_DESC_COMPAT {
  DWORD Format;
  DWORD Type;
  DWORD Usage;
  DWORD Pool;
  DWORD MultiSampleType;
  DWORD MultiSampleQuality;
  UINT Width;
  UINT Height;
} D3DSURFACE_DESC_COMPAT;

typedef struct D3D9TLVERTEX {
  float sx;
  float sy;
  float sz;
  float rhw;
  uint32_t color;
  float tu;
  float tv;
} D3D9TLVERTEX;

typedef struct D3D9StateSnapshot {
  DWORD z_enable;
  DWORD z_write;
  DWORD z_func;
  DWORD alpha_test;
  DWORD alpha_ref;
  DWORD alpha_func;
  DWORD alpha_blend;
  DWORD src_blend;
  DWORD dest_blend;
  DWORD cull_mode;
  DWORD color_write;
  DWORD shade_mode;
  DWORD dither_enable;
  int has_z_enable;
  int has_z_write;
  int has_z_func;
  int has_alpha_test;
  int has_alpha_ref;
  int has_alpha_func;
  int has_alpha_blend;
  int has_src_blend;
  int has_dest_blend;
  int has_cull_mode;
  int has_color_write;
  int has_shade_mode;
  int has_dither_enable;
} D3D9StateSnapshot;

static HookEntry g_hooks[128];
static volatile LONG g_hook_count = 0;
static char g_log_path[MAX_PATH];
static int g_log_enabled = 0;
static Direct3DCreate9Proc g_real_direct3d_create9 = NULL;
static int g_scene_open = 0;
static DWORD g_current_fvf = 0;
static void* g_current_texture0 = NULL;
static UINT g_current_texture0_width = 0;
static UINT g_current_texture0_height = 0;
static void* g_owned_depth_device = NULL;
static void* g_owned_depth_surface = NULL;
static UINT g_owned_depth_width = 0;
static UINT g_owned_depth_height = 0;
static DWORD g_owned_depth_format = 0;
static volatile LONG g_dpup_total = 0;
static volatile LONG g_dpup_accepted = 0;
static volatile LONG g_dpup_stride_rejected = 0;
static volatile LONG g_dpup_alpha_rejected = 0;
static volatile LONG g_dpup_axis_rejected = 0;
static volatile LONG g_dpup_rhw_rejected = 0;
static volatile LONG g_dpup_other_rejected = 0;
static volatile LONG g_dp_calls = 0;
static volatile LONG g_dip_calls = 0;
static volatile LONG g_dipup_total = 0;
static volatile LONG g_dipup_accepted = 0;
static volatile LONG g_dipup_rejected = 0;
static volatile LONG g_dpup_transparent_accepted = 0;
static volatile LONG g_dpup_cutout_accepted = 0;
static volatile LONG g_dipup_cutout_accepted = 0;
static volatile LONG g_frame_counter = 0;
static volatile LONG g_model_depth_prepass_draws = 0;
static volatile LONG g_model_depth_prepass_failures = 0;
static volatile LONG g_callsite_profile_hits = 0;
static volatile LONG g_adaptive_depth_draws = 0;
static volatile LONG g_adaptive_depth_vertices = 0;
static volatile LONG g_adaptive_depth_logged = 0;
static volatile LONG g_depth_clear_count = 0;
static volatile LONG g_depth_clear_failures = 0;
static volatile LONG g_owned_depth_creates = 0;
static volatile LONG g_owned_depth_sets = 0;
static volatile LONG g_owned_depth_failures = 0;
static volatile LONG g_set_texture_calls = 0;
static volatile LONG g_set_texture0_changes = 0;
static volatile LONG g_set_fvf_calls = 0;
static volatile LONG g_set_render_state_calls = 0;

static const int g_enabled = 1;
static const int g_upgrade_depth_stencil = 1;
static const int g_prefer_high_precision_depth = 1;
static const int g_force_owned_depth_surface = 1;
static const int g_force_z_enable = 1;
static const int g_force_z_write = 1;
static const int g_force_z_func = D3DCMP_LESSEQUAL;
static const int g_restore_state = 1;
static const int g_clear_depth_each_scene = 1;
static const int g_model_depth_prepass = 1;
static const int g_model_color_pass_z_write = 0;
static const int g_model_color_pass_z_func = D3DCMP_LESSEQUAL;
static const int g_model_alpha_test = 1;
static const DWORD g_model_alpha_ref = 8u;
static const DWORD g_model_alpha_func = D3DCMP_GREATER;
static const int g_model_gouraud_shading = 0;
static const int g_model_dither = 0;
static const int g_skip_axis_tile_draws = 1;
static const int g_min_vertex_alpha = 250;
static const int g_reject_alpha_only_when_blending = 1;
static const int g_diagnostics = 1;
static const int g_transparent_model_z_test = 1;
static const int g_cutout_model_alpha_test = 1;
static const int g_cutout_model_disable_alpha_blend = 1;
static const int g_cutout_model_two_sided = 1;
static const int g_cutout_alpha_min = 224;
static const int g_cutout_hard_alpha_low_max = 16;
static const int g_cutout_hard_alpha_high_min = 240;
static const DWORD g_cutout_model_alpha_ref = 16u;
static const int g_callsite_profiles_enabled = 1;
static const int g_adaptive_depth_conflict_resolver = 1;
static const LONG g_initial_frame_summaries = 3;
static const LONG g_frame_summary_interval = 300;

static const float g_max_screen_extent = 900.0f;
static const float g_max_screen_area = 250000.0f;
static const float g_min_model_rhw = 0.000001f;
static const float g_max_model_rhw = 2.500f;
static const float g_max_model_rhw_ratio = 256.0f;
static const float g_max_triangle_aspect = 100.0f;
static const float g_spike_long_extent = 520.0f;
static const float g_spike_thin_extent = 2.0f;
static const float g_min_depth_variance = 0.000001f;
static const float g_min_rhw_variance = 0.00000001f;
static const float g_adaptive_depth_flat_span = 0.000080f;
static const float g_adaptive_depth_target_span = 0.000420f;
static const float g_adaptive_depth_max_span = 0.000900f;
static const float g_adaptive_depth_max_shift = 0.000300f;
static const float g_adaptive_depth_min_area = 4.0f;
static const float g_adaptive_depth_max_area = 250000.0f;
static const float g_adaptive_depth_min_extent = 1.5f;
static const float g_adaptive_depth_max_extent = 900.0f;
static const float g_adaptive_depth_normal_min_area = 18.0f;
static const float g_adaptive_depth_normal_min_extent = 4.0f;
static const float g_adaptive_depth_small_min_area = 3.0f;
static const float g_adaptive_depth_small_min_extent = 1.20f;
static const float g_adaptive_depth_small_strength = 0.70f;
static const float g_adaptive_depth_rhw_signal = 0.00000001f;
static const float g_adaptive_depth_axis_signal = 1.0f;
static const ZfixCallsiteProfile g_callsite_profiles[] = {
  {
    "dc1-up-model-default",
    0u,
    0u,
    ZFIX_DEPTH_PROFILE_NORMAL,
    D3DCMP_LESSEQUAL,
    0.000320f,
    0.000700f,
    0.000190f,
    18.0f,
    250000.0f,
    4.0f,
    900.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0u,
    0u,
    0u,
    0.000110f,
    0.40f,
    0.000240f
  }
};

static void PatchAllImports(void);
static void PatchD3D9Device(void* device);
static void* WINAPI Hook_Direct3DCreate9(UINT sdk_version);
static HRESULT STDMETHODCALLTYPE Hook_D3D9_CreateDevice(void* self, UINT adapter, DWORD device_type, HWND focus,
                                                       DWORD behavior_flags, void* presentation_parameters,
                                                       void** returned_device);
static HRESULT STDMETHODCALLTYPE Hook_D3D9_Reset(void* self, void* presentation_parameters);
static HRESULT STDMETHODCALLTYPE Hook_D3D9_BeginScene(void* self);
static HRESULT STDMETHODCALLTYPE Hook_D3D9_EndScene(void* self);
static HRESULT STDMETHODCALLTYPE Hook_D3D9_SetRenderState(void* self, DWORD state, DWORD value);
static HRESULT STDMETHODCALLTYPE Hook_D3D9_SetTexture(void* self, DWORD stage, void* texture);
static HRESULT STDMETHODCALLTYPE Hook_D3D9_SetFVF(void* self, DWORD fvf);
static HRESULT STDMETHODCALLTYPE Hook_D3D9_DrawPrimitive(void* self, DWORD primitive_type, UINT start_vertex,
                                                        UINT primitive_count);
static HRESULT STDMETHODCALLTYPE Hook_D3D9_DrawIndexedPrimitive(void* self, DWORD primitive_type,
                                                               INT base_vertex_index, UINT min_vertex_index,
                                                               UINT num_vertices, UINT start_index,
                                                               UINT primitive_count);
static HRESULT STDMETHODCALLTYPE Hook_D3D9_DrawPrimitiveUP(void* self, DWORD primitive_type, UINT primitive_count,
                                                          const void* vertex_data, UINT vertex_stride);
static HRESULT STDMETHODCALLTYPE Hook_D3D9_DrawIndexedPrimitiveUP(void* self, DWORD primitive_type,
                                                                 UINT min_vertex_index, UINT num_vertices,
                                                                 UINT primitive_count, const void* index_data,
                                                                 DWORD index_format, const void* vertex_data,
                                                                 UINT vertex_stride);

static void LogLine(const char* fmt, ...)
{
  if (!g_log_enabled || !g_diagnostics)
    return;

  va_list args;
  va_start(args, fmt);
  ZfixLogLineV(1, g_log_path, fmt, args);
  va_end(args);
}

static LONG LogCounterIncrement(volatile LONG* counter)
{
  return (g_log_enabled && counter) ? InterlockedIncrement(counter) : 0;
}

static LONG LogCounterAdd(volatile LONG* counter, LONG value)
{
  return (g_log_enabled && counter) ? InterlockedExchangeAdd(counter, value) + value : 0;
}

static void BuildLogPath(HINSTANCE instance)
{
  ZfixBuildLogPath(instance, NULL, 0, g_log_path, sizeof(g_log_path), "dc1_zfix.log");
}

static void* GetOriginal(void** vtable, int slot)
{
  return ZfixGetOriginal(g_hooks, &g_hook_count, vtable, slot);
}

static int PatchVTableSlot(void* obj, int slot, void* hook)
{
  return ZfixPatchVTableSlot(g_hooks, (LONG)ARRAYSIZE(g_hooks),
                             &g_hook_count, obj, slot, hook);
}

static const ZfixCallsiteProfile* FindModelCallsiteProfile(DWORD caller)
{
  if (!g_callsite_profiles_enabled)
    return NULL;
  return ZfixFindCallsiteProfile(g_callsite_profiles, (DWORD)ARRAYSIZE(g_callsite_profiles), caller);
}

static void FillProfileMatchInfo(DWORD caller, const DrawBounds* bounds,
                                 ZfixProfileMatchInfo* info)
{
  if (!info)
    return;
  memset(info, 0, sizeof(*info));
  info->caller = caller;
  if (bounds)
  {
    const float width = AbsF(bounds->width);
    const float height = AbsF(bounds->height);
    info->z_span = bounds->max_z - bounds->min_z;
    info->area = bounds->area;
    info->extent = width > height ? width : height;
  }
  info->texture_handle = (DWORD)(uintptr_t)g_current_texture0;
  info->texture_width = g_current_texture0_width;
  info->texture_height = g_current_texture0_height;
}

static const ZfixCallsiteProfile* FindModelCallsiteProfileForDraw(DWORD caller,
                                                                  const DrawBounds* bounds)
{
  if (!g_callsite_profiles_enabled)
    return NULL;
  if (!bounds)
    return FindModelCallsiteProfile(caller);

  ZfixProfileMatchInfo info;
  FillProfileMatchInfo(caller, bounds, &info);
  return ZfixFindCallsiteProfileForDraw(g_callsite_profiles,
                                        (DWORD)ARRAYSIZE(g_callsite_profiles), &info);
}

static int CanResolveAdaptiveFlatDepth(const ZfixCallsiteProfile* profile)
{
  return g_adaptive_depth_conflict_resolver && ZfixProfileDepthEnabled(profile);
}

static void ReleaseComObject(void* obj)
{
  if (!obj)
    return;
  ComReleaseProc release = (ComReleaseProc)GetVTableSlot(obj, 2);
  if (release)
    release(obj);
}

static int GetTexture2DDesc(void* texture, D3DSURFACE_DESC_COMPAT* desc)
{
  if (!texture || !desc)
    return 0;

  D3D9ResourceGetTypeProc get_type = (D3D9ResourceGetTypeProc)GetVTableSlot(texture, 10);
  if (get_type && get_type(texture) != D3DRTYPE_TEXTURE)
    return 0;

  memset(desc, 0, sizeof(*desc));
  D3D9TextureGetLevelDescProc get_desc = (D3D9TextureGetLevelDescProc)GetVTableSlot(texture, 17);
  return get_desc && SUCCEEDED(get_desc(texture, 0, desc));
}

static void UpdateCurrentTexture0Desc(void* texture)
{
  g_current_texture0_width = 0;
  g_current_texture0_height = 0;
  if (!texture)
    return;

  D3DSURFACE_DESC_COMPAT desc;
  if (!GetTexture2DDesc(texture, &desc))
    return;

  g_current_texture0_width = desc.Width;
  g_current_texture0_height = desc.Height;
}

static void ReleaseOwnedDepthSurface(void)
{
  if (!g_owned_depth_surface)
    return;

  LogLine("owned-depth release surface=%p size=%ux%u fmt=%lu",
          g_owned_depth_surface, g_owned_depth_width, g_owned_depth_height,
          g_owned_depth_format);
  ReleaseComObject(g_owned_depth_surface);
  g_owned_depth_device = NULL;
  g_owned_depth_surface = NULL;
  g_owned_depth_width = 0;
  g_owned_depth_height = 0;
  g_owned_depth_format = 0;
}

static int GetSurfaceDescCompat(void* surface, D3DSURFACE_DESC_COMPAT* desc)
{
  if (!surface || !desc)
    return 0;

  memset(desc, 0, sizeof(*desc));
  D3D9SurfaceGetDescProc get_desc = (D3D9SurfaceGetDescProc)GetVTableSlot(surface, 12);
  return get_desc && SUCCEEDED(get_desc(surface, desc));
}

static int GetBackBufferDesc(void* self, D3DSURFACE_DESC_COMPAT* desc)
{
  D3D9GetBackBufferProc get_back_buffer = (D3D9GetBackBufferProc)GetVTableSlot(self, 18);
  if (!get_back_buffer)
    return 0;

  void* back_buffer = NULL;
  HRESULT hr = get_back_buffer(self, 0, 0, D3DBACKBUFFER_TYPE_MONO, &back_buffer);
  if (FAILED(hr) || !back_buffer)
  {
    const LONG failures = LogCounterIncrement(&g_owned_depth_failures);
    if (failures <= 16)
      LogLine("owned-depth backbuffer fail #%ld hr=0x%08X", failures, (unsigned)hr);
    return 0;
  }

  const int ok = GetSurfaceDescCompat(back_buffer, desc);
  if (!ok)
  {
    const LONG failures = LogCounterIncrement(&g_owned_depth_failures);
    if (failures <= 16)
      LogLine("owned-depth backbuffer desc fail #%ld surface=%p", failures, back_buffer);
  }
  ReleaseComObject(back_buffer);
  return ok;
}

static void LogCurrentDepthSurface(void* self, const char* reason)
{
  static volatile LONG logs = 0;
  const LONG log_id = LogCounterIncrement(&logs);
  if (log_id > 24)
    return;

  D3D9GetDepthStencilSurfaceProc get_depth =
    (D3D9GetDepthStencilSurfaceProc)GetVTableSlot(self, 40);
  if (!get_depth)
  {
    LogLine("depth-surface #%ld %s getDepth=no", log_id, reason ? reason : "unknown");
    return;
  }

  void* current = NULL;
  HRESULT hr = get_depth(self, &current);
  if (FAILED(hr) || !current)
  {
    LogLine("depth-surface #%ld %s getDepth hr=0x%08X surface=%p",
            log_id, reason ? reason : "unknown", (unsigned)hr, current);
    return;
  }

  D3DSURFACE_DESC_COMPAT desc;
  const int has_desc = GetSurfaceDescCompat(current, &desc);
  LogLine("depth-surface #%ld %s surface=%p owned=%d desc=%d size=%ux%u fmt=%lu ms=%lu/%lu",
          log_id, reason ? reason : "unknown", current,
          current == g_owned_depth_surface, has_desc,
          has_desc ? desc.Width : 0, has_desc ? desc.Height : 0,
          has_desc ? desc.Format : 0, has_desc ? desc.MultiSampleType : 0,
          has_desc ? desc.MultiSampleQuality : 0);
  ReleaseComObject(current);
}

static int SetOwnedDepthSurface(void* self, const char* reason)
{
  if (!self || !g_owned_depth_surface || self != g_owned_depth_device)
    return 0;

  D3D9SetDepthStencilSurfaceProc set_depth =
    (D3D9SetDepthStencilSurfaceProc)GetVTableSlot(self, 39);
  if (!set_depth)
    return 0;

  HRESULT hr = set_depth(self, g_owned_depth_surface);
  if (SUCCEEDED(hr))
  {
    const LONG sets = LogCounterIncrement(&g_owned_depth_sets);
    if (sets <= 32)
      LogLine("owned-depth set #%ld %s surface=%p size=%ux%u fmt=%lu",
              sets, reason ? reason : "unknown", g_owned_depth_surface,
              g_owned_depth_width, g_owned_depth_height, g_owned_depth_format);
    return 1;
  }

  const LONG failures = LogCounterIncrement(&g_owned_depth_failures);
  if (failures <= 16)
    LogLine("owned-depth set fail #%ld %s hr=0x%08X surface=%p",
            failures, reason ? reason : "unknown", (unsigned)hr, g_owned_depth_surface);
  return 0;
}

static int EnsureOwnedDepthSurface(void* self, const char* reason)
{
  if (!g_force_owned_depth_surface || !self)
    return 0;

  if (g_owned_depth_surface && g_owned_depth_device && g_owned_depth_device != self)
    ReleaseOwnedDepthSurface();

  D3DSURFACE_DESC_COMPAT bb_desc;
  if (!GetBackBufferDesc(self, &bb_desc) || !bb_desc.Width || !bb_desc.Height)
    return 0;

  if (g_owned_depth_surface &&
      g_owned_depth_width == bb_desc.Width &&
      g_owned_depth_height == bb_desc.Height)
  {
    return SetOwnedDepthSurface(self, reason);
  }

  ReleaseOwnedDepthSurface();

  D3D9CreateDepthStencilSurfaceProc create_depth =
    (D3D9CreateDepthStencilSurfaceProc)GetVTableSlot(self, 29);
  if (!create_depth)
    return 0;

  const DWORD formats[] = {
    D3DFMT_D24X8,
    D3DFMT_D24S8,
    D3DFMT_D32,
    D3DFMT_D16,
  };
  const DWORD ms_type = bb_desc.MultiSampleType;
  const DWORD ms_quality = bb_desc.MultiSampleQuality;

  for (DWORD i = 0; i < (DWORD)ARRAYSIZE(formats); i++)
  {
    void* surface = NULL;
    HRESULT hr = create_depth(self, bb_desc.Width, bb_desc.Height, formats[i],
                              ms_type, ms_quality, TRUE, &surface, NULL);
    if (FAILED(hr) && ms_type != D3DMULTISAMPLE_NONE)
    {
      hr = create_depth(self, bb_desc.Width, bb_desc.Height, formats[i],
                        D3DMULTISAMPLE_NONE, 0, TRUE, &surface, NULL);
    }
    if (FAILED(hr) || !surface)
    {
      const LONG failures = LogCounterIncrement(&g_owned_depth_failures);
      if (failures <= 16)
        LogLine("owned-depth create fail #%ld %s hr=0x%08X size=%ux%u fmt=%lu bbFmt=%lu ms=%lu/%lu",
                failures, reason ? reason : "unknown", (unsigned)hr,
                bb_desc.Width, bb_desc.Height, formats[i], bb_desc.Format,
                ms_type, ms_quality);
      continue;
    }

    g_owned_depth_surface = surface;
    g_owned_depth_device = self;
    g_owned_depth_width = bb_desc.Width;
    g_owned_depth_height = bb_desc.Height;
    g_owned_depth_format = formats[i];

    const LONG creates = LogCounterIncrement(&g_owned_depth_creates);
    LogLine("owned-depth create #%ld %s surface=%p size=%ux%u fmt=%lu bbFmt=%lu ms=%lu/%lu",
            creates, reason ? reason : "unknown", surface,
            g_owned_depth_width, g_owned_depth_height, g_owned_depth_format,
            bb_desc.Format, ms_type, ms_quality);

    if (SetOwnedDepthSurface(self, reason))
    {
      LogCurrentDepthSurface(self, reason);
      return 1;
    }

    ReleaseOwnedDepthSurface();
  }

  return 0;
}

static void* RvaToPtr(BYTE* module, DWORD rva)
{
  if (!rva)
    return NULL;
  return module + rva;
}

static int PatchModuleImport(HMODULE module, const char* dll_name, const char* proc_name,
                             void* hook, void** original)
{
  BYTE* base = (BYTE*)module;
  IMAGE_DOS_HEADER* dos = (IMAGE_DOS_HEADER*)base;
  if (!base || dos->e_magic != IMAGE_DOS_SIGNATURE)
    return 0;

  IMAGE_NT_HEADERS* nt = (IMAGE_NT_HEADERS*)(base + dos->e_lfanew);
  if (nt->Signature != IMAGE_NT_SIGNATURE)
    return 0;

  IMAGE_DATA_DIRECTORY dir = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
  if (!dir.VirtualAddress)
    return 0;

  int patched = 0;
  IMAGE_IMPORT_DESCRIPTOR* desc = (IMAGE_IMPORT_DESCRIPTOR*)RvaToPtr(base, dir.VirtualAddress);
  for (; desc && desc->Name; desc++)
  {
    const char* imported_dll = (const char*)RvaToPtr(base, desc->Name);
    if (!imported_dll || _stricmp(imported_dll, dll_name) != 0)
      continue;

    IMAGE_THUNK_DATA* orig_thunk = (IMAGE_THUNK_DATA*)RvaToPtr(base, desc->OriginalFirstThunk);
    IMAGE_THUNK_DATA* thunk = (IMAGE_THUNK_DATA*)RvaToPtr(base, desc->FirstThunk);
    if (!orig_thunk)
      orig_thunk = thunk;

    for (; orig_thunk && thunk && orig_thunk->u1.AddressOfData; orig_thunk++, thunk++)
    {
#ifdef IMAGE_ORDINAL_FLAG32
      if (orig_thunk->u1.Ordinal & IMAGE_ORDINAL_FLAG32)
#else
      if (orig_thunk->u1.Ordinal & IMAGE_ORDINAL_FLAG)
#endif
        continue;

      IMAGE_IMPORT_BY_NAME* by_name = (IMAGE_IMPORT_BY_NAME*)RvaToPtr(base, (DWORD)orig_thunk->u1.AddressOfData);
      if (!by_name || strcmp((const char*)by_name->Name, proc_name) != 0)
        continue;

      void** target = (void**)&thunk->u1.Function;
      if (*target == hook)
        continue;
      if (original && !*original)
        *original = *target;

      DWORD old_protect = 0;
      if (!VirtualProtect(target, sizeof(void*), PAGE_READWRITE, &old_protect))
        continue;
      *target = hook;
      DWORD ignored = 0;
      VirtualProtect(target, sizeof(void*), old_protect, &ignored);
      FlushInstructionCache(GetCurrentProcess(), target, sizeof(void*));
      patched++;
    }
  }
  return patched;
}

static void PatchLoadedModuleImports(HMODULE module)
{
  int patched = PatchModuleImport(module, "d3d9.dll", "Direct3DCreate9",
                                  (void*)Hook_Direct3DCreate9, (void**)&g_real_direct3d_create9);
  if (patched)
    LogLine("patched Direct3DCreate9 imports=%d module=%p", patched, module);
}

static void PatchAllImports(void)
{
  HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
  if (snap == INVALID_HANDLE_VALUE)
  {
    PatchLoadedModuleImports(GetModuleHandleA(NULL));
    return;
  }

  MODULEENTRY32 me;
  memset(&me, 0, sizeof(me));
  me.dwSize = sizeof(me);
  if (Module32First(snap, &me))
  {
    do
    {
      PatchLoadedModuleImports(me.hModule);
    } while (Module32Next(snap, &me));
  }
  CloseHandle(snap);
}

static DWORD VertexCountForPrimitive(DWORD primitive_type, UINT primitive_count)
{
  switch (primitive_type)
  {
    case D3DPT_TRIANGLELIST:
      return primitive_count * 3;
    case D3DPT_TRIANGLESTRIP:
    case D3DPT_TRIANGLEFAN:
      return primitive_count + 2;
    default:
      return 0;
  }
}

static void ComputeDrawBounds(const D3D9TLVERTEX* vertices, DWORD count, DrawBounds* bounds)
{
  memset(bounds, 0, sizeof(*bounds));
  if (!vertices || count == 0)
    return;

  bounds->min_x = bounds->max_x = vertices[0].sx;
  bounds->min_y = bounds->max_y = vertices[0].sy;
  bounds->min_z = bounds->max_z = vertices[0].sz;
  bounds->min_rhw = bounds->max_rhw = vertices[0].rhw;
  for (DWORD i = 1; i < count; i++)
  {
    if (vertices[i].sx < bounds->min_x)
      bounds->min_x = vertices[i].sx;
    if (vertices[i].sx > bounds->max_x)
      bounds->max_x = vertices[i].sx;
    if (vertices[i].sy < bounds->min_y)
      bounds->min_y = vertices[i].sy;
    if (vertices[i].sy > bounds->max_y)
      bounds->max_y = vertices[i].sy;
    if (vertices[i].sz < bounds->min_z)
      bounds->min_z = vertices[i].sz;
    if (vertices[i].sz > bounds->max_z)
      bounds->max_z = vertices[i].sz;
    if (vertices[i].rhw < bounds->min_rhw)
      bounds->min_rhw = vertices[i].rhw;
    if (vertices[i].rhw > bounds->max_rhw)
      bounds->max_rhw = vertices[i].rhw;
  }

  bounds->width = bounds->max_x - bounds->min_x;
  bounds->height = bounds->max_y - bounds->min_y;
  bounds->area = bounds->width * bounds->height;
}

static int IsAxisRectInScreenAndUV(const D3D9TLVERTEX* vertices, DWORD vertex_count)
{
  if (!g_skip_axis_tile_draws || !vertices || vertex_count < 3)
    return 0;

  float min_x = vertices[0].sx;
  float max_x = vertices[0].sx;
  float min_y = vertices[0].sy;
  float max_y = vertices[0].sy;
  float min_u = vertices[0].tu;
  float max_u = vertices[0].tu;
  float min_v = vertices[0].tv;
  float max_v = vertices[0].tv;
  for (DWORD i = 1; i < vertex_count; i++)
  {
    if (vertices[i].sx < min_x)
      min_x = vertices[i].sx;
    if (vertices[i].sx > max_x)
      max_x = vertices[i].sx;
    if (vertices[i].sy < min_y)
      min_y = vertices[i].sy;
    if (vertices[i].sy > max_y)
      max_y = vertices[i].sy;
    if (vertices[i].tu < min_u)
      min_u = vertices[i].tu;
    if (vertices[i].tu > max_u)
      max_u = vertices[i].tu;
    if (vertices[i].tv < min_v)
      min_v = vertices[i].tv;
    if (vertices[i].tv > max_v)
      max_v = vertices[i].tv;
  }

  const float width = max_x - min_x;
  const float height = max_y - min_y;
  if (width < 24.0f || height < 24.0f || (max_u - min_u) < 0.0001f || (max_v - min_v) < 0.0001f)
    return 0;

  for (DWORD i = 0; i < vertex_count; i++)
  {
    if (!NearF(vertices[i].sx, min_x, 0.05f) && !NearF(vertices[i].sx, max_x, 0.05f))
      return 0;
    if (!NearF(vertices[i].sy, min_y, 0.05f) && !NearF(vertices[i].sy, max_y, 0.05f))
      return 0;
    if (!NearF(vertices[i].tu, min_u, 0.0015f) && !NearF(vertices[i].tu, max_u, 0.0015f))
      return 0;
    if (!NearF(vertices[i].tv, min_v, 0.0015f) && !NearF(vertices[i].tv, max_v, 0.0015f))
      return 0;
  }

  return 1;
}

static int HasTransparentVertex(const D3D9TLVERTEX* vertices, DWORD vertex_count)
{
  if (!vertices || vertex_count == 0 || g_min_vertex_alpha <= 0)
    return 0;

  for (DWORD i = 0; i < vertex_count; i++)
  {
    const int alpha = (int)((vertices[i].color >> 24) & 0xFFu);
    if (alpha < g_min_vertex_alpha)
      return 1;
  }
  return 0;
}

static int MinVertexAlpha(const D3D9TLVERTEX* vertices, DWORD vertex_count)
{
  if (!vertices || vertex_count == 0)
    return 255;

  int min_alpha = 255;
  for (DWORD i = 0; i < vertex_count; i++)
  {
    const int alpha = (int)((vertices[i].color >> 24) & 0xFFu);
    if (alpha < min_alpha)
      min_alpha = alpha;
  }
  return min_alpha;
}

static int MaxVertexAlpha(const D3D9TLVERTEX* vertices, DWORD vertex_count)
{
  if (!vertices || vertex_count == 0)
    return 255;

  int max_alpha = 0;
  for (DWORD i = 0; i < vertex_count; i++)
  {
    const int alpha = (int)((vertices[i].color >> 24) & 0xFFu);
    if (alpha > max_alpha)
      max_alpha = alpha;
  }
  return max_alpha;
}

static int HasHardAlphaSpread(const D3D9TLVERTEX* vertices, DWORD vertex_count)
{
  if (!vertices || vertex_count == 0)
    return 0;

  int low = 0;
  int high = 0;
  for (DWORD i = 0; i < vertex_count; i++)
  {
    const int alpha = (int)((vertices[i].color >> 24) & 0xFFu);
    if (alpha <= g_cutout_hard_alpha_low_max)
      low = 1;
    if (alpha >= g_cutout_hard_alpha_high_min)
      high = 1;
  }
  return low && high;
}

static AlphaModelClass ClassifyAlphaModel(const D3D9TLVERTEX* vertices, DWORD vertex_count,
                                          int alpha_blend_enabled, int alpha_test_enabled,
                                          int min_alpha)
{
  if (!vertices || vertex_count == 0)
    return ALPHA_MODEL_TRANSLUCENT;
  if (g_cutout_model_alpha_test && alpha_test_enabled)
    return ALPHA_MODEL_CUTOUT;
  if (min_alpha >= g_cutout_alpha_min)
    return ALPHA_MODEL_CUTOUT;
  if (HasHardAlphaSpread(vertices, vertex_count))
    return ALPHA_MODEL_CUTOUT;
  if (!alpha_blend_enabled && MaxVertexAlpha(vertices, vertex_count) >= g_cutout_hard_alpha_high_min)
    return ALPHA_MODEL_CUTOUT;
  return ALPHA_MODEL_TRANSLUCENT;
}

static int ReadUPIndex(const void* index_data, DWORD index_format, DWORD index_offset, DWORD* out_index)
{
  if (!index_data || !out_index)
    return 0;

  if (index_format == D3DFMT_INDEX16)
  {
    const uint16_t* indices = (const uint16_t*)index_data;
    *out_index = (DWORD)indices[index_offset];
    return 1;
  }
  if (index_format == D3DFMT_INDEX32)
  {
    const uint32_t* indices = (const uint32_t*)index_data;
    *out_index = (DWORD)indices[index_offset];
    return 1;
  }
  return 0;
}

static D3D9TLVERTEX* BuildIndexedVertexList(const D3D9TLVERTEX* vertices, UINT num_vertices,
                                            const void* index_data, DWORD index_format,
                                            DWORD index_count)
{
  if (!vertices || !index_data || num_vertices == 0 || index_count == 0 || index_count > 8192)
    return NULL;

  D3D9TLVERTEX* indexed = (D3D9TLVERTEX*)HeapAlloc(GetProcessHeap(), 0,
                                                   sizeof(D3D9TLVERTEX) * (SIZE_T)index_count);
  if (!indexed)
    return NULL;

  for (DWORD i = 0; i < index_count; i++)
  {
    DWORD vertex_index = 0;
    if (!ReadUPIndex(index_data, index_format, i, &vertex_index) || vertex_index >= num_vertices)
    {
      HeapFree(GetProcessHeap(), 0, indexed);
      return NULL;
    }
    indexed[i] = vertices[vertex_index];
  }
  return indexed;
}

static int ComputeIndexedUPDrawBounds(const D3D9TLVERTEX* vertices, UINT num_vertices,
                                      const void* index_data, DWORD index_format,
                                      DWORD index_count, DrawBounds* bounds)
{
  if (!vertices || !index_data || !bounds || num_vertices == 0 || index_count == 0)
    return 0;

  int initialized = 0;
  for (DWORD i = 0; i < index_count; i++)
  {
    DWORD vertex_index = 0;
    if (!ReadUPIndex(index_data, index_format, i, &vertex_index) || vertex_index >= num_vertices)
      return 0;

    const D3D9TLVERTEX* v = &vertices[vertex_index];
    if (!initialized)
    {
      bounds->min_x = bounds->max_x = v->sx;
      bounds->min_y = bounds->max_y = v->sy;
      bounds->min_z = bounds->max_z = v->sz;
      bounds->min_rhw = bounds->max_rhw = v->rhw;
      initialized = 1;
      continue;
    }
    if (v->sx < bounds->min_x) bounds->min_x = v->sx;
    if (v->sx > bounds->max_x) bounds->max_x = v->sx;
    if (v->sy < bounds->min_y) bounds->min_y = v->sy;
    if (v->sy > bounds->max_y) bounds->max_y = v->sy;
    if (v->sz < bounds->min_z) bounds->min_z = v->sz;
    if (v->sz > bounds->max_z) bounds->max_z = v->sz;
    if (v->rhw < bounds->min_rhw) bounds->min_rhw = v->rhw;
    if (v->rhw > bounds->max_rhw) bounds->max_rhw = v->rhw;
  }

  if (!initialized)
    return 0;

  bounds->width = bounds->max_x - bounds->min_x;
  bounds->height = bounds->max_y - bounds->min_y;
  bounds->area = AbsF(bounds->width * bounds->height);
  return 1;
}

static int HasModelRhwRange(const DrawBounds* bounds)
{
  if (!bounds)
    return 0;
  if (bounds->min_rhw < g_min_model_rhw || bounds->max_rhw > g_max_model_rhw)
    return 0;
  if (bounds->min_rhw > 0.0f && (bounds->max_rhw / bounds->min_rhw) > g_max_model_rhw_ratio)
    return 0;
  return 1;
}

static int IsSpikeLikeTriangle(const DrawBounds* bounds)
{
  if (!bounds)
    return 0;

  const float width = bounds->width < 0.0f ? -bounds->width : bounds->width;
  const float height = bounds->height < 0.0f ? -bounds->height : bounds->height;
  const float long_extent = width > height ? width : height;
  const float short_extent = width > height ? height : width;

  if (g_spike_long_extent > 0.0f && g_spike_thin_extent > 0.0f &&
      long_extent > g_spike_long_extent && short_extent < g_spike_thin_extent)
    return 1;
  if (g_max_triangle_aspect > 0.0f && short_extent > 0.0f &&
      (long_extent / short_extent) > g_max_triangle_aspect)
    return 1;
  return 0;
}

static int IsFlat2DLayerBounds(const DrawBounds* bounds)
{
  if (!bounds)
    return 0;

  const float z_span = bounds->max_z - bounds->min_z;
  const float rhw_span = bounds->max_rhw - bounds->min_rhw;
  const int flat_depth = z_span < 0.000010f;
  const int flat_rhw = rhw_span < 0.000010f;
  const int screen_rhw = bounds->min_rhw > 0.95f && bounds->max_rhw < 1.05f;
  const int large_2d = bounds->width > 256.0f || bounds->height > 256.0f ||
                       bounds->area > 40000.0f;

  if (flat_depth && flat_rhw && screen_rhw)
    return 1;
  if (flat_depth && flat_rhw && large_2d)
    return 1;
  if (screen_rhw && large_2d)
    return 1;
  return 0;
}

static int IsTransparentModelDepthDraw(const D3D9TLVERTEX* vertices, DWORD primitive_type,
                                       DWORD vertex_count, UINT primitive_count,
                                       DrawBounds* bounds, int* min_alpha, const char** reason)
{
  if (reason)
    *reason = "transparent_ok";
  if (!g_enabled || !g_transparent_model_z_test || !vertices ||
      vertex_count == 0 || primitive_count == 0)
  {
    if (reason)
      *reason = "transparent_off";
    return 0;
  }
  if (primitive_type != D3DPT_TRIANGLELIST &&
      primitive_type != D3DPT_TRIANGLESTRIP &&
      primitive_type != D3DPT_TRIANGLEFAN)
  {
    if (reason)
      *reason = "transparent_primitive";
    return 0;
  }

  const int local_min_alpha = MinVertexAlpha(vertices, vertex_count);
  if (min_alpha)
    *min_alpha = local_min_alpha;
  if (local_min_alpha >= g_min_vertex_alpha)
  {
    if (reason)
      *reason = "transparent_no_alpha";
    return 0;
  }

  DrawBounds local_bounds;
  ComputeDrawBounds(vertices, vertex_count, &local_bounds);
  if (bounds)
    *bounds = local_bounds;

  if (IsAxisRectInScreenAndUV(vertices, vertex_count) && IsFlat2DLayerBounds(&local_bounds))
  {
    if (reason)
      *reason = "transparent_axis";
    return 0;
  }

  if (g_max_screen_extent > 0.0f &&
      (local_bounds.width > g_max_screen_extent || local_bounds.height > g_max_screen_extent))
  {
    if (reason)
      *reason = "transparent_large_extent";
    return 0;
  }
  if (g_max_screen_area > 0.0f && local_bounds.area > g_max_screen_area)
  {
    if (reason)
      *reason = "transparent_large_area";
    return 0;
  }
  if (local_bounds.min_z < 0.0f || local_bounds.max_z > 1.0f)
  {
    if (reason)
      *reason = "transparent_z_range";
    return 0;
  }
  if (!HasModelRhwRange(&local_bounds))
  {
    if (reason)
      *reason = "transparent_rhw";
    return 0;
  }
  if (IsSpikeLikeTriangle(&local_bounds))
  {
    if (reason)
      *reason = "transparent_spike";
    return 0;
  }

  const float z_span = local_bounds.max_z - local_bounds.min_z;
  const float rhw_span = local_bounds.max_rhw - local_bounds.min_rhw;
  if (z_span < g_min_depth_variance && rhw_span < g_min_rhw_variance)
  {
    if (!CanResolveAdaptiveFlatDepth(NULL))
    {
      if (reason)
        *reason = "transparent_flat";
      return 0;
    }
  }
  return 1;
}

static int IsModelDepthDraw(const D3D9TLVERTEX* vertices, DWORD primitive_type, DWORD vertex_count,
                            UINT primitive_count, int alpha_blend_enabled, DrawBounds* bounds,
                            const char** reason)
{
  if (reason)
    *reason = "ok";
  if (!g_enabled || !vertices || vertex_count == 0 || primitive_count == 0)
  {
    if (reason)
      *reason = "off";
    return 0;
  }
  if (primitive_type != D3DPT_TRIANGLELIST &&
      primitive_type != D3DPT_TRIANGLESTRIP &&
      primitive_type != D3DPT_TRIANGLEFAN)
  {
    if (reason)
      *reason = "primitive";
    return 0;
  }
  if ((!g_reject_alpha_only_when_blending || alpha_blend_enabled) &&
      HasTransparentVertex(vertices, vertex_count))
  {
    if (reason)
      *reason = "alpha";
    return 0;
  }

  DrawBounds local_bounds;
  ComputeDrawBounds(vertices, vertex_count, &local_bounds);
  if (bounds)
    *bounds = local_bounds;

  if (IsAxisRectInScreenAndUV(vertices, vertex_count) && IsFlat2DLayerBounds(&local_bounds))
  {
    if (reason)
      *reason = "axis";
    return 0;
  }

  if (g_max_screen_extent > 0.0f &&
      (local_bounds.width > g_max_screen_extent || local_bounds.height > g_max_screen_extent))
  {
    if (reason)
      *reason = "large_extent";
    return 0;
  }
  if (g_max_screen_area > 0.0f && local_bounds.area > g_max_screen_area)
  {
    if (reason)
      *reason = "large_area";
    return 0;
  }
  if (local_bounds.min_z < 0.0f || local_bounds.max_z > 1.0f)
  {
    if (reason)
      *reason = "z_range";
    return 0;
  }
  if (!HasModelRhwRange(&local_bounds))
  {
    if (reason)
      *reason = "rhw";
    return 0;
  }
  if (IsSpikeLikeTriangle(&local_bounds))
  {
    if (reason)
      *reason = "spike";
    return 0;
  }

  const float z_span = local_bounds.max_z - local_bounds.min_z;
  const float rhw_span = local_bounds.max_rhw - local_bounds.min_rhw;
  if (z_span < g_min_depth_variance && rhw_span < g_min_rhw_variance)
  {
    if (!CanResolveAdaptiveFlatDepth(NULL))
    {
      if (reason)
        *reason = "flat";
      return 0;
    }
  }
  return 1;
}

static DWORD ApplyAdaptiveDepthConflictResolver(D3D9TLVERTEX* vertices, DWORD vertex_count,
                                                const DrawBounds* bounds,
                                                const ZfixCallsiteProfile* profile,
                                                DWORD caller, const char* label)
{
  if (!g_adaptive_depth_conflict_resolver || !vertices || vertex_count == 0 ||
      !bounds || !ZfixProfileDepthEnabled(profile))
    return 0;

  const float width = AbsF(bounds->width);
  const float height = AbsF(bounds->height);
  const float extent = width > height ? width : height;
  const int aggressive = ZfixProfileIsAggressive(profile);
  const float min_area = ZfixFloatOrDefault(profile ? profile->min_area : 0.0f,
                                            g_adaptive_depth_min_area);
  const float max_area = ZfixFloatOrDefault(profile ? profile->max_area : 0.0f,
                                            g_adaptive_depth_max_area);
  const float min_extent = ZfixFloatOrDefault(profile ? profile->min_extent : 0.0f,
                                              g_adaptive_depth_min_extent);
  const float max_extent = ZfixFloatOrDefault(profile ? profile->max_extent : 0.0f,
                                              g_adaptive_depth_max_extent);

  const int below_profile_floor = bounds->area < min_area || extent < min_extent;
  const int small_normal = !aggressive && below_profile_floor;
  if (bounds->area > max_area || extent > max_extent)
    return 0;
  if (below_profile_floor && !small_normal)
    return 0;
  if (small_normal &&
      (bounds->area < g_adaptive_depth_small_min_area ||
       extent < g_adaptive_depth_small_min_extent))
    return 0;
  if (!aggressive && !small_normal &&
      (bounds->area < g_adaptive_depth_normal_min_area ||
       extent < g_adaptive_depth_normal_min_extent))
    return 0;

  const float z_span = bounds->max_z - bounds->min_z;
  const float rhw_span = bounds->max_rhw - bounds->min_rhw;
  if (z_span < 0.0f || bounds->min_z < 0.0f || bounds->max_z > 1.0f)
    return 0;

  float target_span = ZfixFloatOrDefault(profile ? profile->conflict_min_span : 0.0f,
                                         g_adaptive_depth_target_span);
  const float max_span = ZfixFloatOrDefault(profile ? profile->conflict_max_span : 0.0f,
                                            g_adaptive_depth_max_span);
  const float raw_max_shift = ZfixFloatOrDefault(profile ? profile->conflict_max_shift : 0.0f,
                                                 g_adaptive_depth_max_shift);
  const float max_shift = ZfixProfileGuardedMaxShift(profile, z_span, raw_max_shift);
  if (max_span > 0.0f && target_span > max_span)
    target_span = max_span;

  if (target_span <= z_span)
    return 0;

  if (!aggressive && z_span > g_adaptive_depth_flat_span)
  {
    const int near_flat = z_span <= (target_span * 0.35f) &&
                          rhw_span <= (g_adaptive_depth_rhw_signal * 256.0f);
    if (!near_flat)
      return 0;
  }

  float effective_span = small_normal ? (target_span * g_adaptive_depth_small_strength) : target_span;
  if (max_shift > 0.0f && effective_span > (z_span + (max_shift * 2.0f)))
    effective_span = z_span + (max_shift * 2.0f);
  if (effective_span <= z_span)
    return 0;

  float center_z = (bounds->min_z + bounds->max_z) * 0.5f;
  const float half_span = effective_span * 0.5f;
  if (center_z < half_span)
    center_z = half_span;
  if (center_z > (1.0f - half_span))
    center_z = 1.0f - half_span;

  enum {
    ADAPTIVE_BY_Z = 0,
    ADAPTIVE_BY_RHW = 1,
    ADAPTIVE_BY_X = 2,
    ADAPTIVE_BY_Y = 3,
    ADAPTIVE_BY_ORDER = 4
  } mode = ADAPTIVE_BY_ORDER;
  const char* mode_name = "order";
  float source_min = 0.0f;
  float source_span = 0.0f;

  if (z_span > 0.00000020f)
  {
    mode = ADAPTIVE_BY_Z;
    mode_name = "z";
    source_min = bounds->min_z;
    source_span = z_span;
  }
  else if (rhw_span > g_adaptive_depth_rhw_signal)
  {
    mode = ADAPTIVE_BY_RHW;
    mode_name = "rhw";
    source_min = bounds->min_rhw;
    source_span = rhw_span;
  }
  else if (width >= height && width > g_adaptive_depth_axis_signal)
  {
    mode = ADAPTIVE_BY_X;
    mode_name = "x";
    source_min = bounds->min_x;
    source_span = width;
  }
  else if (height > g_adaptive_depth_axis_signal)
  {
    mode = ADAPTIVE_BY_Y;
    mode_name = "y";
    source_min = bounds->min_y;
    source_span = height;
  }
  if (!aggressive && mode == ADAPTIVE_BY_ORDER)
    return 0;

  DWORD changed = 0;
  for (DWORD i = 0; i < vertex_count; i++)
  {
    float t = 0.5f;
    switch (mode)
    {
      case ADAPTIVE_BY_Z:
        t = (vertices[i].sz - source_min) / source_span;
        break;
      case ADAPTIVE_BY_RHW:
        t = (vertices[i].rhw - source_min) / source_span;
        break;
      case ADAPTIVE_BY_X:
        t = (vertices[i].sx - source_min) / source_span;
        break;
      case ADAPTIVE_BY_Y:
        t = (vertices[i].sy - source_min) / source_span;
        break;
      default:
        t = (vertex_count > 1) ? ((float)i / (float)(vertex_count - 1)) : 0.5f;
        break;
    }
    t = Clamp01(t);

    const float original_z = vertices[i].sz;
    const float target_z = center_z + ((t - 0.5f) * effective_span);
    float delta = target_z - original_z;
    if (max_shift > 0.0f)
    {
      if (delta > max_shift)
        delta = max_shift;
      else if (delta < -max_shift)
        delta = -max_shift;
    }
    const float new_z = ClampDepth(original_z + delta);
    if (AbsF(new_z - original_z) > 0.00000001f)
    {
      vertices[i].sz = new_z;
      changed++;
    }
  }

  if (!changed)
    return 0;

  LogCounterIncrement(&g_adaptive_depth_draws);
  LogCounterAdd(&g_adaptive_depth_vertices, (LONG)changed);
  if (g_log_enabled)
  {
    const LONG logged = LogCounterIncrement(&g_adaptive_depth_logged);
    if (logged <= 96)
    {
      LogLine("adaptive-depth #%ld %s profile=%s caller=0x%08lX verts=%lu changed=%lu "
              "mode=%s zSpan=%.8f target=%.8f rhwSpan=%.8f area=%.2f extent=%.2f maxShift=%.8f",
              logged, label ? label : "draw", profile ? profile->name : "default",
              caller, vertex_count, changed, mode_name, z_span, effective_span,
              rhw_span, bounds->area, extent, max_shift);
    }
  }
  return changed;
}

static int CaptureRenderState(void* self, DWORD state, DWORD* value)
{
  D3D9GetRenderStateProc get_rs = (D3D9GetRenderStateProc)GetVTableSlot(self, 58);
  return get_rs && SUCCEEDED(get_rs(self, state, value));
}

static void SetOneRenderState(void* self, DWORD state, DWORD value)
{
  D3D9SetRenderStateProc set_rs = (D3D9SetRenderStateProc)GetVTableSlot(self, 57);
  if (set_rs)
    set_rs(self, state, value);
}

static void CaptureState(void* self, D3D9StateSnapshot* snapshot)
{
  memset(snapshot, 0, sizeof(*snapshot));
  snapshot->has_z_enable = CaptureRenderState(self, D3DRS_ZENABLE, &snapshot->z_enable);
  snapshot->has_z_write = CaptureRenderState(self, D3DRS_ZWRITEENABLE, &snapshot->z_write);
  snapshot->has_z_func = CaptureRenderState(self, D3DRS_ZFUNC, &snapshot->z_func);
  snapshot->has_alpha_test = CaptureRenderState(self, D3DRS_ALPHATESTENABLE, &snapshot->alpha_test);
  snapshot->has_alpha_ref = CaptureRenderState(self, D3DRS_ALPHAREF, &snapshot->alpha_ref);
  snapshot->has_alpha_func = CaptureRenderState(self, D3DRS_ALPHAFUNC, &snapshot->alpha_func);
  snapshot->has_alpha_blend = CaptureRenderState(self, D3DRS_ALPHABLENDENABLE, &snapshot->alpha_blend);
  snapshot->has_src_blend = CaptureRenderState(self, D3DRS_SRCBLEND, &snapshot->src_blend);
  snapshot->has_dest_blend = CaptureRenderState(self, D3DRS_DESTBLEND, &snapshot->dest_blend);
  snapshot->has_cull_mode = CaptureRenderState(self, D3DRS_CULLMODE, &snapshot->cull_mode);
  snapshot->has_color_write = CaptureRenderState(self, D3DRS_COLORWRITEENABLE, &snapshot->color_write);
  snapshot->has_shade_mode = CaptureRenderState(self, D3DRS_SHADEMODE, &snapshot->shade_mode);
  snapshot->has_dither_enable = CaptureRenderState(self, D3DRS_DITHERENABLE, &snapshot->dither_enable);
}

static void ForceModelAlphaTest(void* self, const D3D9StateSnapshot* snapshot)
{
  if (!g_model_alpha_test || !snapshot)
    return;
  if (snapshot->has_alpha_test)
    SetOneRenderState(self, D3DRS_ALPHATESTENABLE, 1);
  if (snapshot->has_alpha_ref)
    SetOneRenderState(self, D3DRS_ALPHAREF, g_model_alpha_ref);
  if (snapshot->has_alpha_func)
    SetOneRenderState(self, D3DRS_ALPHAFUNC, g_model_alpha_func);
}

static void ForceCutoutGeometryState(void* self, const D3D9StateSnapshot* snapshot)
{
  if (!snapshot)
    return;

  if (g_cutout_model_alpha_test)
  {
    if (snapshot->has_alpha_test)
      SetOneRenderState(self, D3DRS_ALPHATESTENABLE, 1);
    if (snapshot->has_alpha_ref)
      SetOneRenderState(self, D3DRS_ALPHAREF, g_cutout_model_alpha_ref);
    if (snapshot->has_alpha_func)
      SetOneRenderState(self, D3DRS_ALPHAFUNC, g_model_alpha_func);
  }
  if (g_cutout_model_two_sided && snapshot->has_cull_mode)
    SetOneRenderState(self, D3DRS_CULLMODE, D3DCULL_NONE);
  if (g_cutout_model_disable_alpha_blend && snapshot->has_alpha_blend)
    SetOneRenderState(self, D3DRS_ALPHABLENDENABLE, 0);
}

static void ForceModelState(void* self, const D3D9StateSnapshot* snapshot)
{
  if (g_force_z_enable && snapshot->has_z_enable)
    SetOneRenderState(self, D3DRS_ZENABLE, 1);
  if (g_force_z_write && snapshot->has_z_write)
    SetOneRenderState(self, D3DRS_ZWRITEENABLE, 1);
  if (snapshot->has_z_func)
    SetOneRenderState(self, D3DRS_ZFUNC, (DWORD)g_force_z_func);
  ForceModelAlphaTest(self, snapshot);
  if (g_model_gouraud_shading && snapshot->has_shade_mode)
    SetOneRenderState(self, D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
  if (g_model_dither && snapshot->has_dither_enable)
    SetOneRenderState(self, D3DRS_DITHERENABLE, 1);
}

static void ForceTransparentModelState(void* self, const D3D9StateSnapshot* snapshot, int write_depth)
{
  if (g_force_z_enable && snapshot->has_z_enable)
    SetOneRenderState(self, D3DRS_ZENABLE, 1);
  if (snapshot->has_z_write)
    SetOneRenderState(self, D3DRS_ZWRITEENABLE, write_depth ? 1 : 0);
  if (snapshot->has_z_func)
    SetOneRenderState(self, D3DRS_ZFUNC, (DWORD)g_force_z_func);
  if (g_cutout_model_alpha_test && write_depth)
    ForceModelAlphaTest(self, snapshot);
  if (g_model_gouraud_shading && snapshot->has_shade_mode)
    SetOneRenderState(self, D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
  if (g_model_dither && snapshot->has_dither_enable)
    SetOneRenderState(self, D3DRS_DITHERENABLE, 1);
}

static void RestoreState(void* self, const D3D9StateSnapshot* snapshot)
{
  if (!g_restore_state || !snapshot)
    return;
  if (snapshot->has_z_enable)
    SetOneRenderState(self, D3DRS_ZENABLE, snapshot->z_enable);
  if (snapshot->has_z_write)
    SetOneRenderState(self, D3DRS_ZWRITEENABLE, snapshot->z_write);
  if (snapshot->has_z_func)
    SetOneRenderState(self, D3DRS_ZFUNC, snapshot->z_func);
  if (snapshot->has_alpha_test)
    SetOneRenderState(self, D3DRS_ALPHATESTENABLE, snapshot->alpha_test);
  if (snapshot->has_alpha_ref)
    SetOneRenderState(self, D3DRS_ALPHAREF, snapshot->alpha_ref);
  if (snapshot->has_alpha_func)
    SetOneRenderState(self, D3DRS_ALPHAFUNC, snapshot->alpha_func);
  if (snapshot->has_alpha_blend)
    SetOneRenderState(self, D3DRS_ALPHABLENDENABLE, snapshot->alpha_blend);
  if (snapshot->has_src_blend)
    SetOneRenderState(self, D3DRS_SRCBLEND, snapshot->src_blend);
  if (snapshot->has_dest_blend)
    SetOneRenderState(self, D3DRS_DESTBLEND, snapshot->dest_blend);
  if (snapshot->has_cull_mode)
    SetOneRenderState(self, D3DRS_CULLMODE, snapshot->cull_mode);
  if (snapshot->has_color_write)
    SetOneRenderState(self, D3DRS_COLORWRITEENABLE, snapshot->color_write);
  if (snapshot->has_shade_mode)
    SetOneRenderState(self, D3DRS_SHADEMODE, snapshot->shade_mode);
  if (snapshot->has_dither_enable)
    SetOneRenderState(self, D3DRS_DITHERENABLE, snapshot->dither_enable);
}

static void ForceModelDepthPrepassState(void* self, const D3D9StateSnapshot* snapshot, int cutout)
{
  if (snapshot->has_z_enable)
    SetOneRenderState(self, D3DRS_ZENABLE, 1);
  if (snapshot->has_z_write)
    SetOneRenderState(self, D3DRS_ZWRITEENABLE, 1);
  if (snapshot->has_z_func)
    SetOneRenderState(self, D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
  if (snapshot->has_color_write)
    SetOneRenderState(self, D3DRS_COLORWRITEENABLE, 0);
  if (cutout)
    ForceCutoutGeometryState(self, snapshot);
  else
    ForceModelAlphaTest(self, snapshot);
  if (g_model_gouraud_shading && snapshot->has_shade_mode)
    SetOneRenderState(self, D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
  if (g_model_dither && snapshot->has_dither_enable)
    SetOneRenderState(self, D3DRS_DITHERENABLE, 1);
}

static void ForceModelColorPassAfterPrepassState(void* self, const D3D9StateSnapshot* snapshot,
                                                 int cutout, const ZfixCallsiteProfile* profile)
{
  if (snapshot->has_z_enable)
    SetOneRenderState(self, D3DRS_ZENABLE, 1);
  if (snapshot->has_z_write)
    SetOneRenderState(self, D3DRS_ZWRITEENABLE, (DWORD)g_model_color_pass_z_write);
  if (snapshot->has_z_func)
    SetOneRenderState(self, D3DRS_ZFUNC,
                      ZfixProfileColorZFunc(profile, (DWORD)g_model_color_pass_z_func));
  if (snapshot->has_color_write)
    SetOneRenderState(self, D3DRS_COLORWRITEENABLE,
                      snapshot->color_write ? snapshot->color_write : D3D_COLORWRITE_ALL);
  if (cutout)
    ForceCutoutGeometryState(self, snapshot);
  else
    ForceModelAlphaTest(self, snapshot);
  if (g_model_gouraud_shading && snapshot->has_shade_mode)
    SetOneRenderState(self, D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
  if (g_model_dither && snapshot->has_dither_enable)
    SetOneRenderState(self, D3DRS_DITHERENABLE, 1);
}

static void CountModelDepthPrepass(HRESULT hr)
{
  if (SUCCEEDED(hr))
    LogCounterIncrement(&g_model_depth_prepass_draws);
  else
    LogCounterIncrement(&g_model_depth_prepass_failures);
}

static HRESULT DrawPrimitiveUPWithModelDepth(void* self, D3D9DrawPrimitiveUPProc orig, DWORD primitive_type,
                                             UINT primitive_count, const void* vertex_data, UINT vertex_stride,
                                             DWORD vertex_count, DWORD caller)
{
  const ZfixCallsiteProfile* profile = NULL;
  D3D9StateSnapshot snapshot;
  CaptureState(self, &snapshot);

  const SIZE_T bytes = (SIZE_T)vertex_stride * (SIZE_T)vertex_count;
  D3D9TLVERTEX* copy = (D3D9TLVERTEX*)HeapAlloc(GetProcessHeap(), 0, bytes);
  if (!copy)
  {
    ForceModelState(self, &snapshot);
    HRESULT hr = orig(self, primitive_type, primitive_count, vertex_data, vertex_stride);
    RestoreState(self, &snapshot);
    return hr;
  }

  memcpy(copy, vertex_data, bytes);
  DrawBounds adjusted_bounds;
  ComputeDrawBounds(copy, vertex_count, &adjusted_bounds);
  profile = FindModelCallsiteProfileForDraw(caller, &adjusted_bounds);
  if (profile)
    LogCounterIncrement(&g_callsite_profile_hits);
  const DWORD adaptive_changed =
    ApplyAdaptiveDepthConflictResolver(copy, vertex_count, &adjusted_bounds,
                                       profile, caller, "opaque-dpup");
  if (adaptive_changed)
    ComputeDrawBounds(copy, vertex_count, &adjusted_bounds);
  if (g_model_depth_prepass)
  {
    ForceModelDepthPrepassState(self, &snapshot, 0);
    const HRESULT prepass_hr = orig(self, primitive_type, primitive_count, copy, vertex_stride);
    CountModelDepthPrepass(prepass_hr);
    if (SUCCEEDED(prepass_hr))
      ForceModelColorPassAfterPrepassState(self, &snapshot, 0, profile);
    else
      ForceModelState(self, &snapshot);
  }
  else
  {
    ForceModelState(self, &snapshot);
  }
  HRESULT hr = orig(self, primitive_type, primitive_count, copy, vertex_stride);
  RestoreState(self, &snapshot);
  HeapFree(GetProcessHeap(), 0, copy);
  return hr;
}

static HRESULT DrawPrimitiveUPWithTransparentModelDepth(void* self, D3D9DrawPrimitiveUPProc orig,
                                                        DWORD primitive_type, UINT primitive_count,
                                                        const void* vertex_data, UINT vertex_stride,
                                                        DWORD vertex_count, AlphaModelClass alpha_class,
                                                        DWORD caller)
{
  const ZfixCallsiteProfile* profile = NULL;
  D3D9StateSnapshot snapshot;
  CaptureState(self, &snapshot);
  const int cutout = alpha_class == ALPHA_MODEL_CUTOUT;
  if (!cutout)
    ForceTransparentModelState(self, &snapshot, 0);

  const SIZE_T bytes = (SIZE_T)vertex_stride * (SIZE_T)vertex_count;
  D3D9TLVERTEX* copy = NULL;
  const void* draw_vertices = vertex_data;

  if (cutout)
  {
    copy = (D3D9TLVERTEX*)HeapAlloc(GetProcessHeap(), 0, bytes);
    if (copy)
    {
      memcpy(copy, vertex_data, bytes);
      DrawBounds adjusted_bounds;
      ComputeDrawBounds(copy, vertex_count, &adjusted_bounds);
      profile = FindModelCallsiteProfileForDraw(caller, &adjusted_bounds);
      if (profile)
        LogCounterIncrement(&g_callsite_profile_hits);
      const DWORD adaptive_changed =
        ApplyAdaptiveDepthConflictResolver(copy, vertex_count, &adjusted_bounds,
                                           profile, caller, "cutout-dpup");
      if (adaptive_changed)
        ComputeDrawBounds(copy, vertex_count, &adjusted_bounds);
      draw_vertices = copy;
    }
  }
  if (cutout)
  {
    if (g_model_depth_prepass && copy)
    {
      ForceModelDepthPrepassState(self, &snapshot, 1);
      const HRESULT prepass_hr = orig(self, primitive_type, primitive_count, draw_vertices, vertex_stride);
      CountModelDepthPrepass(prepass_hr);
      if (SUCCEEDED(prepass_hr))
        ForceModelColorPassAfterPrepassState(self, &snapshot, 1, profile);
      else
        ForceModelState(self, &snapshot);
    }
    else
    {
      ForceModelState(self, &snapshot);
      ForceCutoutGeometryState(self, &snapshot);
    }
  }

  HRESULT hr = orig(self, primitive_type, primitive_count, draw_vertices, vertex_stride);
  RestoreState(self, &snapshot);
  if (copy)
    HeapFree(GetProcessHeap(), 0, copy);
  return hr;
}

static HRESULT DrawIndexedPrimitiveUPWithModelDepth(void* self, D3D9DrawIndexedPrimitiveUPProc orig,
                                                    DWORD primitive_type, UINT min_vertex_index,
                                                    UINT num_vertices, UINT primitive_count,
                                                    const void* index_data, DWORD index_format,
                                                    const void* vertex_data, UINT vertex_stride,
                                                    DWORD caller)
{
  const ZfixCallsiteProfile* profile = NULL;
  D3D9StateSnapshot snapshot;
  CaptureState(self, &snapshot);
  const DWORD index_count = VertexCountForPrimitive(primitive_type, primitive_count);

  const SIZE_T bytes = (SIZE_T)vertex_stride * (SIZE_T)num_vertices;
  D3D9TLVERTEX* copy = (D3D9TLVERTEX*)HeapAlloc(GetProcessHeap(), 0, bytes);
  if (!copy)
  {
    ForceModelState(self, &snapshot);
    HRESULT hr = orig(self, primitive_type, min_vertex_index, num_vertices, primitive_count,
                      index_data, index_format, vertex_data, vertex_stride);
    RestoreState(self, &snapshot);
    return hr;
  }

  memcpy(copy, vertex_data, bytes);
  DrawBounds adjusted_bounds;
  if (!ComputeIndexedUPDrawBounds(copy, num_vertices, index_data, index_format,
                                  index_count, &adjusted_bounds))
    ComputeDrawBounds(copy, num_vertices, &adjusted_bounds);
  profile = FindModelCallsiteProfileForDraw(caller, &adjusted_bounds);
  if (profile)
    LogCounterIncrement(&g_callsite_profile_hits);
  const DWORD adaptive_changed =
    ApplyAdaptiveDepthConflictResolver(copy, num_vertices, &adjusted_bounds,
                                       profile, caller, "opaque-dipup");
  if (adaptive_changed)
  {
    if (!ComputeIndexedUPDrawBounds(copy, num_vertices, index_data, index_format,
                                    index_count, &adjusted_bounds))
      ComputeDrawBounds(copy, num_vertices, &adjusted_bounds);
  }
  if (g_model_depth_prepass)
  {
    ForceModelDepthPrepassState(self, &snapshot, 0);
    const HRESULT prepass_hr =
      orig(self, primitive_type, min_vertex_index, num_vertices, primitive_count,
           index_data, index_format, copy, vertex_stride);
    CountModelDepthPrepass(prepass_hr);
    if (SUCCEEDED(prepass_hr))
      ForceModelColorPassAfterPrepassState(self, &snapshot, 0, profile);
    else
      ForceModelState(self, &snapshot);
  }
  else
  {
    ForceModelState(self, &snapshot);
  }
  HRESULT hr = orig(self, primitive_type, min_vertex_index, num_vertices, primitive_count,
                    index_data, index_format, copy, vertex_stride);
  RestoreState(self, &snapshot);
  HeapFree(GetProcessHeap(), 0, copy);
  return hr;
}

static HRESULT DrawIndexedPrimitiveUPWithTransparentModelDepth(void* self, D3D9DrawIndexedPrimitiveUPProc orig,
                                                              DWORD primitive_type, UINT min_vertex_index,
                                                              UINT num_vertices, UINT primitive_count,
                                                              const void* index_data, DWORD index_format,
                                                              const void* vertex_data, UINT vertex_stride,
                                                              AlphaModelClass alpha_class, DWORD caller)
{
  const ZfixCallsiteProfile* profile = NULL;
  D3D9StateSnapshot snapshot;
  CaptureState(self, &snapshot);
  const int cutout = alpha_class == ALPHA_MODEL_CUTOUT;
  const DWORD index_count = VertexCountForPrimitive(primitive_type, primitive_count);
  if (!cutout)
    ForceTransparentModelState(self, &snapshot, 0);

  const SIZE_T bytes = (SIZE_T)vertex_stride * (SIZE_T)num_vertices;
  D3D9TLVERTEX* copy = NULL;
  const void* draw_vertices = vertex_data;

  if (cutout)
  {
    copy = (D3D9TLVERTEX*)HeapAlloc(GetProcessHeap(), 0, bytes);
    if (copy)
    {
      memcpy(copy, vertex_data, bytes);
      DrawBounds adjusted_bounds;
      if (!ComputeIndexedUPDrawBounds(copy, num_vertices, index_data, index_format,
                                      index_count, &adjusted_bounds))
        ComputeDrawBounds(copy, num_vertices, &adjusted_bounds);
      profile = FindModelCallsiteProfileForDraw(caller, &adjusted_bounds);
      if (profile)
        LogCounterIncrement(&g_callsite_profile_hits);
      const DWORD adaptive_changed =
        ApplyAdaptiveDepthConflictResolver(copy, num_vertices, &adjusted_bounds,
                                           profile, caller, "cutout-dipup");
      if (adaptive_changed)
      {
        if (!ComputeIndexedUPDrawBounds(copy, num_vertices, index_data, index_format,
                                        index_count, &adjusted_bounds))
          ComputeDrawBounds(copy, num_vertices, &adjusted_bounds);
      }
      draw_vertices = copy;
    }
  }
  if (cutout)
  {
    if (g_model_depth_prepass && copy)
    {
      ForceModelDepthPrepassState(self, &snapshot, 1);
      const HRESULT prepass_hr =
        orig(self, primitive_type, min_vertex_index, num_vertices, primitive_count,
             index_data, index_format, draw_vertices, vertex_stride);
      CountModelDepthPrepass(prepass_hr);
      if (SUCCEEDED(prepass_hr))
        ForceModelColorPassAfterPrepassState(self, &snapshot, 1, profile);
      else
        ForceModelState(self, &snapshot);
    }
    else
    {
      ForceModelState(self, &snapshot);
      ForceCutoutGeometryState(self, &snapshot);
    }
  }

  HRESULT hr = orig(self, primitive_type, min_vertex_index, num_vertices, primitive_count,
                    index_data, index_format, draw_vertices, vertex_stride);
  RestoreState(self, &snapshot);
  if (copy)
    HeapFree(GetProcessHeap(), 0, copy);
  return hr;
}

static HRESULT STDMETHODCALLTYPE Hook_D3D9_DrawPrimitiveUP(void* self, DWORD primitive_type, UINT primitive_count,
                                                          const void* vertex_data, UINT vertex_stride)
{
  LogCounterIncrement(&g_dpup_total);
  D3D9DrawPrimitiveUPProc orig = (D3D9DrawPrimitiveUPProc)GetOriginal(*(void***)self, 83);
  if (!orig)
    return D3D_OK;
  const DWORD caller = (DWORD)(uintptr_t)_ReturnAddress();

  if (!g_enabled || !vertex_data || vertex_stride != sizeof(D3D9TLVERTEX))
  {
    if (vertex_stride != sizeof(D3D9TLVERTEX))
      LogCounterIncrement(&g_dpup_stride_rejected);
    return orig(self, primitive_type, primitive_count, vertex_data, vertex_stride);
  }

  const DWORD vertex_count = VertexCountForPrimitive(primitive_type, primitive_count);
  if (vertex_count == 0 || vertex_count > 4096)
  {
    LogCounterIncrement(&g_dpup_other_rejected);
    return orig(self, primitive_type, primitive_count, vertex_data, vertex_stride);
  }

  DrawBounds bounds;
  DWORD alpha_blend = 0;
  const int alpha_blend_enabled =
    CaptureRenderState(self, D3DRS_ALPHABLENDENABLE, &alpha_blend) && alpha_blend != 0;
  DWORD alpha_test = 0;
  const int alpha_test_enabled =
    CaptureRenderState(self, D3DRS_ALPHATESTENABLE, &alpha_test) && alpha_test != 0;
  const char* reason = NULL;
  if (!IsModelDepthDraw((const D3D9TLVERTEX*)vertex_data, primitive_type, vertex_count, primitive_count,
                        alpha_blend_enabled, &bounds, &reason))
  {
    DrawBounds transparent_bounds;
    int min_alpha = 255;
    const char* transparent_reason = NULL;
    if (reason && strcmp(reason, "alpha") == 0 &&
        IsTransparentModelDepthDraw((const D3D9TLVERTEX*)vertex_data, primitive_type, vertex_count,
                                    primitive_count, &transparent_bounds, &min_alpha,
                                    &transparent_reason))
    {
      (void)transparent_reason;
      const AlphaModelClass alpha_class =
        ClassifyAlphaModel((const D3D9TLVERTEX*)vertex_data, vertex_count,
                           alpha_blend_enabled, alpha_test_enabled, min_alpha);
      if (alpha_class == ALPHA_MODEL_CUTOUT)
        LogCounterIncrement(&g_dpup_cutout_accepted);
      else
        LogCounterIncrement(&g_dpup_transparent_accepted);
      return DrawPrimitiveUPWithTransparentModelDepth(self, orig, primitive_type, primitive_count,
                                                     vertex_data, vertex_stride, vertex_count,
                                                     alpha_class, caller);
    }

    if (reason && strcmp(reason, "alpha") == 0)
      LogCounterIncrement(&g_dpup_alpha_rejected);
    else if (reason && strcmp(reason, "axis") == 0)
      LogCounterIncrement(&g_dpup_axis_rejected);
    else if (reason && strcmp(reason, "rhw") == 0)
      LogCounterIncrement(&g_dpup_rhw_rejected);
    else
      LogCounterIncrement(&g_dpup_other_rejected);
    return orig(self, primitive_type, primitive_count, vertex_data, vertex_stride);
  }

  LogCounterIncrement(&g_dpup_accepted);
  return DrawPrimitiveUPWithModelDepth(self, orig, primitive_type, primitive_count, vertex_data,
                                       vertex_stride, vertex_count, caller);
}

static HRESULT STDMETHODCALLTYPE Hook_D3D9_SetRenderState(void* self, DWORD state, DWORD value)
{
  LogCounterIncrement(&g_set_render_state_calls);
  D3D9SetRenderStateProc orig = (D3D9SetRenderStateProc)GetOriginal(*(void***)self, 57);
  return orig ? orig(self, state, value) : D3D_OK;
}

static HRESULT STDMETHODCALLTYPE Hook_D3D9_SetTexture(void* self, DWORD stage, void* texture)
{
  LogCounterIncrement(&g_set_texture_calls);
  D3D9SetTextureProc orig = (D3D9SetTextureProc)GetOriginal(*(void***)self, 65);
  if (stage == 0)
  {
    if (g_current_texture0 != texture)
    {
      g_current_texture0 = texture;
      UpdateCurrentTexture0Desc(texture);
      LogCounterIncrement(&g_set_texture0_changes);
    }
  }
  return orig ? orig(self, stage, texture) : D3D_OK;
}

static HRESULT STDMETHODCALLTYPE Hook_D3D9_SetFVF(void* self, DWORD fvf)
{
  LogCounterIncrement(&g_set_fvf_calls);
  D3D9SetFVFProc orig = (D3D9SetFVFProc)GetOriginal(*(void***)self, 89);
  if (g_current_fvf != fvf)
    g_current_fvf = fvf;
  return orig ? orig(self, fvf) : D3D_OK;
}

static HRESULT STDMETHODCALLTYPE Hook_D3D9_DrawPrimitive(void* self, DWORD primitive_type, UINT start_vertex,
                                                        UINT primitive_count)
{
  LogCounterIncrement(&g_dp_calls);
  D3D9DrawPrimitiveProc orig = (D3D9DrawPrimitiveProc)GetOriginal(*(void***)self, 81);
  return orig ? orig(self, primitive_type, start_vertex, primitive_count) : D3D_OK;
}

static HRESULT STDMETHODCALLTYPE Hook_D3D9_DrawIndexedPrimitive(void* self, DWORD primitive_type,
                                                               INT base_vertex_index, UINT min_vertex_index,
                                                               UINT num_vertices, UINT start_index,
                                                               UINT primitive_count)
{
  LogCounterIncrement(&g_dip_calls);
  D3D9DrawIndexedPrimitiveProc orig = (D3D9DrawIndexedPrimitiveProc)GetOriginal(*(void***)self, 82);
  return orig ? orig(self, primitive_type, base_vertex_index, min_vertex_index, num_vertices,
                     start_index, primitive_count) : D3D_OK;
}

static HRESULT STDMETHODCALLTYPE Hook_D3D9_DrawIndexedPrimitiveUP(void* self, DWORD primitive_type,
                                                                 UINT min_vertex_index, UINT num_vertices,
                                                                 UINT primitive_count, const void* index_data,
                                                                 DWORD index_format, const void* vertex_data,
                                                                 UINT vertex_stride)
{
  LogCounterIncrement(&g_dipup_total);
  D3D9DrawIndexedPrimitiveUPProc orig =
    (D3D9DrawIndexedPrimitiveUPProc)GetOriginal(*(void***)self, 84);
  if (!orig)
    return D3D_OK;
  const DWORD caller = (DWORD)(uintptr_t)_ReturnAddress();

  const DWORD index_count = VertexCountForPrimitive(primitive_type, primitive_count);
  if (!g_enabled || !vertex_data || !index_data || vertex_stride != sizeof(D3D9TLVERTEX) ||
      (index_format != D3DFMT_INDEX16 && index_format != D3DFMT_INDEX32) ||
      num_vertices == 0 || num_vertices > 4096 || index_count == 0 || index_count > 8192)
  {
    LogCounterIncrement(&g_dipup_rejected);
    return orig(self, primitive_type, min_vertex_index, num_vertices, primitive_count,
                index_data, index_format, vertex_data, vertex_stride);
  }

  const D3D9TLVERTEX* vertices = (const D3D9TLVERTEX*)vertex_data;
  D3D9TLVERTEX* indexed = BuildIndexedVertexList(vertices, num_vertices, index_data,
                                                 index_format, index_count);
  if (!indexed)
  {
    LogCounterIncrement(&g_dipup_rejected);
    return orig(self, primitive_type, min_vertex_index, num_vertices, primitive_count,
                index_data, index_format, vertex_data, vertex_stride);
  }

  DrawBounds bounds;
  DWORD alpha_blend = 0;
  const int alpha_blend_enabled =
    CaptureRenderState(self, D3DRS_ALPHABLENDENABLE, &alpha_blend) && alpha_blend != 0;
  DWORD alpha_test = 0;
  const int alpha_test_enabled =
    CaptureRenderState(self, D3DRS_ALPHATESTENABLE, &alpha_test) && alpha_test != 0;
  const char* reason = NULL;
  if (!IsModelDepthDraw(indexed, primitive_type, index_count, primitive_count,
                        alpha_blend_enabled, &bounds, &reason))
  {
    DrawBounds transparent_bounds;
    int min_alpha = 255;
    const char* transparent_reason = NULL;
    if (reason && strcmp(reason, "alpha") == 0 &&
        IsTransparentModelDepthDraw(indexed, primitive_type, index_count, primitive_count,
                                    &transparent_bounds, &min_alpha, &transparent_reason))
    {
      (void)transparent_reason;
      const AlphaModelClass alpha_class =
        ClassifyAlphaModel(indexed, index_count, alpha_blend_enabled,
                           alpha_test_enabled, min_alpha);
      if (alpha_class == ALPHA_MODEL_CUTOUT)
        LogCounterIncrement(&g_dipup_cutout_accepted);
      else
        LogCounterIncrement(&g_dpup_transparent_accepted);
      HeapFree(GetProcessHeap(), 0, indexed);
      return DrawIndexedPrimitiveUPWithTransparentModelDepth(self, orig, primitive_type,
                                                            min_vertex_index, num_vertices,
                                                            primitive_count, index_data,
                                                            index_format, vertex_data,
                                                            vertex_stride, alpha_class, caller);
    }
    LogCounterIncrement(&g_dipup_rejected);
    HeapFree(GetProcessHeap(), 0, indexed);
    return orig(self, primitive_type, min_vertex_index, num_vertices, primitive_count,
                index_data, index_format, vertex_data, vertex_stride);
  }

  LogCounterIncrement(&g_dipup_accepted);
  HeapFree(GetProcessHeap(), 0, indexed);
  return DrawIndexedPrimitiveUPWithModelDepth(self, orig, primitive_type, min_vertex_index,
                                             num_vertices, primitive_count, index_data,
                                             index_format, vertex_data, vertex_stride, caller);
}

static void ClearDepthBufferForScene(void* self)
{
  if (!g_clear_depth_each_scene || !self)
    return;

  D3D9ClearProc clear = (D3D9ClearProc)GetVTableSlot(self, 43);
  if (!clear)
    return;

  const HRESULT hr = clear(self, 0, NULL, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
  if (SUCCEEDED(hr))
  {
    const LONG cleared = LogCounterIncrement(&g_depth_clear_count);
    if (cleared <= 16)
      LogLine("depth-clear #%ld ok", cleared);
  }
  else
  {
    const LONG failed = LogCounterIncrement(&g_depth_clear_failures);
    if (failed <= 16)
      LogLine("depth-clear fail #%ld hr=0x%08X", failed, (unsigned)hr);
  }
}

static HRESULT STDMETHODCALLTYPE Hook_D3D9_BeginScene(void* self)
{
  const int first_scene = !g_scene_open;
  if (first_scene)
  {
    g_scene_open = 1;
    EnsureOwnedDepthSurface(self, "begin");
  }
  D3D9BeginSceneProc orig = (D3D9BeginSceneProc)GetOriginal(*(void***)self, 41);
  const HRESULT hr = orig ? orig(self) : D3D_OK;
  if (first_scene && SUCCEEDED(hr))
  {
    LogCurrentDepthSurface(self, "clear");
    ClearDepthBufferForScene(self);
  }
  return hr;
}

static HRESULT STDMETHODCALLTYPE Hook_D3D9_EndScene(void* self)
{
  if (g_log_enabled)
  {
    const LONG frame = LogCounterIncrement(&g_frame_counter);
    if (frame <= g_initial_frame_summaries ||
        (g_frame_summary_interval > 0 && (frame % g_frame_summary_interval) == 0))
    {
      LogLine("frame=%ld dp=%ld dip=%ld dpup=%ld accepted=%ld transparent=%ld cutout=%ld/%ld "
              "dipup=%ld dipupAccepted=%ld dipupRejected=%ld depthPrepass=%ld/%ld depthClear=%ld depthFail=%ld "
              "profiles=%ld adaptive=%ld/%ld "
              "stride=%ld alpha=%ld axis=%ld rhw=%ld other=%ld "
              "setTex=%ld tex0Changes=%ld setFVF=%ld setRS=%ld curTex0=%p wh=%ux%u curFVF=0x%lX "
              "ownedDepthCreate=%ld ownedDepthSet=%ld ownedDepthFail=%ld",
              frame, g_dp_calls, g_dip_calls, g_dpup_total, g_dpup_accepted,
              g_dpup_transparent_accepted, g_dpup_cutout_accepted, g_dipup_cutout_accepted,
              g_dipup_total, g_dipup_accepted,
              g_dipup_rejected, g_model_depth_prepass_draws, g_model_depth_prepass_failures,
              g_depth_clear_count, g_depth_clear_failures,
              g_callsite_profile_hits, g_adaptive_depth_draws, g_adaptive_depth_vertices,
              g_dpup_stride_rejected, g_dpup_alpha_rejected, g_dpup_axis_rejected,
              g_dpup_rhw_rejected, g_dpup_other_rejected,
              g_set_texture_calls, g_set_texture0_changes, g_set_fvf_calls,
              g_set_render_state_calls, g_current_texture0,
              g_current_texture0_width, g_current_texture0_height, g_current_fvf,
              g_owned_depth_creates, g_owned_depth_sets, g_owned_depth_failures);
    }
  }
  g_scene_open = 0;
  D3D9EndSceneProc orig = (D3D9EndSceneProc)GetOriginal(*(void***)self, 42);
  return orig ? orig(self) : D3D_OK;
}

static HRESULT STDMETHODCALLTYPE Hook_D3D9_Reset(void* self, void* presentation_parameters)
{
  ReleaseOwnedDepthSurface();

  D3D9ResetProc orig = (D3D9ResetProc)GetOriginal(*(void***)self, 16);
  HRESULT hr = orig ? orig(self, presentation_parameters) : E_FAIL;
  LogLine("Reset hr=0x%08X device=%p", (unsigned)hr, self);
  if (SUCCEEDED(hr))
  {
    PatchD3D9Device(self);
    EnsureOwnedDepthSurface(self, "reset");
  }
  return hr;
}

static HRESULT TryCreateDeviceWithDepth(void* self, D3D9CreateDeviceProc orig, UINT adapter, DWORD device_type,
                                        HWND focus, DWORD behavior_flags,
                                        D3DPRESENT_PARAMETERS_COMPAT* original_pp,
                                        void** returned_device, DWORD depth_format)
{
  D3DPRESENT_PARAMETERS_COMPAT pp = *original_pp;
  pp.EnableAutoDepthStencil = TRUE;
  pp.AutoDepthStencilFormat = depth_format;
  return orig(self, adapter, device_type, focus, behavior_flags, &pp, returned_device);
}

static HRESULT STDMETHODCALLTYPE Hook_D3D9_CreateDevice(void* self, UINT adapter, DWORD device_type, HWND focus,
                                                       DWORD behavior_flags, void* presentation_parameters,
                                                       void** returned_device)
{
  D3D9CreateDeviceProc orig = (D3D9CreateDeviceProc)GetOriginal(*(void***)self, 16);
  if (!orig)
    return E_FAIL;

  D3DPRESENT_PARAMETERS_COMPAT* pp = (D3DPRESENT_PARAMETERS_COMPAT*)presentation_parameters;
  HRESULT hr = E_FAIL;
  if (g_upgrade_depth_stencil && pp && pp->EnableAutoDepthStencil)
  {
    const DWORD original_depth = pp->AutoDepthStencilFormat;
    const DWORD high_precision_formats[] = {
      D3DFMT_D32,
      D3DFMT_D24S8,
      D3DFMT_D24X8,
      D3DFMT_D32F_LOCKABLE,
    };
    const DWORD legacy_formats[] = {
      D3DFMT_D24S8,
      D3DFMT_D24X8,
      D3DFMT_D32,
    };
    const DWORD* formats = g_prefer_high_precision_depth ? high_precision_formats : legacy_formats;
    const DWORD format_count = g_prefer_high_precision_depth ?
                               (DWORD)ARRAYSIZE(high_precision_formats) :
                               (DWORD)ARRAYSIZE(legacy_formats);

    for (DWORD i = 0; i < format_count; i++)
    {
      const DWORD depth_format = formats[i];
      if (depth_format == original_depth && i != (format_count - 1))
        continue;

      hr = TryCreateDeviceWithDepth(self, orig, adapter, device_type, focus, behavior_flags,
                                    pp, returned_device, depth_format);
      if (SUCCEEDED(hr))
      {
        void* device = returned_device ? *returned_device : NULL;
        PatchD3D9Device(device);
        EnsureOwnedDepthSurface(device, "create-upgrade");
        LogLine("CreateDevice upgraded depth hr=0x%08X device=%p originalDepth=%lu usedDepth=%lu",
                (unsigned)hr, device, original_depth, depth_format);
        return hr;
      }
    }
  }

  hr = orig(self, adapter, device_type, focus, behavior_flags, presentation_parameters, returned_device);
  if (SUCCEEDED(hr) && returned_device && *returned_device)
  {
    PatchD3D9Device(*returned_device);
    EnsureOwnedDepthSurface(*returned_device, "create");
  }
  LogLine("CreateDevice hr=0x%08X device=%p autoDepth=%d depthFmt=%lu",
          (unsigned)hr, returned_device ? *returned_device : NULL,
          pp ? (int)pp->EnableAutoDepthStencil : -1,
          pp ? pp->AutoDepthStencilFormat : 0);
  return hr;
}

static void PatchD3D9Device(void* device)
{
  if (!device)
    return;
  PatchVTableSlot(device, 16, (void*)Hook_D3D9_Reset);
  PatchVTableSlot(device, 41, (void*)Hook_D3D9_BeginScene);
  PatchVTableSlot(device, 42, (void*)Hook_D3D9_EndScene);
  PatchVTableSlot(device, 57, (void*)Hook_D3D9_SetRenderState);
  PatchVTableSlot(device, 65, (void*)Hook_D3D9_SetTexture);
  PatchVTableSlot(device, 81, (void*)Hook_D3D9_DrawPrimitive);
  PatchVTableSlot(device, 82, (void*)Hook_D3D9_DrawIndexedPrimitive);
  PatchVTableSlot(device, 83, (void*)Hook_D3D9_DrawPrimitiveUP);
  PatchVTableSlot(device, 84, (void*)Hook_D3D9_DrawIndexedPrimitiveUP);
  PatchVTableSlot(device, 89, (void*)Hook_D3D9_SetFVF);
}

static void* WINAPI Hook_Direct3DCreate9(UINT sdk_version)
{
  if (!g_real_direct3d_create9)
  {
    HMODULE d3d9 = GetModuleHandleA("d3d9.dll");
    if (!d3d9)
      d3d9 = LoadLibraryA("d3d9.dll");
    g_real_direct3d_create9 = d3d9 ? (Direct3DCreate9Proc)GetProcAddress(d3d9, "Direct3DCreate9") : NULL;
  }

  if (!g_real_direct3d_create9)
    return NULL;

  void* d3d9 = g_real_direct3d_create9(sdk_version);
  if (d3d9)
    PatchVTableSlot(d3d9, 16, (void*)Hook_D3D9_CreateDevice);
  LogLine("Direct3DCreate9 sdk=%u result=%p", sdk_version, d3d9);
  return d3d9;
}

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
  (void)reserved;
  if (reason == DLL_PROCESS_ATTACH)
  {
    BuildLogPath(instance);
    g_log_enabled = ZfixLogEnabledByMarker(g_log_path);
    if (g_log_enabled)
      DeleteFileA(g_log_path);
    DisableThreadLibraryCalls(instance);
    LogLine("dc1_zfix loaded log=%d", g_log_enabled);
    LogLine("depth prepass=%d colorZWrite=%d colorZFunc=%d alphaTest=%d alphaRef=%lu alphaFunc=%lu",
            g_model_depth_prepass, g_model_color_pass_z_write,
            g_model_color_pass_z_func, g_model_alpha_test,
            g_model_alpha_ref, g_model_alpha_func);
    LogLine("profiles enabled=%d count=%lu adaptiveDepth=%d flatSpan=%.8f targetSpan=%.8f "
            "maxSpan=%.8f maxShift=%.8f area=%.1f..%.1f extent=%.1f..%.1f "
            "small=%.1f/%.1f strength=%.2f",
            g_callsite_profiles_enabled, (DWORD)ARRAYSIZE(g_callsite_profiles),
            g_adaptive_depth_conflict_resolver, g_adaptive_depth_flat_span,
            g_adaptive_depth_target_span, g_adaptive_depth_max_span,
            g_adaptive_depth_max_shift, g_adaptive_depth_min_area,
            g_adaptive_depth_max_area, g_adaptive_depth_min_extent,
            g_adaptive_depth_max_extent, g_adaptive_depth_small_min_area,
            g_adaptive_depth_small_min_extent, g_adaptive_depth_small_strength);
    LogLine("alpha classify cutoutMin=%d hardAlpha=%d/%d twoSided=%d alphaRef=%lu",
            g_cutout_alpha_min, g_cutout_hard_alpha_low_max,
            g_cutout_hard_alpha_high_min, g_cutout_model_two_sided,
            g_cutout_model_alpha_ref);
    PatchAllImports();
  }
  return TRUE;
}
