#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <intrin.h>

#if defined(_MSC_VER)
#pragma intrinsic(_ReturnAddress)
#endif

// DirectDraw and Direct3D 2 compatibility declarations.
#define D3D_OK 0
#define D3DVT_TLVERTEX 3
#define D3DPT_LINELIST 2
#define D3DPT_TRIANGLELIST 4
#define D3DPT_TRIANGLESTRIP 5
#define D3DPT_TRIANGLEFAN 6
#define D3DRENDERSTATE_ZENABLE 7
#define D3DRENDERSTATE_ALPHATESTENABLE 15
#define D3DRENDERSTATE_LASTPIXEL 16
#define D3DRENDERSTATE_ZWRITEENABLE 14
#define D3DRENDERSTATE_TEXTUREHANDLE 1
#define D3DRENDERSTATE_TEXTUREADDRESS 3
#define D3DRENDERSTATE_WRAPU 5
#define D3DRENDERSTATE_WRAPV 6
#define D3DRENDERSTATE_SRCBLEND 19
#define D3DRENDERSTATE_DESTBLEND 20
#define D3DRENDERSTATE_TEXTUREMAPBLEND 21
#define D3DRENDERSTATE_CULLMODE 22
#define D3DRENDERSTATE_ZFUNC 23
#define D3DRENDERSTATE_ALPHAREF 24
#define D3DRENDERSTATE_ALPHAFUNC 25
#define D3DRENDERSTATE_ALPHABLENDENABLE 27
#define D3DRENDERSTATE_ZBIAS 47
#define D3DRENDERSTATE_STIPPLEDALPHA 33
#define D3DRENDERSTATE_FOGENABLE 28
#define D3DRENDERSTATE_TEXTUREADDRESSU 44
#define D3DRENDERSTATE_TEXTUREADDRESSV 45
#define D3DCMP_EQUAL 3
#define D3DCMP_LESSEQUAL 4
#define D3DCMP_GREATER 5
#define D3DCMP_ALWAYS 8
#define D3DCLEAR_ZBUFFER 0x00000002l
#define DDSCAPS_ZBUFFER 0x00020000u
#define DDSCAPS_TEXTURE 0x00001000u
#define DDSD_CAPS 0x00000001u
#define DDSD_HEIGHT 0x00000002u
#define DDSD_WIDTH 0x00000004u
#define DDPF_ZBUFFER 0x00000400u
#define DDSD_ZBUFFERBITDEPTH 0x00000040u
#define DDSD_PIXELFORMAT 0x00001000u
#define D3DRENDERSTATE_TEXTUREPERSPECTIVE 4
#define D3DRENDERSTATE_SHADEMODE 9
#define D3DRENDERSTATE_TEXTUREMAG 17
#define D3DRENDERSTATE_TEXTUREMIN 18
#define D3DRENDERSTATE_DITHERENABLE 26
#define D3DRENDERSTATE_SPECULARENABLE 29
#define D3DRENDERSTATE_SUBPIXEL 31
#define D3DRENDERSTATE_SUBPIXELX 32
#define D3DSHADE_GOURAUD 2
#define D3DBLEND_ZERO 1
#define D3DBLEND_ONE 2
#define D3DCULL_NONE 1

#include "zfix_common.h"
#include "zfix_hooks.h"
#include "zfix_log.h"
#include "zfix_profiles.h"

typedef struct D3DTLVERTEX_COMPAT {
  float sx;
  float sy;
  float sz;
  float rhw;
  uint32_t color;
  uint32_t specular;
  float tu;
  float tv;
} D3DTLVERTEX_COMPAT;

typedef struct DDCOLORKEY_COMPAT {
  DWORD dwColorSpaceLowValue;
  DWORD dwColorSpaceHighValue;
} DDCOLORKEY_COMPAT;

typedef struct DDPIXELFORMAT_COMPAT {
  DWORD dwSize;
  DWORD dwFlags;
  DWORD dwFourCC;
  DWORD dwZBufferBitDepth;
  DWORD dwStencilBitDepth;
  DWORD dwZBitMask;
  DWORD dwStencilBitMask;
  DWORD dwRGBZBitMask;
} DDPIXELFORMAT_COMPAT;

typedef struct DDSCAPS_COMPAT {
  DWORD dwCaps;
} DDSCAPS_COMPAT;

typedef struct DDSURFACEDESC_COMPAT {
  DWORD dwSize;
  DWORD dwFlags;
  DWORD dwHeight;
  DWORD dwWidth;
  DWORD dwPitchOrLinearSize;
  DWORD dwBackBufferCount;
  DWORD dwZBufferBitDepth;
  DWORD dwAlphaBitDepth;
  DWORD dwReserved;
  void* lpSurface;
  DDCOLORKEY_COMPAT ddckCKDestOverlay;
  DDCOLORKEY_COMPAT ddckCKDestBlt;
  DDCOLORKEY_COMPAT ddckCKSrcOverlay;
  DDCOLORKEY_COMPAT ddckCKSrcBlt;
  DDPIXELFORMAT_COMPAT ddpfPixelFormat;
  DDSCAPS_COMPAT ddsCaps;
} DDSURFACEDESC_COMPAT;

typedef HRESULT(WINAPI* DirectDrawCreateProc)(GUID* lpGUID, void** lplpDD, void* pUnkOuter);
typedef HRESULT(STDMETHODCALLTYPE* DirectDrawCreateSurfaceProc)(void* self, DDSURFACEDESC_COMPAT* desc,
                                                                void** surface, void* outer);
typedef HRESULT(STDMETHODCALLTYPE* QueryInterfaceProc)(void* self, REFIID riid, void** ppvObj);
typedef HRESULT(STDMETHODCALLTYPE* D3D2CreateDeviceProc)(void* self, REFCLSID rclsid, void* surface, void** device);
typedef HRESULT(STDMETHODCALLTYPE* D3DDevice2BeginSceneProc)(void* self);
typedef HRESULT(STDMETHODCALLTYPE* D3DDevice2EndSceneProc)(void* self);
typedef HRESULT(STDMETHODCALLTYPE* D3DDevice2GetCurrentViewportProc)(void* self, void** viewport);
typedef HRESULT(STDMETHODCALLTYPE* D3DDevice2GetRenderStateProc)(void* self, DWORD state, DWORD* value);
typedef HRESULT(STDMETHODCALLTYPE* D3DDevice2SetRenderStateProc)(void* self, DWORD state, DWORD value);
typedef HRESULT(STDMETHODCALLTYPE* D3DDevice2DrawPrimitiveProc)(void* self, DWORD primitive_type, DWORD vertex_type,
                                                               void* vertices, DWORD vertex_count, DWORD flags);
typedef HRESULT(STDMETHODCALLTYPE* D3DDevice2DrawIndexedPrimitiveProc)(void* self, DWORD primitive_type,
                                                                       DWORD vertex_type, void* vertices,
                                                                       DWORD vertex_count, WORD* indices,
                                                                       DWORD index_count, DWORD flags);
typedef HRESULT(STDMETHODCALLTYPE* D3DTexture2GetHandleProc)(void* self, void* device, DWORD* handle);
typedef HRESULT(STDMETHODCALLTYPE* D3DViewport2ClearProc)(void* self, DWORD count, void* rects, DWORD flags);

static const GUID kIID_IDirectDraw2 =
  {0xB3A6F3E0, 0x2B43, 0x11CF, {0xA2, 0xDE, 0x00, 0xAA, 0x00, 0xB9, 0x33, 0x56}};
static const GUID kIID_IDirect3D2 =
  {0x6aae1ec1, 0x662a, 0x11d0, {0x88, 0x9d, 0x00, 0xaa, 0x00, 0xbb, 0xb7, 0x6a}};
static const GUID kIID_IDirect3DDevice2 =
  {0x93281501, 0x8cf8, 0x11d0, {0x89, 0xab, 0x00, 0xa0, 0xc9, 0x05, 0x41, 0x29}};
static const GUID kIID_IDirect3DTexture2 =
  {0x93281502, 0x8cf8, 0x11d0, {0x89, 0xab, 0x00, 0xa0, 0xc9, 0x05, 0x41, 0x29}};

// Runtime bookkeeping.
typedef struct ModuleAddressInfo {
  char module_name[80];
  DWORD module_offset;
} ModuleAddressInfo;

typedef struct DrawCallsiteStats {
  DWORD caller;
  ModuleAddressInfo address;
  volatile LONG seen;
  volatile LONG accepted_dp;
  volatile LONG accepted_dip;
  volatile LONG transparent_dp;
  volatile LONG transparent_dip;
  volatile LONG rejected;
  volatile LONG low_span;
  volatile LONG tiny_delta;
} DrawCallsiteStats;

typedef struct DepthStateSnapshot {
  DWORD z_enable;
  DWORD z_write;
  DWORD z_func;
  DWORD z_bias;
  DWORD texture_perspective;
  DWORD texture_mag;
  DWORD texture_min;
  DWORD alpha_test;
  DWORD alpha_ref;
  DWORD alpha_func;
  DWORD alpha_blend;
  DWORD cull_mode;
  DWORD shade_mode;
  DWORD dither_enable;
  DWORD subpixel;
  DWORD subpixel_x;
  int has_z_enable;
  int has_z_write;
  int has_z_func;
  int has_z_bias;
  int has_texture_perspective;
  int has_texture_mag;
  int has_texture_min;
  int has_alpha_test;
  int has_alpha_ref;
  int has_alpha_func;
  int has_alpha_blend;
  int has_cull_mode;
  int has_shade_mode;
  int has_dither_enable;
  int has_subpixel;
  int has_subpixel_x;
} DepthStateSnapshot;

#define MODEL_SORT_STATE_COUNT 24
#define TEXTURE_SURFACE_TRACE_CAPACITY 1024
#define TEXTURE_HANDLE_TRACE_CAPACITY 256
#define TEXTURE_BINDING_TRACE_CAPACITY 512

typedef struct BatchRenderState {
  DWORD value[MODEL_SORT_STATE_COUNT];
  int has[MODEL_SORT_STATE_COUNT];
} BatchRenderState;

typedef struct TextureHandleTrace {
  void* texture;
  void* surface;
  DWORD handle;
  DWORD width;
  DWORD height;
  DWORD caps;
  volatile LONG seen;
} TextureHandleTrace;

typedef struct TextureSurfaceTrace {
  void* surface;
  DWORD flags;
  DWORD caps;
  DWORD width;
  DWORD height;
  DWORD pitch;
  volatile LONG seen;
} TextureSurfaceTrace;

typedef struct TextureBindingTrace {
  void* texture;
  void* surface;
  volatile LONG seen;
} TextureBindingTrace;

static HookEntry g_hooks[128];
static DrawCallsiteStats g_draw_callsites[128];
static TextureHandleTrace g_texture_handles[TEXTURE_HANDLE_TRACE_CAPACITY];
static TextureSurfaceTrace g_texture_surfaces[TEXTURE_SURFACE_TRACE_CAPACITY];
static TextureBindingTrace g_texture_bindings[TEXTURE_BINDING_TRACE_CAPACITY];
static volatile LONG g_hook_count = 0;
static volatile LONG g_draw_callsite_count = 0;
static DirectDrawCreateProc g_real_direct_draw_create = NULL;
static HMODULE g_module = NULL;
static char g_game_dir[MAX_PATH];
static char g_log_path[MAX_PATH];
static int g_log_enabled = 0;
static volatile LONG g_draw_total = 0;
static volatile LONG g_draw_model_accepted = 0;
static volatile LONG g_draw_indexed_model_accepted = 0;
static volatile LONG g_draw_transparent_accepted = 0;
static volatile LONG g_draw_cutout_accepted = 0;
static volatile LONG g_draw_rejected = 0;
static volatile LONG g_zfight_sample_logged = 0;
static volatile LONG g_scene_counter = 0;
static volatile LONG g_model_depth_prepass_draws = 0;
static volatile LONG g_model_depth_prepass_failures = 0;
static volatile LONG g_callsite_profile_hits = 0;
static volatile LONG g_adaptive_depth_draws = 0;
static volatile LONG g_adaptive_depth_vertices = 0;
static volatile LONG g_adaptive_depth_logged = 0;
static volatile LONG g_flat_depth_reject_logged = 0;
static volatile LONG g_texture_surface_seen = 0;
static volatile LONG g_texture_surface_count = 0;
static volatile LONG g_texture_surface_logged = 0;
static volatile LONG g_texture_handle_seen = 0;
static volatile LONG g_texture_handle_count = 0;
static volatile LONG g_texture_handle_logged = 0;
static volatile LONG g_texture_qi_seen = 0;
static volatile LONG g_texture_binding_count = 0;
static volatile LONG g_texture_binding_logged = 0;
static volatile LONG g_re2_mask_overlay_skipped = 0;
static volatile LONG g_re2_mask_overlay_logged = 0;

static const int g_enabled = 1;
static const int g_diagnostics = 1;
static const int g_callsite_diagnostics = 1;
static const int g_precise_model_depth_pass = 1;
static const int g_force_z_enable = 1;
static const int g_force_z_write = 1;
static const int g_force_z_func = D3DCMP_LESSEQUAL;
static const int g_force_z_bias = 0;
static const int g_model_depth_prepass = 1;
static const int g_model_color_pass_z_write = 0;
static const int g_model_color_pass_z_func = D3DCMP_LESSEQUAL;
static const int g_restore_depth_state = 1;
static const int g_clear_depth_each_scene = 1;
static const int g_skip_axis_tile_draws = 1;
static const int g_precise_min_vertex_alpha = 250;
static const int g_single_triangle_dp_only = 1;
static const int g_allow_indexed_model_draws = 1;
static const int g_require_model_callsite = 1;
static const int g_clear_model_depth_before_2d = 1;
static const int g_transparent_model_z_test = 1;
static const int g_transparent_model_z_write = 0;
static const int g_transparent_model_z_func = D3DCMP_LESSEQUAL;
static const int g_transparent_model_z_bias = ZFIX_SKIP_STATE;
static const int g_cutout_model_disable_alpha_blend = 1;
static const int g_cutout_model_two_sided = 1;
static const int g_cutout_alpha_min = 224;
static const int g_cutout_hard_alpha_low_max = 16;
static const int g_cutout_hard_alpha_high_min = 240;
static const DWORD g_cutout_model_alpha_ref = 16u;
static const int g_model_texture_perspective = 0;
static const int g_model_alpha_test = 1;
static const DWORD g_model_alpha_ref = 8u;
static const DWORD g_model_alpha_func = D3DCMP_GREATER;
static const int g_model_gouraud_shading = 0;
static const int g_model_dither = 0;
static const int g_model_subpixel = 0;
static const int g_callsite_profiles_enabled = 1;
static const int g_adaptive_depth_conflict_resolver = 1;
static const int g_upgrade_zbuffer_format = 1;
static const int g_upgrade_zbuffer_pixel_format = 1;
static const int g_zbuffer_upgrade_variants = 1;
static const int g_preferred_zbuffer_depth = 32;
static const int g_fallback_zbuffer_depth = 24;
static const int g_texture_handle_trace = 1;
static const int g_re2_mask_overlay_guard = 1;

static const float g_max_screen_extent = 360.0f;
static const float g_max_screen_area = 60000.0f;
static const float g_min_model_rhw = 0.00002f;
static const float g_max_model_rhw = 0.01f;
static const float g_max_model_rhw_ratio = 8.0f;
static const float g_max_triangle_aspect = 80.0f;
static const float g_spike_long_extent = 260.0f;
static const float g_spike_thin_extent = 2.0f;
static const float g_min_depth_variance = 0.000001f;
static const float g_min_rhw_variance = 0.00000001f;
static const DWORD g_crow_profile_callsite = 0x0040EC01u;
static const float g_crow_profile_flat_boost_z_span = 0.000180f;
static const float g_crow_profile_flat_boost_target_span = 0.000940f;
static const float g_crow_profile_mid_boost_z_span = 0.000620f;
static const float g_crow_profile_mid_boost_target_span = 0.001020f;
static const int g_crow_profile_precision_snap = 1;
static const float g_crow_profile_center_snap = 0.000004f;
static const float g_crow_profile_span_snap = 0.000010f;
static const float g_zfight_tiny_span = 0.000080f;
static const float g_zfight_tiny_delta = 0.000030f;
static const float g_adaptive_depth_flat_span = 0.000160f;
static const float g_adaptive_depth_target_span = 0.000900f;
static const float g_adaptive_depth_max_span = 0.001400f;
static const float g_adaptive_depth_max_shift = 0.000420f;
static const float g_adaptive_depth_min_area = 8.0f;
static const float g_adaptive_depth_max_area = 420000.0f;
static const float g_adaptive_depth_min_extent = 2.0f;
static const float g_adaptive_depth_max_extent = 960.0f;
static const float g_adaptive_depth_normal_min_area = 28.0f;
static const float g_adaptive_depth_normal_min_extent = 5.0f;
static const float g_adaptive_depth_small_min_area = 4.0f;
static const float g_adaptive_depth_small_min_extent = 1.25f;
static const float g_adaptive_depth_small_strength = 0.70f;
static const float g_adaptive_depth_rhw_signal = 0.00000001f;
static const float g_adaptive_depth_axis_signal = 1.0f;
static const float g_re2_mask_overlay_min_extent = 4.0f;
static const float g_re2_mask_overlay_min_area = 48.0f;
static const float g_re2_mask_overlay_max_extent = 4096.0f;
static const float g_re2_mask_overlay_flat_z_span = 0.0020f;
static const float g_re2_mask_overlay_flat_rhw_span = 0.0200f;
static const float g_re2_mask_overlay_min_rhw = 0.5f;
static const float g_re2_mask_overlay_axis_epsilon = 0.75f;
static const DWORD g_re2_hires_mask_texture_min_side = 1024u;
static const DWORD g_re2_classic_mask_texture_min_side = 256u;
static const DWORD g_re2_classic_mask_texture_max_side = 512u;
static const DWORD g_model_callsite_min = 0x0040E000u;
static const DWORD g_model_callsite_max = 0x0040F800u;
static const DWORD g_re2_batched_model_callsite = 0x004080D8u;
static const DWORD g_batched_model_max_vertices = 768u;
static const float g_batched_model_max_screen_extent = 960.0f;
static const float g_batched_model_max_screen_area = 420000.0f;

static const ZfixCallsiteProfile g_callsite_profiles[] = {
  {
    "re2-crow-v2-micro-flat",
    0x0040EC01u,
    0x0040EC01u,
    ZFIX_DEPTH_PROFILE_AGGRESSIVE,
    D3DCMP_LESSEQUAL,
    0.000800f,
    0.000980f,
    0.000400f,
    24.0f,
    60000.0f,
    5.0f,
    520.0f,
    0.0f,
    0.000180f,
    24.0f,
    60000.0f,
    5.0f,
    520.0f,
    0x0000001Au,
    512u,
    512u,
    0.000300f,
    0.38f,
    0.000400f
  },
  {
    "re2-crow-v2-mid-detail",
    0x0040EC01u,
    0x0040EC01u,
    ZFIX_DEPTH_PROFILE_AGGRESSIVE,
    D3DCMP_LESSEQUAL,
    0.000940f,
    0.001080f,
    0.000440f,
    24.0f,
    60000.0f,
    5.0f,
    520.0f,
    0.000180f,
    0.000620f,
    24.0f,
    60000.0f,
    5.0f,
    520.0f,
    0x0000001Au,
    512u,
    512u,
    0.000260f,
    0.30f,
    0.000440f
  },
  {
    "re2-crow-v2-wide-depth",
    0x0040EC01u,
    0x0040EC01u,
    ZFIX_DEPTH_PROFILE_AGGRESSIVE,
    D3DCMP_LESSEQUAL,
    0.001000f,
    0.001160f,
    0.000420f,
    24.0f,
    60000.0f,
    5.0f,
    520.0f,
    0.000620f,
    0.001250f,
    24.0f,
    60000.0f,
    5.0f,
    520.0f,
    0x0000001Au,
    512u,
    512u,
    0.000300f,
    0.20f,
    0.000420f
  },
  {
    "re2-batched-model",
    0x004080D8u,
    0x004080D8u,
    ZFIX_DEPTH_PROFILE_NORMAL,
    D3DCMP_LESSEQUAL,
    0.000480f,
    0.000780f,
    0.000240f,
    28.0f,
    420000.0f,
    5.0f,
    960.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0u,
    0u,
    0u,
    0.0f,
    0.0f,
    0.0f
  },
  {
    "re2-model-range",
    0x0040E000u,
    0x0040F800u,
    ZFIX_DEPTH_PROFILE_NORMAL,
    D3DCMP_LESSEQUAL,
    0.000430f,
    0.000760f,
    0.000230f,
    28.0f,
    60000.0f,
    5.0f,
    520.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0u,
    0u,
    0u,
    0.0f,
    0.0f,
    0.0f
  }
};

static const DWORD k_model_sort_states[MODEL_SORT_STATE_COUNT] = {
  D3DRENDERSTATE_TEXTUREHANDLE,
  D3DRENDERSTATE_TEXTUREADDRESS,
  D3DRENDERSTATE_WRAPU,
  D3DRENDERSTATE_WRAPV,
  D3DRENDERSTATE_ALPHATESTENABLE,
  D3DRENDERSTATE_LASTPIXEL,
  D3DRENDERSTATE_TEXTUREMAG,
  D3DRENDERSTATE_TEXTUREMIN,
  D3DRENDERSTATE_SRCBLEND,
  D3DRENDERSTATE_DESTBLEND,
  D3DRENDERSTATE_TEXTUREMAPBLEND,
  D3DRENDERSTATE_CULLMODE,
  D3DRENDERSTATE_ALPHAREF,
  D3DRENDERSTATE_ALPHAFUNC,
  D3DRENDERSTATE_DITHERENABLE,
  D3DRENDERSTATE_ALPHABLENDENABLE,
  D3DRENDERSTATE_FOGENABLE,
  D3DRENDERSTATE_SPECULARENABLE,
  D3DRENDERSTATE_SUBPIXEL,
  D3DRENDERSTATE_SUBPIXELX,
  D3DRENDERSTATE_STIPPLEDALPHA,
  D3DRENDERSTATE_TEXTUREADDRESSU,
  D3DRENDERSTATE_TEXTUREADDRESSV,
  D3DRENDERSTATE_ZBIAS
};

static DWORD g_rs_z_enable = 0xFFFFFFFFu;
static DWORD g_rs_z_write = 0xFFFFFFFFu;
static DWORD g_rs_z_func = 0xFFFFFFFFu;
static DWORD g_rs_z_bias = 0xFFFFFFFFu;
static DWORD g_rs_alpha_blend = 0xFFFFFFFFu;
static DWORD g_render_state_cache[256];
static int g_model_depth_written_this_scene = 0;

// Forward declarations.
static void* GetOriginal(void** vtable, int slot);
static int PatchVTableSlot(void* obj, int slot, void* hook);
static void LogLine(const char* fmt, ...);
static TextureSurfaceTrace* FindTextureSurface(void* surface);
static TextureHandleTrace* FindTextureHandleTraceByHandle(DWORD handle);
static void PatchD3DDevice2(void* device);
static void ClearDepthBuffer(void* self);
static void ClearDepthBufferForScene(void* self);
static void ClearModelDepthBeforeKnown2D(void* self, DWORD caller, DWORD vertex_type,
                                         const void* vertices, DWORD vertex_count,
                                         const WORD* indices, DWORD index_count);
static HRESULT DrawPrimitiveWithModelDepth(void* self, D3DDevice2DrawPrimitiveProc orig, DWORD primitive_type,
                                           DWORD vertex_type, void* vertices, DWORD vertex_count, DWORD flags,
                                           DWORD caller, DWORD tris, const DrawBounds* bounds);
// Hook entry points.
static HRESULT STDMETHODCALLTYPE Hook_DD_CreateSurface(void* self, DDSURFACEDESC_COMPAT* desc,
                                                       void** surface, void* outer);
static HRESULT STDMETHODCALLTYPE Hook_QueryInterface(void* self, REFIID riid, void** ppvObj);
static HRESULT STDMETHODCALLTYPE Hook_D3D2_CreateDevice(void* self, REFCLSID rclsid, void* surface, void** device);
static HRESULT STDMETHODCALLTYPE Hook_D3DTexture2_GetHandle(void* self, void* device, DWORD* handle);

static void BuildLogPath(HINSTANCE instance)
{
  ZfixBuildLogPath(instance, g_game_dir, sizeof(g_game_dir),
                   g_log_path, sizeof(g_log_path), "re2_zfix.log");
}

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
  return ZfixLogCounterIncrement(g_log_enabled, counter);
}

static LONG LogCounterAdd(volatile LONG* counter, LONG value)
{
  return ZfixLogCounterAdd(g_log_enabled, counter, value);
}

static void ResolveModuleForAddress(DWORD address, ModuleAddressInfo* info)
{
  if (!info)
    return;
  memset(info, 0, sizeof(*info));
  info->module_offset = address;
  strncpy(info->module_name, "unknown", sizeof(info->module_name) - 1);

  HMODULE module = NULL;
  const DWORD flags = GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                      GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT;
  if (!address || !GetModuleHandleExA(flags, (LPCSTR)(uintptr_t)address, &module) || !module)
    return;

  char path[MAX_PATH];
  DWORD len = GetModuleFileNameA(module, path, sizeof(path));
  const char* base = (len > 0 && len < sizeof(path)) ? ZfixBaseNameFromPath(path) : "module";
  strncpy(info->module_name, base, sizeof(info->module_name) - 1);
  info->module_name[sizeof(info->module_name) - 1] = '\0';
  info->module_offset = address - (DWORD)(uintptr_t)module;
}

static DrawCallsiteStats* FindOrCreateDrawCallsite(DWORD caller)
{
  if (!g_log_enabled || !g_callsite_diagnostics || !caller)
    return NULL;

  static volatile LONG callsite_lock = 0;
  LONG count = g_draw_callsite_count;
  if (count > (LONG)ARRAYSIZE(g_draw_callsites))
    count = (LONG)ARRAYSIZE(g_draw_callsites);
  for (LONG i = 0; i < count; i++)
  {
    if (g_draw_callsites[i].caller == caller)
      return &g_draw_callsites[i];
  }

  ZfixAcquirePatchLock(&callsite_lock);
  count = g_draw_callsite_count;
  if (count < 0)
    count = 0;
  if (count > (LONG)ARRAYSIZE(g_draw_callsites))
    count = (LONG)ARRAYSIZE(g_draw_callsites);
  for (LONG i = 0; i < count; i++)
  {
    if (g_draw_callsites[i].caller == caller)
    {
      ZfixReleasePatchLock(&callsite_lock);
      return &g_draw_callsites[i];
    }
  }
  if (count >= (LONG)ARRAYSIZE(g_draw_callsites))
  {
    ZfixReleasePatchLock(&callsite_lock);
    return NULL;
  }

  DrawCallsiteStats* site = &g_draw_callsites[count];
  memset(site, 0, sizeof(*site));
  site->caller = caller;
  ResolveModuleForAddress(caller, &site->address);
  InterlockedExchange(&g_draw_callsite_count, count + 1);
  ZfixReleasePatchLock(&callsite_lock);
  return site;
}

static void TrackDrawCallsite(DWORD caller, const char* kind, int indexed, int transparent,
                              DWORD primitive_type, DWORD vertex_count, DWORD index_count,
                              DWORD tris, const char* reason, const DrawBounds* bounds)
{
  DrawCallsiteStats* site = FindOrCreateDrawCallsite(caller);
  if (!site)
    return;

  const LONG seen = InterlockedIncrement(&site->seen);
  if (kind && strcmp(kind, "accept") == 0)
  {
    if (transparent)
    {
      if (indexed)
        InterlockedIncrement(&site->transparent_dip);
      else
        InterlockedIncrement(&site->transparent_dp);
    }
    else if (indexed)
      InterlockedIncrement(&site->accepted_dip);
    else
      InterlockedIncrement(&site->accepted_dp);
  }
  else
    InterlockedIncrement(&site->rejected);

  if (bounds)
  {
    const float z_span = bounds->max_z - bounds->min_z;
    if (z_span <= g_zfight_tiny_span)
      InterlockedIncrement(&site->low_span);
  }

  if (seen == 1)
  {
    LogLine("callsite new %s caller=%s+0x%08lX raw=0x%08lX indexed=%d transparent=%d "
            "type=%lu verts=%lu indices=%lu tris=%lu reason=%s "
            "xy=[%.2f..%.2f %.2f..%.2f] z=[%.6f..%.6f] rhw=[%.8f..%.8f]",
            kind ? kind : "draw", site->address.module_name, site->address.module_offset,
            caller, indexed, transparent, primitive_type, vertex_count, index_count, tris,
            reason ? reason : "ok",
            bounds ? bounds->min_x : 0.0f, bounds ? bounds->max_x : 0.0f,
            bounds ? bounds->min_y : 0.0f, bounds ? bounds->max_y : 0.0f,
            bounds ? bounds->min_z : 0.0f, bounds ? bounds->max_z : 0.0f,
            bounds ? bounds->min_rhw : 0.0f, bounds ? bounds->max_rhw : 0.0f);
  }
}

static void MarkCallsiteTinyDelta(DWORD caller)
{
  DrawCallsiteStats* site = FindOrCreateDrawCallsite(caller);
  if (site)
    InterlockedIncrement(&site->tiny_delta);
}

static void LogZFightSample(DWORD caller, const char* kind, int indexed, DWORD primitive_type,
                            DWORD vertex_count, DWORD index_count, const DrawBounds* before,
                            const DrawBounds* after, DWORD adjusted_vertices)
{
  if (!g_log_enabled)
    return;
  if (!before || !after)
    return;

  const float before_span = before->max_z - before->min_z;
  const float after_span = after->max_z - after->min_z;
  const float depth_delta = after_span - before_span;
  const float abs_depth_delta = depth_delta < 0.0f ? -depth_delta : depth_delta;
  const int suspicious = before_span <= g_zfight_tiny_span ||
                         after_span <= g_zfight_tiny_span ||
                         abs_depth_delta <= g_zfight_tiny_delta;
  if (!suspicious)
    return;

  MarkCallsiteTinyDelta(caller);
  const LONG sample = InterlockedIncrement(&g_zfight_sample_logged);
  if (sample > 192)
    return;

  ModuleAddressInfo info;
  ResolveModuleForAddress(caller, &info);
  LogLine("zfight-sample #%ld %s caller=%s+0x%08lX indexed=%d type=%lu verts=%lu indices=%lu "
          "adjusted=%lu z %.6f..%.6f span=%.8f -> %.6f..%.6f span=%.8f "
          "rhw=[%.8f..%.8f] xy=[%.2f..%.2f %.2f..%.2f]",
          sample, kind ? kind : "draw", info.module_name, info.module_offset, indexed,
          primitive_type, vertex_count, index_count, adjusted_vertices,
          before->min_z, before->max_z, before_span,
          after->min_z, after->max_z, after_span,
          before->min_rhw, before->max_rhw,
          before->min_x, before->max_x, before->min_y, before->max_y);
}

static void LogDrawCallsiteSummary(const char* reason)
{
  if (!g_log_enabled)
    return;

  LONG count = g_draw_callsite_count;
  if (count > (LONG)ARRAYSIZE(g_draw_callsites))
    count = (LONG)ARRAYSIZE(g_draw_callsites);

  LogLine("summary %s callsites=%ld total=%ld acceptDP=%ld acceptDIP=%ld transparent=%ld cutout=%ld rejected=%ld",
          reason ? reason : "snapshot", count, g_draw_total, g_draw_model_accepted,
          g_draw_indexed_model_accepted, g_draw_transparent_accepted,
          g_draw_cutout_accepted, g_draw_rejected);
  LogLine("summary depth prepass=%d draws=%ld failures=%ld colorZWrite=%d colorZFunc=%d "
          "clearEachScene=%d modelDepthWritten=%d",
          g_model_depth_prepass, g_model_depth_prepass_draws, g_model_depth_prepass_failures,
          g_model_color_pass_z_write, g_model_color_pass_z_func,
          g_clear_depth_each_scene, g_model_depth_written_this_scene);
  LogLine("summary profiles enabled=%d count=%lu hits=%ld adaptive=%d draws=%ld changedVerts=%ld logged=%ld",
          g_callsite_profiles_enabled, (DWORD)ARRAYSIZE(g_callsite_profiles), g_callsite_profile_hits,
          g_adaptive_depth_conflict_resolver, g_adaptive_depth_draws,
          g_adaptive_depth_vertices, g_adaptive_depth_logged);
  LogLine("summary texture_trace enabled=%d surfaces=%ld surfaceSlots=%ld surfaceLogged=%ld "
          "textureQI=%ld bindings=%ld bindingLogged=%ld handleCalls=%ld handles=%ld handleLogged=%ld",
          g_texture_handle_trace, g_texture_surface_seen, g_texture_surface_count,
          g_texture_surface_logged, g_texture_qi_seen, g_texture_binding_count,
          g_texture_binding_logged, g_texture_handle_seen, g_texture_handle_count,
          g_texture_handle_logged);
  LogLine("summary re2_mask_overlay guard=%d skipped=%ld logged=%ld",
          g_re2_mask_overlay_guard, g_re2_mask_overlay_skipped,
          g_re2_mask_overlay_logged);
  for (LONG i = 0; i < count; i++)
  {
    DrawCallsiteStats* site = &g_draw_callsites[i];
    if (!site->caller || site->seen <= 0)
      continue;
    LogLine("summary callsite #%ld caller=%s+0x%08lX raw=0x%08lX seen=%ld "
            "acceptDP=%ld acceptDIP=%ld transparentDP=%ld transparentDIP=%ld reject=%ld "
            "lowSpan=%ld tinyDelta=%ld",
            i + 1, site->address.module_name, site->address.module_offset, site->caller,
            site->seen, site->accepted_dp, site->accepted_dip, site->transparent_dp,
            site->transparent_dip, site->rejected, site->low_span, site->tiny_delta);
  }
}

// Geometry classification helpers.
static void ComputeDrawBounds(const D3DTLVERTEX_COMPAT* vertices, DWORD count, DrawBounds* bounds)
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

static void ExpandBoundsWithVertex(DrawBounds* bounds, const D3DTLVERTEX_COMPAT* v, int* valid)
{
  if (!bounds || !v || !valid)
    return;

  if (!*valid)
  {
    bounds->min_x = bounds->max_x = v->sx;
    bounds->min_y = bounds->max_y = v->sy;
    bounds->min_z = bounds->max_z = v->sz;
    bounds->min_rhw = bounds->max_rhw = v->rhw;
    *valid = 1;
    return;
  }

  if (v->sx < bounds->min_x)
    bounds->min_x = v->sx;
  if (v->sx > bounds->max_x)
    bounds->max_x = v->sx;
  if (v->sy < bounds->min_y)
    bounds->min_y = v->sy;
  if (v->sy > bounds->max_y)
    bounds->max_y = v->sy;
  if (v->sz < bounds->min_z)
    bounds->min_z = v->sz;
  if (v->sz > bounds->max_z)
    bounds->max_z = v->sz;
  if (v->rhw < bounds->min_rhw)
    bounds->min_rhw = v->rhw;
  if (v->rhw > bounds->max_rhw)
    bounds->max_rhw = v->rhw;
}

static int ComputeIndexedDrawBounds(const D3DTLVERTEX_COMPAT* vertices, DWORD vertex_count,
                                    const WORD* indices, DWORD index_count, DrawBounds* bounds)
{
  memset(bounds, 0, sizeof(*bounds));
  if (!vertices || !indices || vertex_count == 0 || index_count == 0)
    return 0;

  int valid = 0;
  for (DWORD i = 0; i < index_count; i++)
  {
    const DWORD idx = (DWORD)indices[i];
    if (idx >= vertex_count)
      return 0;
    ExpandBoundsWithVertex(bounds, &vertices[idx], &valid);
  }

  if (!valid)
    return 0;

  bounds->width = bounds->max_x - bounds->min_x;
  bounds->height = bounds->max_y - bounds->min_y;
  bounds->area = bounds->width * bounds->height;
  return 1;
}

static int AxisHalfRect3(float x0, float y0, float x1, float y1, float x2, float y2, float min_size, float eps)
{
  float min_x = x0;
  float max_x = x0;
  float min_y = y0;
  float max_y = y0;
  const float xs[3] = {x0, x1, x2};
  const float ys[3] = {y0, y1, y2};

  for (DWORD i = 1; i < 3; i++)
  {
    if (xs[i] < min_x)
      min_x = xs[i];
    if (xs[i] > max_x)
      max_x = xs[i];
    if (ys[i] < min_y)
      min_y = ys[i];
    if (ys[i] > max_y)
      max_y = ys[i];
  }

  const float width = max_x - min_x;
  const float height = max_y - min_y;
  if (width < min_size || height < min_size)
    return 0;

  for (DWORD i = 0; i < 3; i++)
  {
    if (!NearF(xs[i], min_x, eps) && !NearF(xs[i], max_x, eps))
      return 0;
    if (!NearF(ys[i], min_y, eps) && !NearF(ys[i], max_y, eps))
      return 0;
  }

  const float tri_area2 = AbsF((x1 - x0) * (y2 - y0) - (x2 - x0) * (y1 - y0));
  const float rect_area = width * height;
  float area_eps = rect_area * 0.03f;
  if (area_eps < eps * 8.0f)
    area_eps = eps * 8.0f;
  return NearF(tri_area2, rect_area, area_eps);
}

static int IsLikelyAxisTileDraw(const D3DTLVERTEX_COMPAT* vertices, DWORD primitive_type, DWORD vertex_count,
                                DWORD tris)
{
  if (!g_skip_axis_tile_draws || !vertices || primitive_type != D3DPT_TRIANGLELIST || vertex_count != 3 || tris != 1)
    return 0;

  if (!AxisHalfRect3(vertices[0].sx, vertices[0].sy, vertices[1].sx, vertices[1].sy,
                    vertices[2].sx, vertices[2].sy, 24.0f, 0.05f))
    return 0;

  return AxisHalfRect3(vertices[0].tu, vertices[0].tv, vertices[1].tu, vertices[1].tv,
                       vertices[2].tu, vertices[2].tv, 0.0001f, 0.0015f);
}

static int HasTransparentVertex(const D3DTLVERTEX_COMPAT* vertices, DWORD vertex_count)
{
  if (!vertices || vertex_count == 0 || g_precise_min_vertex_alpha <= 0)
    return 0;

  for (DWORD i = 0; i < vertex_count; i++)
  {
    const int alpha = (int)((vertices[i].color >> 24) & 0xFFu);
    if (alpha < g_precise_min_vertex_alpha)
      return 1;
  }
  return 0;
}

static int HasTransparentIndexedVertex(const D3DTLVERTEX_COMPAT* vertices, DWORD vertex_count,
                                       const WORD* indices, DWORD index_count)
{
  if (!vertices || !indices || vertex_count == 0 || index_count == 0 || g_precise_min_vertex_alpha <= 0)
    return 0;

  for (DWORD i = 0; i < index_count; i++)
  {
    const DWORD idx = (DWORD)indices[i];
    if (idx >= vertex_count)
      return 1;

    const int alpha = (int)((vertices[idx].color >> 24) & 0xFFu);
    if (alpha < g_precise_min_vertex_alpha)
      return 1;
  }
  return 0;
}

static int MinVertexAlpha(const D3DTLVERTEX_COMPAT* vertices, DWORD vertex_count)
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

static int MinIndexedVertexAlpha(const D3DTLVERTEX_COMPAT* vertices, DWORD vertex_count,
                                 const WORD* indices, DWORD index_count)
{
  if (!vertices || !indices || vertex_count == 0 || index_count == 0)
    return 255;

  int min_alpha = 255;
  for (DWORD i = 0; i < index_count; i++)
  {
    const DWORD idx = (DWORD)indices[i];
    if (idx >= vertex_count)
      return 0;
    const int alpha = (int)((vertices[idx].color >> 24) & 0xFFu);
    if (alpha < min_alpha)
      min_alpha = alpha;
  }
  return min_alpha;
}

static int HasHardAlphaSpread(const D3DTLVERTEX_COMPAT* vertices, DWORD vertex_count)
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

static int HasHardIndexedAlphaSpread(const D3DTLVERTEX_COMPAT* vertices, DWORD vertex_count,
                                     const WORD* indices, DWORD index_count)
{
  if (!vertices || !indices || vertex_count == 0 || index_count == 0)
    return 0;

  int low = 0;
  int high = 0;
  for (DWORD i = 0; i < index_count; i++)
  {
    const DWORD idx = (DWORD)indices[i];
    if (idx >= vertex_count)
      return 0;
    const int alpha = (int)((vertices[idx].color >> 24) & 0xFFu);
    if (alpha <= g_cutout_hard_alpha_low_max)
      low = 1;
    if (alpha >= g_cutout_hard_alpha_high_min)
      high = 1;
  }
  return low && high;
}

static AlphaModelClass ClassifyAlphaModel(const D3DTLVERTEX_COMPAT* vertices, DWORD vertex_count,
                                          int min_alpha)
{
  if (min_alpha >= g_cutout_alpha_min || HasHardAlphaSpread(vertices, vertex_count))
    return ALPHA_MODEL_CUTOUT;
  return ALPHA_MODEL_TRANSLUCENT;
}

static AlphaModelClass ClassifyIndexedAlphaModel(const D3DTLVERTEX_COMPAT* vertices, DWORD vertex_count,
                                                 const WORD* indices, DWORD index_count,
                                                 int min_alpha)
{
  if (min_alpha >= g_cutout_alpha_min ||
      HasHardIndexedAlphaSpread(vertices, vertex_count, indices, index_count))
    return ALPHA_MODEL_CUTOUT;
  return ALPHA_MODEL_TRANSLUCENT;
}

static int IsLikelyAxisIndexedTileDraw(const D3DTLVERTEX_COMPAT* vertices, DWORD vertex_count,
                                       DWORD primitive_type, const WORD* indices, DWORD index_count,
                                       DWORD tris)
{
  if (!g_skip_axis_tile_draws || !vertices || !indices || primitive_type != D3DPT_TRIANGLELIST ||
      index_count != 3 || tris != 1)
    return 0;

  if ((DWORD)indices[0] >= vertex_count || (DWORD)indices[1] >= vertex_count || (DWORD)indices[2] >= vertex_count)
    return 0;

  D3DTLVERTEX_COMPAT tri[3];
  tri[0] = vertices[indices[0]];
  tri[1] = vertices[indices[1]];
  tri[2] = vertices[indices[2]];
  return IsLikelyAxisTileDraw(tri, primitive_type, 3, 1);
}

static int IsModelCallsite(DWORD caller)
{
  if (!g_require_model_callsite)
    return 1;
  return caller >= g_model_callsite_min && caller <= g_model_callsite_max;
}

static int IsBatchedModelCallsite(DWORD caller)
{
  return caller == g_re2_batched_model_callsite;
}

static int IsAcceptedModelCallsite(DWORD caller)
{
  return IsModelCallsite(caller) || IsBatchedModelCallsite(caller);
}

static float MaxModelScreenExtentForCaller(DWORD caller)
{
  return IsBatchedModelCallsite(caller) ? g_batched_model_max_screen_extent : g_max_screen_extent;
}

static float MaxModelScreenAreaForCaller(DWORD caller)
{
  return IsBatchedModelCallsite(caller) ? g_batched_model_max_screen_area : g_max_screen_area;
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

static int IndicesAreValid(const WORD* indices, DWORD index_count, DWORD vertex_count)
{
  if (!indices || index_count == 0)
    return 0;

  for (DWORD i = 0; i < index_count; i++)
  {
    if ((DWORD)indices[i] >= vertex_count)
      return 0;
  }
  return 1;
}

static void LogFlatDepthReject(DWORD caller, const char* label, const DrawBounds* bounds,
                               DWORD primitive_type, DWORD vertex_count, DWORD index_count)
{
  if (!g_log_enabled)
    return;
  if (!bounds)
    return;

  const LONG logged = InterlockedIncrement(&g_flat_depth_reject_logged);
  if (logged > 64)
    return;

  ModuleAddressInfo info;
  ResolveModuleForAddress(caller, &info);
  LogLine("flat-depth-reject #%ld %s caller=%s+0x%08lX type=%lu verts=%lu indices=%lu "
          "z=[%.6f..%.6f] rhw=[%.8f..%.8f] xy=[%.2f..%.2f %.2f..%.2f]",
          logged, label ? label : "draw", info.module_name, info.module_offset,
          primitive_type, vertex_count, index_count,
          bounds->min_z, bounds->max_z, bounds->min_rhw, bounds->max_rhw,
          bounds->min_x, bounds->max_x, bounds->min_y, bounds->max_y);
}

static const ZfixCallsiteProfile* FindModelCallsiteProfile(DWORD caller)
{
  if (!g_callsite_profiles_enabled)
    return NULL;
  return ZfixFindCallsiteProfile(g_callsite_profiles, (DWORD)ARRAYSIZE(g_callsite_profiles), caller);
}

static int CanResolveAdaptiveFlatDepth(const ZfixCallsiteProfile* profile)
{
  return g_adaptive_depth_conflict_resolver && ZfixProfileDepthEnabled(profile);
}

static float SnapPositiveFloat(float value, float step)
{
  if (step <= 0.0f)
    return value;
  const DWORD bucket = (DWORD)((value / step) + 0.5f);
  return (float)bucket * step;
}

static int IsModelDepthDraw(const D3DTLVERTEX_COMPAT* vertices, DWORD primitive_type, DWORD vertex_count, DWORD tris,
                            int indexed, DWORD caller, DrawBounds* bounds, const char** reason)
{
  if (reason)
    *reason = "ok";
  if (!g_enabled || !g_precise_model_depth_pass || !vertices || vertex_count == 0 || tris == 0)
  {
    if (reason)
      *reason = "off";
    return 0;
  }
  const int batched_model = IsBatchedModelCallsite(caller);
  if (!IsAcceptedModelCallsite(caller))
  {
    if (reason)
      *reason = "callsite";
    return 0;
  }
  if (primitive_type != D3DPT_TRIANGLELIST)
  {
    if (reason)
      *reason = "not_triangle_list";
    return 0;
  }
  if (batched_model)
  {
    if (indexed || vertex_count < 3 || vertex_count > g_batched_model_max_vertices ||
        (vertex_count % 3) != 0)
    {
      if (reason)
        *reason = "batched_shape";
      return 0;
    }
  }
  else if (!indexed && g_single_triangle_dp_only && (vertex_count != 3 || tris != 1))
  {
    if (reason)
      *reason = "not_single_dp_triangle";
    return 0;
  }
  if (indexed && !g_allow_indexed_model_draws)
  {
    if (reason)
      *reason = "indexed_off";
    return 0;
  }
  if (IsLikelyAxisTileDraw(vertices, primitive_type, vertex_count, tris))
  {
    if (reason)
      *reason = "axis_tile";
    return 0;
  }
  if (HasTransparentVertex(vertices, vertex_count))
  {
    if (reason)
      *reason = "vertex_alpha";
    return 0;
  }

  DrawBounds local_bounds;
  ComputeDrawBounds(vertices, vertex_count, &local_bounds);
  if (bounds)
    *bounds = local_bounds;

  const float max_screen_extent = MaxModelScreenExtentForCaller(caller);
  const float max_screen_area = MaxModelScreenAreaForCaller(caller);
  if (max_screen_extent > 0.0f &&
      (local_bounds.width > max_screen_extent || local_bounds.height > max_screen_extent))
  {
    if (reason)
      *reason = "large_extent";
    return 0;
  }
  if (max_screen_area > 0.0f && local_bounds.area > max_screen_area)
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
      *reason = "rhw_not_model";
    return 0;
  }
  if (IsSpikeLikeTriangle(&local_bounds))
  {
    if (reason)
      *reason = "spike_shape";
    return 0;
  }

  const float z_span = local_bounds.max_z - local_bounds.min_z;
  const float rhw_span = local_bounds.max_rhw - local_bounds.min_rhw;
  if (z_span < g_min_depth_variance && rhw_span < g_min_rhw_variance)
  {
    const ZfixCallsiteProfile* profile = FindModelCallsiteProfile(caller);
    if (!CanResolveAdaptiveFlatDepth(profile))
    {
      if (reason)
        *reason = "flat_depth";
      LogFlatDepthReject(caller, "opaque-dp", &local_bounds, primitive_type, vertex_count, 0);
      return 0;
    }
  }
  return 1;
}

static int IsIndexedModelDepthDraw(const D3DTLVERTEX_COMPAT* vertices, DWORD primitive_type, DWORD vertex_count,
                                   const WORD* indices, DWORD index_count, DWORD tris,
                                   DWORD caller, DrawBounds* bounds, const char** reason)
{
  if (reason)
    *reason = "ok";
  if (!g_enabled || !g_precise_model_depth_pass || !vertices || !indices ||
      vertex_count == 0 || index_count == 0 || tris == 0)
  {
    if (reason)
      *reason = "off";
    return 0;
  }
  if (!IsAcceptedModelCallsite(caller))
  {
    if (reason)
      *reason = "callsite";
    return 0;
  }
  if (primitive_type != D3DPT_TRIANGLELIST)
  {
    if (reason)
      *reason = "not_triangle_list";
    return 0;
  }
  if (!g_allow_indexed_model_draws)
  {
    if (reason)
      *reason = "indexed_off";
    return 0;
  }
  if (IsLikelyAxisIndexedTileDraw(vertices, vertex_count, primitive_type, indices, index_count, tris))
  {
    if (reason)
      *reason = "axis_tile";
    return 0;
  }
  if (HasTransparentIndexedVertex(vertices, vertex_count, indices, index_count))
  {
    if (reason)
      *reason = "vertex_alpha";
    return 0;
  }

  DrawBounds local_bounds;
  if (!ComputeIndexedDrawBounds(vertices, vertex_count, indices, index_count, &local_bounds))
  {
    if (reason)
      *reason = "bad_indices";
    return 0;
  }
  if (bounds)
    *bounds = local_bounds;

  const float max_screen_extent = MaxModelScreenExtentForCaller(caller);
  const float max_screen_area = MaxModelScreenAreaForCaller(caller);
  if (max_screen_extent > 0.0f &&
      (local_bounds.width > max_screen_extent || local_bounds.height > max_screen_extent))
  {
    if (reason)
      *reason = "large_extent";
    return 0;
  }
  if (max_screen_area > 0.0f && local_bounds.area > max_screen_area)
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
      *reason = "rhw_not_model";
    return 0;
  }
  if (IsSpikeLikeTriangle(&local_bounds))
  {
    if (reason)
      *reason = "spike_shape";
    return 0;
  }

  const float z_span = local_bounds.max_z - local_bounds.min_z;
  const float rhw_span = local_bounds.max_rhw - local_bounds.min_rhw;
  if (z_span < g_min_depth_variance && rhw_span < g_min_rhw_variance)
  {
    const ZfixCallsiteProfile* profile = FindModelCallsiteProfile(caller);
    if (!CanResolveAdaptiveFlatDepth(profile))
    {
      if (reason)
        *reason = "flat_depth";
      LogFlatDepthReject(caller, "opaque-dip", &local_bounds, primitive_type, vertex_count, index_count);
      return 0;
    }
  }
  return 1;
}

static int IsTransparentModelDepthDraw(const D3DTLVERTEX_COMPAT* vertices, DWORD primitive_type,
                                       DWORD vertex_count, DWORD tris, int indexed, DWORD caller,
                                       DrawBounds* bounds, const char** reason)
{
  if (reason)
    *reason = "transparent_ok";
  if (!g_enabled || !g_precise_model_depth_pass || !g_transparent_model_z_test ||
      !vertices || vertex_count == 0 || tris == 0)
  {
    if (reason)
      *reason = "transparent_off";
    return 0;
  }
  if (!IsModelCallsite(caller))
  {
    if (reason)
      *reason = "transparent_callsite";
    return 0;
  }
  if (primitive_type != D3DPT_TRIANGLELIST)
  {
    if (reason)
      *reason = "transparent_not_triangle_list";
    return 0;
  }
  if (!indexed && g_single_triangle_dp_only && (vertex_count != 3 || tris != 1))
  {
    if (reason)
      *reason = "transparent_not_single_dp_triangle";
    return 0;
  }
  if (indexed && !g_allow_indexed_model_draws)
  {
    if (reason)
      *reason = "transparent_indexed_off";
    return 0;
  }
  if (IsLikelyAxisTileDraw(vertices, primitive_type, vertex_count, tris))
  {
    if (reason)
      *reason = "transparent_axis_tile";
    return 0;
  }
  if (!HasTransparentVertex(vertices, vertex_count))
  {
    if (reason)
      *reason = "transparent_no_alpha";
    return 0;
  }

  DrawBounds local_bounds;
  ComputeDrawBounds(vertices, vertex_count, &local_bounds);
  if (bounds)
    *bounds = local_bounds;

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
      *reason = "transparent_rhw_not_model";
    return 0;
  }
  if (IsSpikeLikeTriangle(&local_bounds))
  {
    if (reason)
      *reason = "transparent_spike_shape";
    return 0;
  }

  const float z_span = local_bounds.max_z - local_bounds.min_z;
  const float rhw_span = local_bounds.max_rhw - local_bounds.min_rhw;
  if (z_span < g_min_depth_variance && rhw_span < g_min_rhw_variance)
  {
    const ZfixCallsiteProfile* profile = FindModelCallsiteProfile(caller);
    if (!CanResolveAdaptiveFlatDepth(profile))
    {
      if (reason)
        *reason = "transparent_flat_depth";
      return 0;
    }
  }
  return 1;
}

static int IsTransparentIndexedModelDepthDraw(const D3DTLVERTEX_COMPAT* vertices, DWORD primitive_type,
                                              DWORD vertex_count, const WORD* indices, DWORD index_count,
                                              DWORD tris, DWORD caller, DrawBounds* bounds,
                                              const char** reason)
{
  if (reason)
    *reason = "transparent_ok";
  if (!g_enabled || !g_precise_model_depth_pass || !g_transparent_model_z_test ||
      !vertices || !indices || vertex_count == 0 || index_count == 0 || tris == 0)
  {
    if (reason)
      *reason = "transparent_off";
    return 0;
  }
  if (!IsModelCallsite(caller))
  {
    if (reason)
      *reason = "transparent_callsite";
    return 0;
  }
  if (primitive_type != D3DPT_TRIANGLELIST)
  {
    if (reason)
      *reason = "transparent_not_triangle_list";
    return 0;
  }
  if (!g_allow_indexed_model_draws)
  {
    if (reason)
      *reason = "transparent_indexed_off";
    return 0;
  }
  if (IsLikelyAxisIndexedTileDraw(vertices, vertex_count, primitive_type, indices, index_count, tris))
  {
    if (reason)
      *reason = "transparent_axis_tile";
    return 0;
  }
  if (!HasTransparentIndexedVertex(vertices, vertex_count, indices, index_count))
  {
    if (reason)
      *reason = "transparent_no_alpha";
    return 0;
  }

  DrawBounds local_bounds;
  if (!ComputeIndexedDrawBounds(vertices, vertex_count, indices, index_count, &local_bounds))
  {
    if (reason)
      *reason = "transparent_bad_indices";
    return 0;
  }
  if (bounds)
    *bounds = local_bounds;

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
      *reason = "transparent_rhw_not_model";
    return 0;
  }
  if (IsSpikeLikeTriangle(&local_bounds))
  {
    if (reason)
      *reason = "transparent_spike_shape";
    return 0;
  }

  const float z_span = local_bounds.max_z - local_bounds.min_z;
  const float rhw_span = local_bounds.max_rhw - local_bounds.min_rhw;
  if (z_span < g_min_depth_variance && rhw_span < g_min_rhw_variance)
  {
    const ZfixCallsiteProfile* profile = FindModelCallsiteProfile(caller);
    if (!CanResolveAdaptiveFlatDepth(profile))
    {
      if (reason)
        *reason = "transparent_flat_depth";
      return 0;
    }
  }
  return 1;
}

static DWORD ApplyAdaptiveDepthConflictResolver(D3DTLVERTEX_COMPAT* vertices, DWORD vertex_count,
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
  if (aggressive && caller == g_crow_profile_callsite &&
      z_span <= g_crow_profile_flat_boost_z_span &&
      g_crow_profile_flat_boost_target_span > target_span)
  {
    target_span = g_crow_profile_flat_boost_target_span;
    if (max_span > 0.0f && target_span > max_span)
      target_span = max_span;
  }
  else if (aggressive && caller == g_crow_profile_callsite &&
           z_span <= g_crow_profile_mid_boost_z_span &&
           g_crow_profile_mid_boost_target_span > target_span)
  {
    target_span = g_crow_profile_mid_boost_target_span;
    if (max_span > 0.0f && target_span > max_span)
      target_span = max_span;
  }
  if (aggressive && caller == g_crow_profile_callsite && g_crow_profile_precision_snap)
  {
    target_span = SnapPositiveFloat(target_span, g_crow_profile_span_snap);
    if (max_span > 0.0f && target_span > max_span)
      target_span = max_span;
  }

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
  if (aggressive && caller == g_crow_profile_callsite && g_crow_profile_precision_snap)
  {
    center_z = ClampDepth(SnapPositiveFloat(center_z, g_crow_profile_center_snap));
    if (center_z < half_span)
      center_z = half_span;
    if (center_z > (1.0f - half_span))
      center_z = 1.0f - half_span;
  }

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
        t = 1.0f - ((vertices[i].rhw - source_min) / source_span);
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
      ModuleAddressInfo info;
      ResolveModuleForAddress(caller, &info);
      LogLine("adaptive-depth #%ld %s profile=%s caller=%s+0x%08lX raw=0x%08lX "
              "verts=%lu changed=%lu mode=%s zSpan=%.8f target=%.8f rhwSpan=%.8f "
              "area=%.2f extent=%.2f maxShift=%.8f",
              logged, label ? label : "draw", profile ? profile->name : "default",
              info.module_name, info.module_offset, caller, vertex_count, changed,
              mode_name, z_span, effective_span, rhw_span, bounds->area, extent,
              max_shift);
    }
  }
  return changed;
}

static void TrackRenderState(DWORD state, DWORD value)
{
  if (state < ARRAYSIZE(g_render_state_cache))
    g_render_state_cache[state] = value;

  DWORD* tracked = NULL;
  switch (state)
  {
    case D3DRENDERSTATE_ZENABLE:
      tracked = &g_rs_z_enable;
      break;
    case D3DRENDERSTATE_ZWRITEENABLE:
      tracked = &g_rs_z_write;
      break;
    case D3DRENDERSTATE_ZFUNC:
      tracked = &g_rs_z_func;
      break;
    case D3DRENDERSTATE_ALPHABLENDENABLE:
      tracked = &g_rs_alpha_blend;
      break;
    case D3DRENDERSTATE_ZBIAS:
      tracked = &g_rs_z_bias;
      break;
    default:
      return;
  }

  *tracked = value;
}

static DWORD TrackedRenderStateValue(DWORD state)
{
  if (state < ARRAYSIZE(g_render_state_cache) && g_render_state_cache[state] != 0xFFFFFFFFu)
    return g_render_state_cache[state];

  switch (state)
  {
    case D3DRENDERSTATE_ZENABLE:
      return g_rs_z_enable;
    case D3DRENDERSTATE_ZWRITEENABLE:
      return g_rs_z_write;
    case D3DRENDERSTATE_ZFUNC:
      return g_rs_z_func;
    case D3DRENDERSTATE_ZBIAS:
      return g_rs_z_bias;
    default:
      return 0xFFFFFFFFu;
  }
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

  const DWORD texture = TrackedRenderStateValue(D3DRENDERSTATE_TEXTUREHANDLE);
  if (!texture || texture == 0xFFFFFFFFu)
    return;
  info->texture_handle = texture;
  const TextureHandleTrace* texture_trace = FindTextureHandleTraceByHandle(texture);
  if (texture_trace)
  {
    info->texture_width = texture_trace->width;
    info->texture_height = texture_trace->height;
  }
}

static ZfixTexturePageClass ClassifyCurrentD3D2MaskTexture(DWORD* out_texture)
{
  if (out_texture)
    *out_texture = 0;
  if (!g_re2_mask_overlay_guard)
    return ZFIX_TEXTURE_PAGE_NONE;

  const DWORD texture = TrackedRenderStateValue(D3DRENDERSTATE_TEXTUREHANDLE);
  if (out_texture)
    *out_texture = texture;
  if (!texture || texture == 0xFFFFFFFFu)
    return ZFIX_TEXTURE_PAGE_NONE;

  const TextureHandleTrace* trace = FindTextureHandleTraceByHandle(texture);
  if (!trace || !trace->width || !trace->height)
    return ZFIX_TEXTURE_PAGE_NONE;

  return ZfixClassifyMaskTexturePage(trace->width, trace->height,
                                     g_re2_hires_mask_texture_min_side,
                                     g_re2_classic_mask_texture_min_side,
                                     g_re2_classic_mask_texture_max_side);
}

static int IsD3D2MaskOverlayPrimitiveShape(DWORD primitive_type, DWORD element_count, DWORD tris)
{
  if (tris != 2)
    return 0;
  if (primitive_type == D3DPT_TRIANGLELIST)
    return element_count == 6;
  if (primitive_type == D3DPT_TRIANGLESTRIP || primitive_type == D3DPT_TRIANGLEFAN)
    return element_count == 4;
  return 0;
}

static int D3D2VertexMatchesBoundsAxis(const D3DTLVERTEX_COMPAT* vertex,
                                       const DrawBounds* bounds)
{
  if (!vertex || !bounds)
    return 0;
  if (!NearF(vertex->sx, bounds->min_x, g_re2_mask_overlay_axis_epsilon) &&
      !NearF(vertex->sx, bounds->max_x, g_re2_mask_overlay_axis_epsilon))
    return 0;
  if (!NearF(vertex->sy, bounds->min_y, g_re2_mask_overlay_axis_epsilon) &&
      !NearF(vertex->sy, bounds->max_y, g_re2_mask_overlay_axis_epsilon))
    return 0;
  return 1;
}

static int D3D2VerticesMatchBoundsAxis(const D3DTLVERTEX_COMPAT* vertices,
                                       DWORD vertex_count, const WORD* indices,
                                       DWORD index_count, const DrawBounds* bounds)
{
  if (!vertices || !bounds)
    return 0;

  if (indices && index_count)
  {
    for (DWORD i = 0; i < index_count; i++)
    {
      const DWORD idx = (DWORD)indices[i];
      if (idx >= vertex_count ||
          !D3D2VertexMatchesBoundsAxis(&vertices[idx], bounds))
        return 0;
    }
    return 1;
  }

  for (DWORD i = 0; i < vertex_count; i++)
  {
    if (!D3D2VertexMatchesBoundsAxis(&vertices[i], bounds))
      return 0;
  }
  return 1;
}

static int IsD3D2MaskOverlayBounds(const DrawBounds* bounds)
{
  if (!bounds)
    return 0;

  const float width = AbsF(bounds->width);
  const float height = AbsF(bounds->height);
  const float extent = width > height ? width : height;
  if (width < g_re2_mask_overlay_min_extent ||
      height < g_re2_mask_overlay_min_extent ||
      bounds->area < g_re2_mask_overlay_min_area ||
      extent > g_re2_mask_overlay_max_extent)
    return 0;
  if (AbsF(bounds->max_z - bounds->min_z) > g_re2_mask_overlay_flat_z_span)
    return 0;
  if (bounds->min_rhw < g_re2_mask_overlay_min_rhw ||
      AbsF(bounds->max_rhw - bounds->min_rhw) > g_re2_mask_overlay_flat_rhw_span)
    return 0;
  return 1;
}

static int IsLikelyD3D2MaskOverlayDraw(const D3DTLVERTEX_COMPAT* vertices,
                                       DWORD vertex_count, DWORD primitive_type,
                                       const WORD* indices, DWORD index_count,
                                       DWORD tris, DrawBounds* out_bounds,
                                       ZfixTexturePageClass* out_texture_class,
                                       DWORD* out_texture)
{
  if (out_texture_class)
    *out_texture_class = ZFIX_TEXTURE_PAGE_NONE;
  if (out_bounds)
    memset(out_bounds, 0, sizeof(*out_bounds));

  DWORD texture = 0;
  const ZfixTexturePageClass texture_class = ClassifyCurrentD3D2MaskTexture(&texture);
  if (out_texture)
    *out_texture = texture;
  if (texture_class == ZFIX_TEXTURE_PAGE_NONE || !vertices || vertex_count == 0)
    return 0;

  const DWORD element_count = indices ? index_count : vertex_count;
  if (!IsD3D2MaskOverlayPrimitiveShape(primitive_type, element_count, tris))
    return 0;

  DrawBounds bounds;
  int bounds_ok = 0;
  if (indices)
    bounds_ok = ComputeIndexedDrawBounds(vertices, vertex_count, indices, index_count, &bounds);
  else
  {
    ComputeDrawBounds(vertices, vertex_count, &bounds);
    bounds_ok = 1;
  }
  if (!bounds_ok || !IsD3D2MaskOverlayBounds(&bounds))
    return 0;
  if (!D3D2VerticesMatchBoundsAxis(vertices, vertex_count, indices, index_count, &bounds))
    return 0;

  if (out_bounds)
    *out_bounds = bounds;
  if (out_texture_class)
    *out_texture_class = texture_class;
  return 1;
}

static void LogD3D2MaskOverlaySkip(DWORD caller, int indexed, DWORD primitive_type,
                                   DWORD vertex_count, DWORD index_count, DWORD tris,
                                   DWORD texture, ZfixTexturePageClass texture_class,
                                   const DrawBounds* bounds)
{
  if (!g_log_enabled)
    return;

  const LONG skipped = InterlockedIncrement(&g_re2_mask_overlay_skipped);
  const LONG logged = InterlockedIncrement(&g_re2_mask_overlay_logged);
  if (logged > 96)
    return;

  ModuleAddressInfo info;
  ResolveModuleForAddress(caller, &info);
  LogLine("re2 mask-overlay-skip #%ld reason=%s caller=%s+0x%08lX raw=0x%08lX "
          "indexed=%d type=%lu verts=%lu indices=%lu tris=%lu texture=0x%08lX "
          "xy=[%.2f..%.2f %.2f..%.2f] z=[%.6f..%.6f] rhw=[%.8f..%.8f]",
          skipped,
          texture_class == ZFIX_TEXTURE_PAGE_HIRES ? "hires-texture" : "classic-texture",
          info.module_name, info.module_offset, caller, indexed, primitive_type,
          vertex_count, index_count, tris, texture,
          bounds ? bounds->min_x : 0.0f, bounds ? bounds->max_x : 0.0f,
          bounds ? bounds->min_y : 0.0f, bounds ? bounds->max_y : 0.0f,
          bounds ? bounds->min_z : 0.0f, bounds ? bounds->max_z : 0.0f,
          bounds ? bounds->min_rhw : 0.0f, bounds ? bounds->max_rhw : 0.0f);
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

static int CaptureOneRenderState(void* self, DWORD state, DWORD* value)
{
  D3DDevice2GetRenderStateProc get_rs = (D3DDevice2GetRenderStateProc)GetVTableSlot(self, 22);
  if (get_rs && SUCCEEDED(get_rs(self, state, value)))
    return 1;

  *value = TrackedRenderStateValue(state);
  return *value != 0xFFFFFFFFu;
}

// Render-state capture and forcing.
static void CaptureDepthState(void* self, DepthStateSnapshot* snapshot)
{
  memset(snapshot, 0, sizeof(*snapshot));
  snapshot->has_z_enable = CaptureOneRenderState(self, D3DRENDERSTATE_ZENABLE, &snapshot->z_enable);
  snapshot->has_z_write = CaptureOneRenderState(self, D3DRENDERSTATE_ZWRITEENABLE, &snapshot->z_write);
  snapshot->has_z_func = CaptureOneRenderState(self, D3DRENDERSTATE_ZFUNC, &snapshot->z_func);
  snapshot->has_z_bias = CaptureOneRenderState(self, D3DRENDERSTATE_ZBIAS, &snapshot->z_bias);
  snapshot->has_texture_perspective =
    CaptureOneRenderState(self, D3DRENDERSTATE_TEXTUREPERSPECTIVE, &snapshot->texture_perspective);
  snapshot->has_texture_mag = CaptureOneRenderState(self, D3DRENDERSTATE_TEXTUREMAG, &snapshot->texture_mag);
  snapshot->has_texture_min = CaptureOneRenderState(self, D3DRENDERSTATE_TEXTUREMIN, &snapshot->texture_min);
  snapshot->has_alpha_test =
    CaptureOneRenderState(self, D3DRENDERSTATE_ALPHATESTENABLE, &snapshot->alpha_test);
  snapshot->has_alpha_ref = CaptureOneRenderState(self, D3DRENDERSTATE_ALPHAREF, &snapshot->alpha_ref);
  snapshot->has_alpha_func = CaptureOneRenderState(self, D3DRENDERSTATE_ALPHAFUNC, &snapshot->alpha_func);
  snapshot->has_alpha_blend =
    CaptureOneRenderState(self, D3DRENDERSTATE_ALPHABLENDENABLE, &snapshot->alpha_blend);
  snapshot->has_cull_mode = CaptureOneRenderState(self, D3DRENDERSTATE_CULLMODE, &snapshot->cull_mode);
  snapshot->has_shade_mode = CaptureOneRenderState(self, D3DRENDERSTATE_SHADEMODE, &snapshot->shade_mode);
  snapshot->has_dither_enable = CaptureOneRenderState(self, D3DRENDERSTATE_DITHERENABLE, &snapshot->dither_enable);
  snapshot->has_subpixel = CaptureOneRenderState(self, D3DRENDERSTATE_SUBPIXEL, &snapshot->subpixel);
  snapshot->has_subpixel_x = CaptureOneRenderState(self, D3DRENDERSTATE_SUBPIXELX, &snapshot->subpixel_x);
}

static void SetOneRenderState(void* self, DWORD state, DWORD value)
{
  D3DDevice2SetRenderStateProc set_rs = (D3DDevice2SetRenderStateProc)GetOriginal(*(void***)self, 23);
  if (!set_rs)
    return;

  set_rs(self, state, value);
  TrackRenderState(state, value);
}

static void ForceDepthStateForDraw(void* self)
{
  if (g_force_z_enable != ZFIX_SKIP_STATE)
    SetOneRenderState(self, D3DRENDERSTATE_ZENABLE, (DWORD)g_force_z_enable);
  if (g_force_z_write != ZFIX_SKIP_STATE)
    SetOneRenderState(self, D3DRENDERSTATE_ZWRITEENABLE, (DWORD)g_force_z_write);
  if (g_force_z_func != ZFIX_SKIP_STATE)
    SetOneRenderState(self, D3DRENDERSTATE_ZFUNC, (DWORD)g_force_z_func);
  if (g_force_z_bias != ZFIX_SKIP_STATE)
    SetOneRenderState(self, D3DRENDERSTATE_ZBIAS, (DWORD)g_force_z_bias);
}

static void ForceModelQualityStateForDraw(void* self, const DepthStateSnapshot* snapshot)
{
  if (!snapshot)
    return;

  if (g_model_texture_perspective && snapshot->has_texture_perspective)
    SetOneRenderState(self, D3DRENDERSTATE_TEXTUREPERSPECTIVE, 1);
  if (g_model_alpha_test)
  {
    if (snapshot->has_alpha_test)
      SetOneRenderState(self, D3DRENDERSTATE_ALPHATESTENABLE, 1);
    if (snapshot->has_alpha_ref)
      SetOneRenderState(self, D3DRENDERSTATE_ALPHAREF, g_model_alpha_ref);
    if (snapshot->has_alpha_func)
      SetOneRenderState(self, D3DRENDERSTATE_ALPHAFUNC, g_model_alpha_func);
  }
  if (g_model_gouraud_shading && snapshot->has_shade_mode)
    SetOneRenderState(self, D3DRENDERSTATE_SHADEMODE, D3DSHADE_GOURAUD);
  if (g_model_dither && snapshot->has_dither_enable)
    SetOneRenderState(self, D3DRENDERSTATE_DITHERENABLE, 1);
  if (g_model_subpixel && snapshot->has_subpixel)
    SetOneRenderState(self, D3DRENDERSTATE_SUBPIXEL, 1);
  if (g_model_subpixel && snapshot->has_subpixel_x)
    SetOneRenderState(self, D3DRENDERSTATE_SUBPIXELX, 1);
}

static void ForceCutoutGeometryState(void* self, const DepthStateSnapshot* snapshot)
{
  if (!snapshot)
    return;

  if (g_model_alpha_test)
  {
    if (snapshot->has_alpha_test)
      SetOneRenderState(self, D3DRENDERSTATE_ALPHATESTENABLE, 1);
    if (snapshot->has_alpha_ref)
      SetOneRenderState(self, D3DRENDERSTATE_ALPHAREF, g_cutout_model_alpha_ref);
    if (snapshot->has_alpha_func)
      SetOneRenderState(self, D3DRENDERSTATE_ALPHAFUNC, g_model_alpha_func);
  }
  if (g_cutout_model_two_sided && snapshot->has_cull_mode)
    SetOneRenderState(self, D3DRENDERSTATE_CULLMODE, D3DCULL_NONE);
  if (g_cutout_model_disable_alpha_blend && snapshot->has_alpha_blend)
    SetOneRenderState(self, D3DRENDERSTATE_ALPHABLENDENABLE, 0);
}

static void ForceDepthStateForTransparentDraw(void* self)
{
  SetOneRenderState(self, D3DRENDERSTATE_ZENABLE, 1);
  if (g_transparent_model_z_write != ZFIX_SKIP_STATE)
    SetOneRenderState(self, D3DRENDERSTATE_ZWRITEENABLE, (DWORD)g_transparent_model_z_write);
  if (g_transparent_model_z_func != ZFIX_SKIP_STATE)
    SetOneRenderState(self, D3DRENDERSTATE_ZFUNC, (DWORD)g_transparent_model_z_func);
  if (g_transparent_model_z_bias != ZFIX_SKIP_STATE)
    SetOneRenderState(self, D3DRENDERSTATE_ZBIAS, (DWORD)g_transparent_model_z_bias);
}

static void RestoreDepthStateForDraw(void* self, const DepthStateSnapshot* snapshot)
{
  if (!g_restore_depth_state || !snapshot)
    return;

  if (snapshot->has_z_enable)
    SetOneRenderState(self, D3DRENDERSTATE_ZENABLE, snapshot->z_enable);
  if (snapshot->has_z_write)
    SetOneRenderState(self, D3DRENDERSTATE_ZWRITEENABLE, snapshot->z_write);
  if (snapshot->has_z_func)
    SetOneRenderState(self, D3DRENDERSTATE_ZFUNC, snapshot->z_func);
  if (snapshot->has_z_bias)
    SetOneRenderState(self, D3DRENDERSTATE_ZBIAS, snapshot->z_bias);
  if (snapshot->has_texture_perspective)
    SetOneRenderState(self, D3DRENDERSTATE_TEXTUREPERSPECTIVE, snapshot->texture_perspective);
  if (snapshot->has_texture_mag)
    SetOneRenderState(self, D3DRENDERSTATE_TEXTUREMAG, snapshot->texture_mag);
  if (snapshot->has_texture_min)
    SetOneRenderState(self, D3DRENDERSTATE_TEXTUREMIN, snapshot->texture_min);
  if (snapshot->has_alpha_test)
    SetOneRenderState(self, D3DRENDERSTATE_ALPHATESTENABLE, snapshot->alpha_test);
  if (snapshot->has_alpha_ref)
    SetOneRenderState(self, D3DRENDERSTATE_ALPHAREF, snapshot->alpha_ref);
  if (snapshot->has_alpha_func)
    SetOneRenderState(self, D3DRENDERSTATE_ALPHAFUNC, snapshot->alpha_func);
  if (snapshot->has_alpha_blend)
    SetOneRenderState(self, D3DRENDERSTATE_ALPHABLENDENABLE, snapshot->alpha_blend);
  if (snapshot->has_cull_mode)
    SetOneRenderState(self, D3DRENDERSTATE_CULLMODE, snapshot->cull_mode);
  if (snapshot->has_shade_mode)
    SetOneRenderState(self, D3DRENDERSTATE_SHADEMODE, snapshot->shade_mode);
  if (snapshot->has_dither_enable)
    SetOneRenderState(self, D3DRENDERSTATE_DITHERENABLE, snapshot->dither_enable);
  if (snapshot->has_subpixel)
    SetOneRenderState(self, D3DRENDERSTATE_SUBPIXEL, snapshot->subpixel);
  if (snapshot->has_subpixel_x)
    SetOneRenderState(self, D3DRENDERSTATE_SUBPIXELX, snapshot->subpixel_x);
}

static void ClearDepthBuffer(void* self)
{
  D3DDevice2GetCurrentViewportProc get_viewport =
    (D3DDevice2GetCurrentViewportProc)GetVTableSlot(self, 14);
  if (!get_viewport)
    return;

  void* viewport = NULL;
  HRESULT hr = get_viewport(self, &viewport);
  if (FAILED(hr) || !viewport)
    return;

  void** vt = *(void***)viewport;
  D3DViewport2ClearProc clear = vt ? (D3DViewport2ClearProc)vt[12] : NULL;
  if (clear)
    clear(viewport, 0, NULL, D3DCLEAR_ZBUFFER);
  ZfixReleaseComObject(viewport);
}

static void ClearDepthBufferForScene(void* self)
{
  if (!g_clear_depth_each_scene)
    return;
  ClearDepthBuffer(self);
}

static int IsKnown2DDepthBoundaryCaller(DWORD caller)
{
  return (caller >= 0x0040E640u && caller <= 0x0040E6A0u) ||
         (caller >= 0x0040EBF0u && caller <= 0x0040EC20u);
}

static int IsKnown2DDepthBoundaryBounds(const DrawBounds* bounds)
{
  if (!bounds)
    return 0;

  const int flat_z = AbsF(bounds->min_z - 0.5f) <= 0.002f &&
                     AbsF(bounds->max_z - 0.5f) <= 0.002f;
  const int screen_rhw = bounds->min_rhw >= 1.5f && bounds->max_rhw <= 2.5f &&
                         (bounds->max_rhw - bounds->min_rhw) <= 0.02f;
  return flat_z && screen_rhw;
}

static void ClearModelDepthBeforeKnown2D(void* self, DWORD caller, DWORD vertex_type,
                                         const void* vertices, DWORD vertex_count,
                                         const WORD* indices, DWORD index_count)
{
  if (!g_clear_model_depth_before_2d || !g_model_depth_written_this_scene ||
      vertex_type != D3DVT_TLVERTEX || !vertices || vertex_count == 0 ||
      !IsKnown2DDepthBoundaryCaller(caller))
    return;

  DrawBounds bounds;
  int bounds_ok = 0;
  if (indices && index_count > 0)
    bounds_ok = ComputeIndexedDrawBounds((const D3DTLVERTEX_COMPAT*)vertices, vertex_count, indices, index_count, &bounds);
  else
  {
    ComputeDrawBounds((const D3DTLVERTEX_COMPAT*)vertices, vertex_count, &bounds);
    bounds_ok = 1;
  }

  if (!bounds_ok || !IsKnown2DDepthBoundaryBounds(&bounds))
    return;

  ClearDepthBuffer(self);
  g_model_depth_written_this_scene = 0;
}

static void CaptureBatchRenderState(void* self, BatchRenderState* state)
{
  if (!state)
    return;

  memset(state, 0, sizeof(*state));
  for (DWORD i = 0; i < ARRAYSIZE(k_model_sort_states); i++)
  {
    DWORD value = 0;
    if (CaptureOneRenderState(self, k_model_sort_states[i], &value))
    {
      state->value[i] = value;
      state->has[i] = 1;
    }
  }
}

static void ApplyBatchRenderState(void* self, const BatchRenderState* state)
{
  if (!state)
    return;

  for (DWORD i = 0; i < ARRAYSIZE(k_model_sort_states); i++)
  {
    if (state->has[i])
      SetOneRenderState(self, k_model_sort_states[i], state->value[i]);
  }
}

static void ForceInvisibleDepthPrepassState(void* self)
{
  SetOneRenderState(self, D3DRENDERSTATE_ZENABLE, 1);
  SetOneRenderState(self, D3DRENDERSTATE_ZWRITEENABLE, 1);
  SetOneRenderState(self, D3DRENDERSTATE_ZFUNC, D3DCMP_LESSEQUAL);
  SetOneRenderState(self, D3DRENDERSTATE_ALPHABLENDENABLE, 1);
  SetOneRenderState(self, D3DRENDERSTATE_SRCBLEND, D3DBLEND_ZERO);
  SetOneRenderState(self, D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
}

static void ForceModelColorPassAfterPrepassState(void* self, const ZfixCallsiteProfile* profile)
{
  SetOneRenderState(self, D3DRENDERSTATE_ZENABLE, 1);
  SetOneRenderState(self, D3DRENDERSTATE_ZWRITEENABLE, (DWORD)g_model_color_pass_z_write);
  SetOneRenderState(self, D3DRENDERSTATE_ZFUNC,
                    ZfixProfileColorZFunc(profile, (DWORD)g_model_color_pass_z_func));
}

static void RunModelDepthPrepass(void* self, D3DDevice2DrawPrimitiveProc orig,
                                 DWORD primitive_type, DWORD vertex_type,
                                 D3DTLVERTEX_COMPAT* vertices, DWORD vertex_count,
                                 DWORD flags)
{
  if (!g_model_depth_prepass || primitive_type != D3DPT_TRIANGLELIST ||
      vertex_type != D3DVT_TLVERTEX || !self || !orig || !vertices || vertex_count < 3)
    return;

  BatchRenderState restore_state;
  CaptureBatchRenderState(self, &restore_state);
  ForceInvisibleDepthPrepassState(self);
  const HRESULT hr = orig(self, primitive_type, vertex_type, vertices, vertex_count, flags);
  ApplyBatchRenderState(self, &restore_state);

  if (SUCCEEDED(hr))
    LogCounterIncrement(&g_model_depth_prepass_draws);
  else
    LogCounterIncrement(&g_model_depth_prepass_failures);
}

static void RunIndexedModelDepthPrepass(void* self, D3DDevice2DrawIndexedPrimitiveProc orig,
                                        DWORD primitive_type, DWORD vertex_type,
                                        D3DTLVERTEX_COMPAT* vertices, DWORD vertex_count,
                                        WORD* indices, DWORD index_count, DWORD flags)
{
  if (!g_model_depth_prepass || primitive_type != D3DPT_TRIANGLELIST ||
      vertex_type != D3DVT_TLVERTEX || !self || !orig || !vertices || !indices ||
      vertex_count == 0 || index_count < 3)
    return;

  BatchRenderState restore_state;
  CaptureBatchRenderState(self, &restore_state);
  ForceInvisibleDepthPrepassState(self);
  const HRESULT hr =
    orig(self, primitive_type, vertex_type, vertices, vertex_count, indices, index_count, flags);
  ApplyBatchRenderState(self, &restore_state);

  if (SUCCEEDED(hr))
    LogCounterIncrement(&g_model_depth_prepass_draws);
  else
    LogCounterIncrement(&g_model_depth_prepass_failures);
}

static HRESULT DrawPrimitiveWithModelDepth(void* self, D3DDevice2DrawPrimitiveProc orig, DWORD primitive_type,
                                           DWORD vertex_type, void* vertices, DWORD vertex_count, DWORD flags,
                                           DWORD caller, DWORD tris, const DrawBounds* bounds)
{
  (void)tris;
  const ZfixCallsiteProfile* profile = NULL;
  DepthStateSnapshot snapshot;
  CaptureDepthState(self, &snapshot);
  ForceDepthStateForDraw(self);
  ForceModelQualityStateForDraw(self, &snapshot);

  D3DTLVERTEX_COMPAT stack_copy[ZFIX_STACK_VERTEX_CAPACITY];
  SIZE_T copy_bytes = 0;
  if (!ZfixCheckedSizeMul(sizeof(stack_copy[0]), (SIZE_T)vertex_count, &copy_bytes))
  {
    HRESULT hr = orig(self, primitive_type, vertex_type, vertices, vertex_count, flags);
    RestoreDepthStateForDraw(self, &snapshot);
    return hr;
  }

  int heap_copy = 0;
  D3DTLVERTEX_COMPAT* copy =
    (D3DTLVERTEX_COMPAT*)ZfixAcquireCopyBuffer(copy_bytes, stack_copy, sizeof(stack_copy), &heap_copy);
  if (!copy)
  {
    HRESULT hr = orig(self, primitive_type, vertex_type, vertices, vertex_count, flags);
    RestoreDepthStateForDraw(self, &snapshot);
    return hr;
  }

  memcpy(copy, vertices, copy_bytes);
  DrawBounds adjusted_bounds;
  ComputeDrawBounds(copy, vertex_count, &adjusted_bounds);
  profile = FindModelCallsiteProfileForDraw(caller, &adjusted_bounds);
  if (profile)
    LogCounterIncrement(&g_callsite_profile_hits);
  const DWORD adaptive_changed =
    ApplyAdaptiveDepthConflictResolver(copy, vertex_count, &adjusted_bounds,
                                       profile, caller, "opaque-dp");
  if (adaptive_changed)
    ComputeDrawBounds(copy, vertex_count, &adjusted_bounds);
  LogZFightSample(caller, "opaque-dp", 0, primitive_type, vertex_count, 0,
                  bounds, &adjusted_bounds,
                  adaptive_changed);

  RunModelDepthPrepass(self, orig, primitive_type, vertex_type, copy, vertex_count, flags);
  ForceDepthStateForDraw(self);
  ForceModelQualityStateForDraw(self, &snapshot);
  if (g_model_depth_prepass)
    ForceModelColorPassAfterPrepassState(self, profile);
  HRESULT hr = orig(self, primitive_type, vertex_type, copy, vertex_count, flags);
  RestoreDepthStateForDraw(self, &snapshot);
  ZfixReleaseCopyBuffer(copy, heap_copy);
  g_model_depth_written_this_scene = 1;
  return hr;
}

static HRESULT DrawIndexedPrimitiveWithModelDepth(void* self, D3DDevice2DrawIndexedPrimitiveProc orig,
                                                  DWORD primitive_type, DWORD vertex_type, void* vertices,
                                                  DWORD vertex_count, WORD* indices, DWORD index_count, DWORD flags,
                                                  DWORD caller, const DrawBounds* bounds)
{
  (void)bounds;
  const ZfixCallsiteProfile* profile = NULL;
  DepthStateSnapshot snapshot;
  CaptureDepthState(self, &snapshot);
  ForceDepthStateForDraw(self);
  ForceModelQualityStateForDraw(self, &snapshot);

  D3DTLVERTEX_COMPAT stack_copy[ZFIX_STACK_VERTEX_CAPACITY];
  SIZE_T copy_bytes = 0;
  if (!ZfixCheckedSizeMul(sizeof(stack_copy[0]), (SIZE_T)vertex_count, &copy_bytes))
  {
    HRESULT hr = orig(self, primitive_type, vertex_type, vertices, vertex_count, indices, index_count, flags);
    RestoreDepthStateForDraw(self, &snapshot);
    return hr;
  }

  int heap_copy = 0;
  D3DTLVERTEX_COMPAT* copy =
    (D3DTLVERTEX_COMPAT*)ZfixAcquireCopyBuffer(copy_bytes, stack_copy, sizeof(stack_copy), &heap_copy);
  if (!copy)
  {
    HRESULT hr = orig(self, primitive_type, vertex_type, vertices, vertex_count, indices, index_count, flags);
    RestoreDepthStateForDraw(self, &snapshot);
    return hr;
  }

  memcpy(copy, vertices, copy_bytes);
  DrawBounds adjusted_bounds;
  if (!ComputeIndexedDrawBounds(copy, vertex_count, indices, index_count, &adjusted_bounds))
    ComputeDrawBounds(copy, vertex_count, &adjusted_bounds);
  profile = FindModelCallsiteProfileForDraw(caller, &adjusted_bounds);
  if (profile)
    LogCounterIncrement(&g_callsite_profile_hits);
  const DWORD adaptive_changed =
    ApplyAdaptiveDepthConflictResolver(copy, vertex_count, &adjusted_bounds,
                                       profile, caller, "opaque-dip");
  if (adaptive_changed &&
      !ComputeIndexedDrawBounds(copy, vertex_count, indices, index_count, &adjusted_bounds))
    ComputeDrawBounds(copy, vertex_count, &adjusted_bounds);
  LogZFightSample(caller, "opaque-dip", 1, primitive_type, vertex_count, index_count,
                  bounds, &adjusted_bounds,
                  adaptive_changed);

  RunIndexedModelDepthPrepass(self, orig, primitive_type, vertex_type, copy,
                              vertex_count, indices, index_count, flags);
  ForceDepthStateForDraw(self);
  ForceModelQualityStateForDraw(self, &snapshot);
  if (g_model_depth_prepass)
    ForceModelColorPassAfterPrepassState(self, profile);
  HRESULT hr = orig(self, primitive_type, vertex_type, copy, vertex_count, indices, index_count, flags);
  RestoreDepthStateForDraw(self, &snapshot);
  ZfixReleaseCopyBuffer(copy, heap_copy);
  g_model_depth_written_this_scene = 1;
  return hr;
}

static HRESULT DrawPrimitiveWithTransparentModelDepth(void* self, D3DDevice2DrawPrimitiveProc orig,
                                                      DWORD primitive_type, DWORD vertex_type, void* vertices,
                                                      DWORD vertex_count, DWORD flags, DWORD caller,
                                                      const DrawBounds* bounds, AlphaModelClass alpha_class)
{
  (void)bounds;
  const ZfixCallsiteProfile* profile = NULL;
  DepthStateSnapshot snapshot;
  CaptureDepthState(self, &snapshot);
  const int cutout = alpha_class == ALPHA_MODEL_CUTOUT;
  if (cutout)
  {
    ForceDepthStateForDraw(self);
    ForceModelQualityStateForDraw(self, &snapshot);
    ForceCutoutGeometryState(self, &snapshot);
  }
  else
  {
    ForceDepthStateForTransparentDraw(self);
    ForceModelQualityStateForDraw(self, &snapshot);
  }

  D3DTLVERTEX_COMPAT* copy = NULL;
  void* draw_vertices = vertices;
  D3DTLVERTEX_COMPAT stack_copy[ZFIX_STACK_VERTEX_CAPACITY];
  int heap_copy = 0;
  if (cutout)
  {
    SIZE_T copy_bytes = 0;
    if (ZfixCheckedSizeMul(sizeof(stack_copy[0]), (SIZE_T)vertex_count, &copy_bytes))
      copy = (D3DTLVERTEX_COMPAT*)ZfixAcquireCopyBuffer(copy_bytes, stack_copy, sizeof(stack_copy), &heap_copy);
    if (copy)
    {
      memcpy(copy, vertices, copy_bytes);
      DrawBounds adjusted_bounds;
      ComputeDrawBounds(copy, vertex_count, &adjusted_bounds);
      profile = FindModelCallsiteProfileForDraw(caller, &adjusted_bounds);
      if (profile)
        LogCounterIncrement(&g_callsite_profile_hits);
      const DWORD adaptive_changed =
        ApplyAdaptiveDepthConflictResolver(copy, vertex_count, &adjusted_bounds,
                                           profile, caller, "cutout-dp");
      if (adaptive_changed)
        ComputeDrawBounds(copy, vertex_count, &adjusted_bounds);
      draw_vertices = copy;
    }
  }

  if (cutout && copy)
  {
    RunModelDepthPrepass(self, orig, primitive_type, vertex_type, copy, vertex_count, flags);
    ForceDepthStateForDraw(self);
    ForceModelQualityStateForDraw(self, &snapshot);
    ForceCutoutGeometryState(self, &snapshot);
    if (g_model_depth_prepass)
      ForceModelColorPassAfterPrepassState(self, profile);
    ForceCutoutGeometryState(self, &snapshot);
  }

  HRESULT hr = orig(self, primitive_type, vertex_type, draw_vertices, vertex_count, flags);
  RestoreDepthStateForDraw(self, &snapshot);
  if (copy)
    ZfixReleaseCopyBuffer(copy, heap_copy);
  return hr;
}

static HRESULT DrawIndexedPrimitiveWithTransparentModelDepth(void* self, D3DDevice2DrawIndexedPrimitiveProc orig,
                                                            DWORD primitive_type, DWORD vertex_type, void* vertices,
                                                            DWORD vertex_count, WORD* indices, DWORD index_count,
                                                            DWORD flags, DWORD caller, const DrawBounds* bounds,
                                                            AlphaModelClass alpha_class)
{
  (void)bounds;
  const ZfixCallsiteProfile* profile = NULL;
  DepthStateSnapshot snapshot;
  CaptureDepthState(self, &snapshot);
  const int cutout = alpha_class == ALPHA_MODEL_CUTOUT;
  if (cutout)
  {
    ForceDepthStateForDraw(self);
    ForceModelQualityStateForDraw(self, &snapshot);
    ForceCutoutGeometryState(self, &snapshot);
  }
  else
  {
    ForceDepthStateForTransparentDraw(self);
    ForceModelQualityStateForDraw(self, &snapshot);
  }

  D3DTLVERTEX_COMPAT* copy = NULL;
  void* draw_vertices = vertices;
  D3DTLVERTEX_COMPAT stack_copy[ZFIX_STACK_VERTEX_CAPACITY];
  int heap_copy = 0;
  if (cutout)
  {
    SIZE_T copy_bytes = 0;
    if (ZfixCheckedSizeMul(sizeof(stack_copy[0]), (SIZE_T)vertex_count, &copy_bytes))
      copy = (D3DTLVERTEX_COMPAT*)ZfixAcquireCopyBuffer(copy_bytes, stack_copy, sizeof(stack_copy), &heap_copy);
    if (copy)
    {
      memcpy(copy, vertices, copy_bytes);
      DrawBounds adjusted_bounds;
      if (!ComputeIndexedDrawBounds(copy, vertex_count, indices, index_count, &adjusted_bounds))
        ComputeDrawBounds(copy, vertex_count, &adjusted_bounds);
      profile = FindModelCallsiteProfileForDraw(caller, &adjusted_bounds);
      if (profile)
        LogCounterIncrement(&g_callsite_profile_hits);
      const DWORD adaptive_changed =
        ApplyAdaptiveDepthConflictResolver(copy, vertex_count, &adjusted_bounds,
                                           profile, caller, "cutout-dip");
      if (adaptive_changed &&
          !ComputeIndexedDrawBounds(copy, vertex_count, indices, index_count, &adjusted_bounds))
        ComputeDrawBounds(copy, vertex_count, &adjusted_bounds);
      draw_vertices = copy;
    }
  }

  if (cutout && copy)
  {
    RunIndexedModelDepthPrepass(self, orig, primitive_type, vertex_type, copy,
                                vertex_count, indices, index_count, flags);
    ForceDepthStateForDraw(self);
    ForceModelQualityStateForDraw(self, &snapshot);
    ForceCutoutGeometryState(self, &snapshot);
    if (g_model_depth_prepass)
      ForceModelColorPassAfterPrepassState(self, profile);
    ForceCutoutGeometryState(self, &snapshot);
  }

  HRESULT hr = orig(self, primitive_type, vertex_type, draw_vertices, vertex_count, indices, index_count, flags);
  RestoreDepthStateForDraw(self, &snapshot);
  if (copy)
    ZfixReleaseCopyBuffer(copy, heap_copy);
  return hr;
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

static int IsZBufferSurfaceDesc(const DDSURFACEDESC_COMPAT* desc)
{
  if (!desc)
    return 0;
  if (desc->ddsCaps.dwCaps & DDSCAPS_ZBUFFER)
    return 1;
  if ((desc->dwFlags & DDSD_PIXELFORMAT) &&
      (desc->ddpfPixelFormat.dwFlags & DDPF_ZBUFFER))
    return 1;
  return 0;
}

// DirectDraw texture handle tracking.
static int IsTextureSurfaceDesc(const DDSURFACEDESC_COMPAT* desc)
{
  return desc && (desc->ddsCaps.dwCaps & DDSCAPS_TEXTURE);
}

static TextureSurfaceTrace* FindTextureSurface(void* surface)
{
  if (!surface)
    return NULL;

  LONG count = g_texture_surface_count;
  if (count > (LONG)ARRAYSIZE(g_texture_surfaces))
    count = (LONG)ARRAYSIZE(g_texture_surfaces);
  for (LONG i = 0; i < count; i++)
  {
    if (g_texture_surfaces[i].surface == surface)
      return &g_texture_surfaces[i];
  }
  return NULL;
}

static TextureSurfaceTrace* RememberTextureSurface(void* surface, const DDSURFACEDESC_COMPAT* desc)
{
  if (!surface || !desc)
    return NULL;

  TextureSurfaceTrace* existing = FindTextureSurface(surface);
  if (existing)
  {
    InterlockedIncrement(&existing->seen);
    return existing;
  }

  const LONG idx = InterlockedIncrement(&g_texture_surface_count) - 1;
  if (idx < 0 || idx >= (LONG)ARRAYSIZE(g_texture_surfaces))
    return NULL;

  TextureSurfaceTrace* entry = &g_texture_surfaces[idx];
  entry->surface = surface;
  entry->flags = desc->dwFlags;
  entry->caps = desc->ddsCaps.dwCaps;
  entry->width = (desc->dwFlags & DDSD_WIDTH) ? desc->dwWidth : 0;
  entry->height = (desc->dwFlags & DDSD_HEIGHT) ? desc->dwHeight : 0;
  entry->pitch = desc->dwPitchOrLinearSize;
  entry->seen = 1;
  return entry;
}

static TextureBindingTrace* FindTextureBinding(void* texture)
{
  if (!texture)
    return NULL;

  LONG count = g_texture_binding_count;
  if (count > (LONG)ARRAYSIZE(g_texture_bindings))
    count = (LONG)ARRAYSIZE(g_texture_bindings);
  for (LONG i = 0; i < count; i++)
  {
    if (g_texture_bindings[i].texture == texture)
      return &g_texture_bindings[i];
  }
  return NULL;
}

static TextureHandleTrace* FindTextureHandleTraceByHandle(DWORD handle)
{
  TextureHandleTrace* fallback = NULL;
  LONG count = g_texture_handle_count;
  if (count > (LONG)ARRAYSIZE(g_texture_handles))
    count = (LONG)ARRAYSIZE(g_texture_handles);
  for (LONG i = count - 1; i >= 0; i--)
  {
    TextureHandleTrace* entry = &g_texture_handles[i];
    if (entry->handle != handle)
      continue;
    fallback = entry;
    if (entry->width || entry->height)
      return entry;
  }
  return fallback;
}

static void RefreshTextureHandleTrace(TextureHandleTrace* entry, void* texture,
                                      TextureSurfaceTrace* surface, DWORD handle)
{
  if (!entry)
    return;

  entry->texture = texture;
  entry->handle = handle;
  if (!surface)
    return;

  entry->surface = surface->surface;
  entry->width = surface->width;
  entry->height = surface->height;
  entry->caps = surface->caps;
}

static void TraceTextureQueryInterface(void* source, void* texture)
{
  if (!g_texture_handle_trace || !source || !texture)
    return;

  InterlockedIncrement(&g_texture_qi_seen);
  TextureSurfaceTrace* surface = FindTextureSurface(source);
  if (!surface)
    return;

  TextureBindingTrace* existing = FindTextureBinding(texture);
  if (existing)
  {
    InterlockedIncrement(&existing->seen);
    return;
  }

  const LONG idx = InterlockedIncrement(&g_texture_binding_count) - 1;
  if (idx < 0 || idx >= (LONG)ARRAYSIZE(g_texture_bindings))
    return;

  TextureBindingTrace* binding = &g_texture_bindings[idx];
  binding->texture = texture;
  binding->surface = source;
  binding->seen = 1;

  const LONG logged = InterlockedIncrement(&g_texture_binding_logged);
  if (logged > 128)
    return;

  LogLine("texture-qi #%ld slot=%ld surface=%p texture=%p wh=%lux%lu caps=0x%08lX",
          logged, idx, source, texture, surface->width, surface->height,
          surface->caps);
}

static void TraceCreatedSurface(void* surface, const DDSURFACEDESC_COMPAT* desc, HRESULT hr)
{
  if (SUCCEEDED(hr) && surface)
  {
    PatchVTableSlot(surface, 0, (void*)Hook_QueryInterface);
  }

  if (!g_texture_handle_trace || !SUCCEEDED(hr) || !surface || !IsTextureSurfaceDesc(desc))
    return;

  TextureSurfaceTrace* entry = RememberTextureSurface(surface, desc);
  const LONG seen = InterlockedIncrement(&g_texture_surface_seen);
  const LONG logged = InterlockedIncrement(&g_texture_surface_logged);
  if (logged > 96)
    return;

  LogLine("texture-surface #%ld seen=%ld slot=%ld surface=%p flags=0x%08lX caps=0x%08lX "
          "wh=%lux%lu pitch=%ld",
          logged, seen, entry ? (LONG)(entry - g_texture_surfaces) : -1L,
          surface, desc->dwFlags, desc->ddsCaps.dwCaps,
          entry ? entry->width : 0, entry ? entry->height : 0,
          (LONG)(entry ? entry->pitch : desc->dwPitchOrLinearSize));
}

static void TraceTextureHandle(void* texture, void* device, DWORD handle)
{
  if (!g_texture_handle_trace)
    return;

  InterlockedIncrement(&g_texture_handle_seen);
  TextureBindingTrace* binding = FindTextureBinding(texture);
  TextureSurfaceTrace* surface = binding ? FindTextureSurface(binding->surface) : NULL;
  LONG count = g_texture_handle_count;
  if (count > (LONG)ARRAYSIZE(g_texture_handles))
    count = (LONG)ARRAYSIZE(g_texture_handles);
  for (LONG i = 0; i < count; i++)
  {
    TextureHandleTrace* entry = &g_texture_handles[i];
    if (entry->texture == texture && entry->handle == handle)
    {
      RefreshTextureHandleTrace(entry, texture, surface, handle);
      InterlockedIncrement(&entry->seen);
      return;
    }
  }

  const LONG idx = InterlockedIncrement(&g_texture_handle_count) - 1;
  if (idx >= 0 && idx < (LONG)ARRAYSIZE(g_texture_handles))
  {
    RefreshTextureHandleTrace(&g_texture_handles[idx], texture, surface, handle);
    g_texture_handles[idx].seen = 1;
  }

  const LONG logged = InterlockedIncrement(&g_texture_handle_logged);
  if (logged > 128)
    return;

  LogLine("texture-handle #%ld slot=%ld texture=%p surface=%p device=%p handle=0x%08lX "
          "wh=%lux%lu caps=0x%08lX",
          logged, idx, texture, surface ? surface->surface : NULL, device, handle,
          surface ? surface->width : 0, surface ? surface->height : 0,
          surface ? surface->caps : 0);
}

static DWORD RequestedZBufferDepth(const DDSURFACEDESC_COMPAT* desc)
{
  if (!desc)
    return 0;
  if ((desc->dwFlags & DDSD_ZBUFFERBITDEPTH) && desc->dwZBufferBitDepth)
    return desc->dwZBufferBitDepth;
  if ((desc->dwFlags & DDSD_PIXELFORMAT) &&
      (desc->ddpfPixelFormat.dwFlags & DDPF_ZBUFFER) &&
      desc->ddpfPixelFormat.dwZBufferBitDepth)
    return desc->ddpfPixelFormat.dwZBufferBitDepth;
  return 0;
}

static DWORD ZBufferMaskForDepth(DWORD depth)
{
  if (depth >= 32)
    return 0xFFFFFFFFu;
  if (depth >= 24)
    return 0x00FFFFFFu;
  if (depth >= 16)
    return 0x0000FFFFu;
  return 0;
}

static void ApplyZBufferDepth(DDSURFACEDESC_COMPAT* desc, DWORD depth, int use_pixel_format)
{
  if (!desc || depth == 0)
    return;

  desc->dwFlags |= DDSD_ZBUFFERBITDEPTH;
  desc->dwZBufferBitDepth = depth;
  if (!use_pixel_format)
  {
    desc->dwFlags &= ~DDSD_PIXELFORMAT;
    return;
  }

  desc->dwFlags |= DDSD_PIXELFORMAT;
  desc->ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT_COMPAT);
  desc->ddpfPixelFormat.dwFlags = DDPF_ZBUFFER;
  desc->ddpfPixelFormat.dwFourCC = 0;
  desc->ddpfPixelFormat.dwZBufferBitDepth = depth;
  desc->ddpfPixelFormat.dwStencilBitDepth = 0;
  desc->ddpfPixelFormat.dwZBitMask = ZBufferMaskForDepth(depth);
  desc->ddpfPixelFormat.dwStencilBitMask = 0;
  desc->ddpfPixelFormat.dwRGBZBitMask = desc->ddpfPixelFormat.dwZBitMask;
}

static HRESULT TryCreateZBufferSurface(void* self, DirectDrawCreateSurfaceProc orig,
                                       const DDSURFACEDESC_COMPAT* original_desc, void** surface,
                                       void* outer, DWORD depth, int use_pixel_format)
{
  DDSURFACEDESC_COMPAT attempt = *original_desc;
  ApplyZBufferDepth(&attempt, depth, use_pixel_format);
  if (surface)
    *surface = NULL;

  return orig(self, &attempt, surface, outer);
}

static HRESULT STDMETHODCALLTYPE Hook_DD_CreateSurface(void* self, DDSURFACEDESC_COMPAT* desc,
                                                       void** surface, void* outer)
{
  DirectDrawCreateSurfaceProc orig = (DirectDrawCreateSurfaceProc)GetOriginal(*(void***)self, 6);
  if (!orig)
    return E_FAIL;

  if (!g_enabled || !g_upgrade_zbuffer_format || !IsZBufferSurfaceDesc(desc))
  {
    HRESULT hr = orig(self, desc, surface, outer);
    TraceCreatedSurface(surface ? *surface : NULL, desc, hr);
    return hr;
  }

  const DWORD original_depth = RequestedZBufferDepth(desc);
  DWORD first_depth = (DWORD)g_preferred_zbuffer_depth;
  DWORD second_depth = (DWORD)g_fallback_zbuffer_depth;
  const int has_pixel_format =
    (desc->dwFlags & DDSD_PIXELFORMAT) && (desc->ddpfPixelFormat.dwFlags & DDPF_ZBUFFER);

  if (original_depth > first_depth)
    first_depth = original_depth;
  if (first_depth == original_depth && has_pixel_format)
  {
    HRESULT hr = orig(self, desc, surface, outer);
    TraceCreatedSurface(surface ? *surface : NULL, desc, hr);
    return hr;
  }

  HRESULT hr = TryCreateZBufferSurface(self, orig, desc, surface, outer, first_depth,
                                       g_upgrade_zbuffer_pixel_format);
  if (SUCCEEDED(hr))
  {
    TraceCreatedSurface(surface ? *surface : NULL, desc, hr);
    return hr;
  }

  if (g_zbuffer_upgrade_variants && g_upgrade_zbuffer_pixel_format)
  {
    hr = TryCreateZBufferSurface(self, orig, desc, surface, outer, first_depth, 0);
    if (SUCCEEDED(hr))
    {
      TraceCreatedSurface(surface ? *surface : NULL, desc, hr);
      return hr;
    }
  }

  if (second_depth && second_depth != first_depth && second_depth > original_depth)
  {
    hr = TryCreateZBufferSurface(self, orig, desc, surface, outer, second_depth,
                                 g_upgrade_zbuffer_pixel_format);
    if (SUCCEEDED(hr))
    {
      TraceCreatedSurface(surface ? *surface : NULL, desc, hr);
      return hr;
    }

    if (g_zbuffer_upgrade_variants && g_upgrade_zbuffer_pixel_format)
    {
      hr = TryCreateZBufferSurface(self, orig, desc, surface, outer, second_depth, 0);
      if (SUCCEEDED(hr))
      {
        TraceCreatedSurface(surface ? *surface : NULL, desc, hr);
        return hr;
      }
    }
  }

  if (surface)
    *surface = NULL;
  hr = orig(self, desc, surface, outer);
  TraceCreatedSurface(surface ? *surface : NULL, desc, hr);
  return hr;
}

static void ClassifyAndPatch(void* obj, REFIID riid)
{
  if (!obj || !riid)
    return;

  PatchVTableSlot(obj, 0, (void*)Hook_QueryInterface);

  if (ZfixIsGuid(riid, &kIID_IDirect3D2))
  {
    PatchVTableSlot(obj, 8, (void*)Hook_D3D2_CreateDevice);
  }
  else if (ZfixIsGuid(riid, &kIID_IDirect3DDevice2))
  {
    PatchD3DDevice2(obj);
  }
  else if (ZfixIsGuid(riid, &kIID_IDirect3DTexture2))
  {
    PatchVTableSlot(obj, 3, (void*)Hook_D3DTexture2_GetHandle);
  }
  else if (ZfixIsGuid(riid, &kIID_IDirectDraw2))
  {
    PatchVTableSlot(obj, 6, (void*)Hook_DD_CreateSurface);
  }
}

static HRESULT STDMETHODCALLTYPE Hook_QueryInterface(void* self, REFIID riid, void** ppvObj)
{
  QueryInterfaceProc orig = (QueryInterfaceProc)GetOriginal(*(void***)self, 0);
  if (!orig)
    return E_NOINTERFACE;

  HRESULT hr = orig(self, riid, ppvObj);
  if (SUCCEEDED(hr) && ppvObj && *ppvObj)
  {
    if (ZfixIsGuid(riid, &kIID_IDirect3DTexture2))
      TraceTextureQueryInterface(self, *ppvObj);
    ClassifyAndPatch(*ppvObj, riid);
  }

  return hr;
}

static HRESULT STDMETHODCALLTYPE Hook_D3DTexture2_GetHandle(void* self, void* device, DWORD* handle)
{
  D3DTexture2GetHandleProc orig = (D3DTexture2GetHandleProc)GetOriginal(*(void***)self, 3);
  if (!orig)
    return E_FAIL;

  HRESULT hr = orig(self, device, handle);
  if (SUCCEEDED(hr) && handle)
    TraceTextureHandle(self, device, *handle);
  return hr;
}

static HRESULT STDMETHODCALLTYPE Hook_D3DDevice2_BeginScene(void* self)
{
  g_model_depth_written_this_scene = 0;
  ClearDepthBufferForScene(self);
  D3DDevice2BeginSceneProc orig = (D3DDevice2BeginSceneProc)GetOriginal(*(void***)self, 10);
  return orig ? orig(self) : D3D_OK;
}

static HRESULT STDMETHODCALLTYPE Hook_D3DDevice2_EndScene(void* self)
{
  if (g_log_enabled)
  {
    const LONG scene = InterlockedIncrement(&g_scene_counter);
    if (scene <= 8 || (scene % 120) == 0)
    {
      LogLine("scene=%ld total=%ld acceptDP=%ld acceptDIP=%ld transparent=%ld cutout=%ld "
              "rejected=%ld zsamples=%ld prepass=%ld/%ld",
              scene, g_draw_total, g_draw_model_accepted, g_draw_indexed_model_accepted,
              g_draw_transparent_accepted, g_draw_cutout_accepted, g_draw_rejected,
              g_zfight_sample_logged, g_model_depth_prepass_draws,
              g_model_depth_prepass_failures);
    }
    if (scene == 60 || scene == 240 || (scene % 720) == 0)
      LogDrawCallsiteSummary("scene");
  }

  D3DDevice2EndSceneProc orig = (D3DDevice2EndSceneProc)GetOriginal(*(void***)self, 11);
  return orig ? orig(self) : D3D_OK;
}

static HRESULT STDMETHODCALLTYPE Hook_D3DDevice2_SetRenderState(void* self, DWORD state, DWORD value)
{
  TrackRenderState(state, value);
  D3DDevice2SetRenderStateProc orig = (D3DDevice2SetRenderStateProc)GetOriginal(*(void***)self, 23);
  return orig ? orig(self, state, value) : D3D_OK;
}

static HRESULT STDMETHODCALLTYPE Hook_D3DDevice2_DrawPrimitive(void* self, DWORD primitive_type, DWORD vertex_type,
                                                              void* vertices, DWORD vertex_count, DWORD flags)
{
  D3DDevice2DrawPrimitiveProc orig = (D3DDevice2DrawPrimitiveProc)GetOriginal(*(void***)self, 29);
  if (!orig)
    return D3D_OK;
  const DWORD caller = (DWORD)(uintptr_t)_ReturnAddress();
  LogCounterIncrement(&g_draw_total);

  if (!g_enabled || vertex_type != D3DVT_TLVERTEX || !vertices || vertex_count == 0)
  {
    ClearModelDepthBeforeKnown2D(self, caller, vertex_type, vertices, vertex_count, NULL, 0);
    return orig(self, primitive_type, vertex_type, vertices, vertex_count, flags);
  }

  const DWORD tris = TriangleCount(primitive_type, vertex_count);
  DrawBounds mask_bounds;
  ZfixTexturePageClass mask_texture_class = ZFIX_TEXTURE_PAGE_NONE;
  DWORD mask_texture = 0;
  if (IsLikelyD3D2MaskOverlayDraw((const D3DTLVERTEX_COMPAT*)vertices, vertex_count,
                                  primitive_type, NULL, 0, tris, &mask_bounds,
                                  &mask_texture_class, &mask_texture))
  {
    LogCounterIncrement(&g_draw_rejected);
    TrackDrawCallsite(caller, "reject", 0, 0, primitive_type, vertex_count, 0, tris,
                      mask_texture_class == ZFIX_TEXTURE_PAGE_HIRES ?
                      "mask_overlay_hires" : "mask_overlay_classic",
                      &mask_bounds);
    LogD3D2MaskOverlaySkip(caller, 0, primitive_type, vertex_count, 0, tris,
                           mask_texture, mask_texture_class, &mask_bounds);
    ClearModelDepthBeforeKnown2D(self, caller, vertex_type, vertices, vertex_count, NULL, 0);
    return orig(self, primitive_type, vertex_type, vertices, vertex_count, flags);
  }

  DrawBounds bounds;
  const char* reason = NULL;
  const int model_ok = IsModelDepthDraw((const D3DTLVERTEX_COMPAT*)vertices, primitive_type, vertex_count, tris,
                                        0, caller, &bounds, &reason);
  if (!model_ok)
  {
    DrawBounds transparent_bounds;
    int transparent_ok = 0;
    const int transparent_candidate = reason && strcmp(reason, "vertex_alpha") == 0;
    if (transparent_candidate)
    {
      transparent_ok =
        IsTransparentModelDepthDraw((const D3DTLVERTEX_COMPAT*)vertices, primitive_type, vertex_count,
                                    tris, 0, caller, &transparent_bounds, NULL);
    }
    if (transparent_ok)
    {
      const int min_alpha = MinVertexAlpha((const D3DTLVERTEX_COMPAT*)vertices, vertex_count);
      const AlphaModelClass alpha_class =
        ClassifyAlphaModel((const D3DTLVERTEX_COMPAT*)vertices, vertex_count, min_alpha);
      if (alpha_class == ALPHA_MODEL_CUTOUT)
        LogCounterIncrement(&g_draw_cutout_accepted);
      else
        LogCounterIncrement(&g_draw_transparent_accepted);
      TrackDrawCallsite(caller, "accept", 0, 1, primitive_type, vertex_count, 0, tris,
                        alpha_class == ALPHA_MODEL_CUTOUT ? "cutout" : "transparent",
                        &transparent_bounds);
      return DrawPrimitiveWithTransparentModelDepth(self, orig, primitive_type, vertex_type, vertices,
                                                   vertex_count, flags, caller, &transparent_bounds,
                                                   alpha_class);
    }
    ComputeDrawBounds((const D3DTLVERTEX_COMPAT*)vertices, vertex_count, &bounds);
    LogCounterIncrement(&g_draw_rejected);
    TrackDrawCallsite(caller, "reject", 0, 0, primitive_type, vertex_count, 0, tris,
                      reason, &bounds);
    ClearModelDepthBeforeKnown2D(self, caller, vertex_type, vertices, vertex_count, NULL, 0);
    return orig(self, primitive_type, vertex_type, vertices, vertex_count, flags);
  }

  LogCounterIncrement(&g_draw_model_accepted);
  TrackDrawCallsite(caller, "accept", 0, 0, primitive_type, vertex_count, 0, tris,
                    "ok", &bounds);
  return DrawPrimitiveWithModelDepth(self, orig, primitive_type, vertex_type, vertices, vertex_count, flags,
                                     caller, tris, &bounds);
}

static HRESULT STDMETHODCALLTYPE Hook_D3DDevice2_DrawIndexedPrimitive(void* self, DWORD primitive_type,
                                                                     DWORD vertex_type, void* vertices,
                                                                     DWORD vertex_count, WORD* indices,
                                                                     DWORD index_count, DWORD flags)
{
  D3DDevice2DrawIndexedPrimitiveProc orig = (D3DDevice2DrawIndexedPrimitiveProc)GetOriginal(*(void***)self, 30);
  if (!orig)
    return D3D_OK;
  const DWORD caller = (DWORD)(uintptr_t)_ReturnAddress();
  LogCounterIncrement(&g_draw_total);

  if (!g_enabled || vertex_type != D3DVT_TLVERTEX || !vertices || !indices || vertex_count == 0 || index_count == 0)
  {
    ClearModelDepthBeforeKnown2D(self, caller, vertex_type, vertices, vertex_count, indices, index_count);
    return orig(self, primitive_type, vertex_type, vertices, vertex_count, indices, index_count, flags);
  }

  const DWORD tris = TriangleCount(primitive_type, index_count);
  DrawBounds mask_bounds;
  ZfixTexturePageClass mask_texture_class = ZFIX_TEXTURE_PAGE_NONE;
  DWORD mask_texture = 0;
  if (IsLikelyD3D2MaskOverlayDraw((const D3DTLVERTEX_COMPAT*)vertices, vertex_count,
                                  primitive_type, indices, index_count, tris,
                                  &mask_bounds, &mask_texture_class, &mask_texture))
  {
    LogCounterIncrement(&g_draw_rejected);
    TrackDrawCallsite(caller, "reject", 1, 0, primitive_type, vertex_count,
                      index_count, tris,
                      mask_texture_class == ZFIX_TEXTURE_PAGE_HIRES ?
                      "mask_overlay_hires" : "mask_overlay_classic",
                      &mask_bounds);
    LogD3D2MaskOverlaySkip(caller, 1, primitive_type, vertex_count, index_count, tris,
                           mask_texture, mask_texture_class, &mask_bounds);
    ClearModelDepthBeforeKnown2D(self, caller, vertex_type, vertices, vertex_count, indices, index_count);
    return orig(self, primitive_type, vertex_type, vertices, vertex_count, indices, index_count, flags);
  }

  DrawBounds bounds;
  const char* reason = NULL;
  const int indices_ok = IndicesAreValid(indices, index_count, vertex_count);
  const int model_ok = indices_ok &&
                       IsIndexedModelDepthDraw((const D3DTLVERTEX_COMPAT*)vertices, primitive_type, vertex_count,
                                               indices, index_count, tris, caller, &bounds, &reason);
  if (!model_ok)
  {
    DrawBounds transparent_bounds;
    int transparent_ok = 0;
    const int transparent_candidate = indices_ok && reason && strcmp(reason, "vertex_alpha") == 0;
    if (transparent_candidate)
    {
      transparent_ok =
        IsTransparentIndexedModelDepthDraw((const D3DTLVERTEX_COMPAT*)vertices, primitive_type, vertex_count,
                                           indices, index_count, tris, caller, &transparent_bounds,
                                           NULL);
    }
    if (transparent_ok)
    {
      const int min_alpha =
        MinIndexedVertexAlpha((const D3DTLVERTEX_COMPAT*)vertices, vertex_count, indices, index_count);
      const AlphaModelClass alpha_class =
        ClassifyIndexedAlphaModel((const D3DTLVERTEX_COMPAT*)vertices, vertex_count,
                                  indices, index_count, min_alpha);
      if (alpha_class == ALPHA_MODEL_CUTOUT)
        LogCounterIncrement(&g_draw_cutout_accepted);
      else
        LogCounterIncrement(&g_draw_transparent_accepted);
      TrackDrawCallsite(caller, "accept", 1, 1, primitive_type, vertex_count, index_count, tris,
                        alpha_class == ALPHA_MODEL_CUTOUT ? "cutout" : "transparent",
                        &transparent_bounds);
      return DrawIndexedPrimitiveWithTransparentModelDepth(self, orig, primitive_type, vertex_type, vertices,
                                                          vertex_count, indices, index_count, flags,
                                                          caller, &transparent_bounds, alpha_class);
    }
    int bounds_ok = 0;
    if (indices_ok && ComputeIndexedDrawBounds((const D3DTLVERTEX_COMPAT*)vertices, vertex_count,
                                                indices, index_count, &bounds))
    {
      bounds_ok = 1;
    }
    else
    {
      ComputeDrawBounds((const D3DTLVERTEX_COMPAT*)vertices, vertex_count, &bounds);
    }

    LogCounterIncrement(&g_draw_rejected);
    if (bounds_ok)
    {
      TrackDrawCallsite(caller, "reject", 1, 0, primitive_type, vertex_count, index_count, tris,
                        reason, &bounds);
    }
    else
    {
      TrackDrawCallsite(caller, "reject", 1, 0, primitive_type, vertex_count, index_count, tris,
                        reason ? reason : "bad_indices", &bounds);
    }
    ClearModelDepthBeforeKnown2D(self, caller, vertex_type, vertices, vertex_count, indices, index_count);
    return orig(self, primitive_type, vertex_type, vertices, vertex_count, indices, index_count, flags);
  }

  LogCounterIncrement(&g_draw_indexed_model_accepted);
  TrackDrawCallsite(caller, "accept", 1, 0, primitive_type, vertex_count, index_count, tris,
                    "ok", &bounds);
  return DrawIndexedPrimitiveWithModelDepth(self, orig, primitive_type, vertex_type, vertices, vertex_count,
                                            indices, index_count, flags, caller, &bounds);
}

// Vtable and import patching.
static void PatchD3DDevice2(void* device)
{
  if (!device)
    return;

  PatchVTableSlot(device, 0, (void*)Hook_QueryInterface);
  PatchVTableSlot(device, 10, (void*)Hook_D3DDevice2_BeginScene);
  PatchVTableSlot(device, 11, (void*)Hook_D3DDevice2_EndScene);
  PatchVTableSlot(device, 23, (void*)Hook_D3DDevice2_SetRenderState);
  PatchVTableSlot(device, 29, (void*)Hook_D3DDevice2_DrawPrimitive);
  PatchVTableSlot(device, 30, (void*)Hook_D3DDevice2_DrawIndexedPrimitive);
}

static HRESULT STDMETHODCALLTYPE Hook_D3D2_CreateDevice(void* self, REFCLSID rclsid, void* surface, void** device)
{
  D3D2CreateDeviceProc orig = (D3D2CreateDeviceProc)GetOriginal(*(void***)self, 8);
  if (!orig)
    return E_FAIL;

  HRESULT hr = orig(self, rclsid, surface, device);
  if (SUCCEEDED(hr) && device && *device)
    PatchD3DDevice2(*device);
  return hr;
}

static HRESULT WINAPI Hook_DirectDrawCreate(GUID* lpGUID, void** lplpDD, void* pUnkOuter)
{
  if (!g_real_direct_draw_create)
  {
    HMODULE ddraw = GetModuleHandleA("DDRAW.dll");
    if (!ddraw)
      ddraw = LoadLibraryA("DDRAW.dll");
    g_real_direct_draw_create = ddraw ? (DirectDrawCreateProc)GetProcAddress(ddraw, "DirectDrawCreate") : NULL;
  }

  if (!g_real_direct_draw_create)
    return E_FAIL;

  HRESULT hr = g_real_direct_draw_create(lpGUID, lplpDD, pUnkOuter);
  if (SUCCEEDED(hr) && lplpDD && *lplpDD)
  {
    PatchVTableSlot(*lplpDD, 0, (void*)Hook_QueryInterface);
    PatchVTableSlot(*lplpDD, 6, (void*)Hook_DD_CreateSurface);
  }
  return hr;
}

static int PatchLoadedModuleImports(HMODULE module)
{
  return ZfixPatchModuleImport(module, "DDRAW.dll",
                               "DirectDrawCreate", (void*)Hook_DirectDrawCreate,
                               (void**)&g_real_direct_draw_create);
}

static int PatchAllImports(void)
{
  return ZfixPatchLoadedModules(PatchLoadedModuleImports);
}

static void LogDelayedImportPatch(DWORD pass, int patched)
{
  LogLine("delayed DirectDrawCreate imports pass=%lu patched=%d", pass, patched);
}

static DWORD WINAPI PatchImportsWorker(LPVOID param)
{
  (void)param;
  ZfixRunDelayedImportPatches(PatchAllImports, ZFIX_IMPORT_REPATCH_PASSES,
                              ZFIX_IMPORT_REPATCH_DELAY_MS, LogDelayedImportPatch);
  return 0;
}

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
  (void)reserved;
  if (reason == DLL_PROCESS_ATTACH)
  {
    g_module = instance;
    BuildLogPath(instance);
    g_log_enabled = ZfixLogEnabledByMarker(g_log_path);
    if (g_log_enabled)
      DeleteFileA(g_log_path);
    DisableThreadLibraryCalls(instance);
    for (DWORD i = 0; i < ARRAYSIZE(g_render_state_cache); i++)
      g_render_state_cache[i] = 0xFFFFFFFFu;
    LogLine("re2_zfix loaded log=%d diagnostics=%d model_callsite=0x%08lX..0x%08lX",
            g_log_enabled, g_diagnostics, g_model_callsite_min, g_model_callsite_max);
    LogLine("depth zbuffer=%d/%d clearEachScene=%d prepass=%d colorZWrite=%d colorZFunc=%d alphaRef=%lu",
            g_preferred_zbuffer_depth, g_fallback_zbuffer_depth, g_clear_depth_each_scene,
            g_model_depth_prepass, g_model_color_pass_z_write, g_model_color_pass_z_func,
            g_model_alpha_ref);
    LogLine("profiles enabled=%d count=%lu adaptiveDepth=%d flatSpan=%.8f targetSpan=%.8f "
            "maxSpan=%.8f maxShift=%.8f area=%.1f..%.1f extent=%.1f..%.1f "
            "small=%.1f/%.1f strength=%.2f crowBoost=%.8f->%.8f/%.8f->%.8f snap=%d",
            g_callsite_profiles_enabled, (DWORD)ARRAYSIZE(g_callsite_profiles),
            g_adaptive_depth_conflict_resolver, g_adaptive_depth_flat_span,
            g_adaptive_depth_target_span, g_adaptive_depth_max_span,
            g_adaptive_depth_max_shift, g_adaptive_depth_min_area,
            g_adaptive_depth_max_area, g_adaptive_depth_min_extent,
            g_adaptive_depth_max_extent, g_adaptive_depth_small_min_area,
            g_adaptive_depth_small_min_extent, g_adaptive_depth_small_strength,
            g_crow_profile_flat_boost_z_span, g_crow_profile_flat_boost_target_span,
            g_crow_profile_mid_boost_z_span, g_crow_profile_mid_boost_target_span,
            g_crow_profile_precision_snap);
    LogLine("transparent_model zTest=%d zWrite=%d zFunc=%d zBias=%d",
            g_transparent_model_z_test, g_transparent_model_z_write, g_transparent_model_z_func,
            g_transparent_model_z_bias);
    LogLine("alpha classify cutoutMin=%d hardAlpha=%d/%d cutoutDisableBlend=%d twoSided=%d "
            "alphaRef=%lu",
            g_cutout_alpha_min, g_cutout_hard_alpha_low_max,
            g_cutout_hard_alpha_high_min, g_cutout_model_disable_alpha_blend,
            g_cutout_model_two_sided, g_cutout_model_alpha_ref);
    LogLine("model_quality perspective=%d alphaTest=%d alphaRef=%lu alphaFunc=%lu "
            "gouraud=%d dither=%d subpixel=%d",
            g_model_texture_perspective, g_model_alpha_test, g_model_alpha_ref, g_model_alpha_func,
            g_model_gouraud_shading, g_model_dither, g_model_subpixel);
    LogLine("re2 mask overlay guard=%d hiresMin=%lu classic=%lu..%lu flatZ=%.6f flatRhw=%.6f minRhw=%.2f",
            g_re2_mask_overlay_guard, g_re2_hires_mask_texture_min_side,
            g_re2_classic_mask_texture_min_side, g_re2_classic_mask_texture_max_side,
            g_re2_mask_overlay_flat_z_span, g_re2_mask_overlay_flat_rhw_span,
            g_re2_mask_overlay_min_rhw);
    LogLine("patch DirectDrawCreate imports=%d", PatchAllImports());
    ZfixStartDetachedThread(PatchImportsWorker, NULL);
  }
  else if (reason == DLL_PROCESS_DETACH)
  {
    LogDrawCallsiteSummary("detach");
  }
  return TRUE;
}
