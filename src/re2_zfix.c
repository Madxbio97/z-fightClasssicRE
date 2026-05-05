#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <intrin.h>
#include <tlhelp32.h>

#if defined(_MSC_VER)
#pragma intrinsic(_ReturnAddress)
#endif

#ifndef ARRAYSIZE
#define ARRAYSIZE(a) (sizeof(a) / sizeof((a)[0]))
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
#define D3DRENDERSTATE_MIPMAPLODBIAS 46
#define D3DRENDERSTATE_RANGEFOGENABLE 48
#define D3DRENDERSTATE_ANISOTROPY 49
#define D3DCMP_EQUAL 3
#define D3DCMP_LESSEQUAL 4
#define D3DCMP_GREATER 5
#define D3DCMP_ALWAYS 8
#define D3DCLEAR_ZBUFFER 0x00000002l
#define ZFIX_SKIP_STATE (-1)
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
#define D3DTFG_POINT 1
#define D3DTFG_LINEAR 2
#define D3DTFN_POINT 1
#define D3DTFN_LINEAR 2
#define D3DBLEND_ZERO 1
#define D3DBLEND_ONE 2

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
typedef ULONG(STDMETHODCALLTYPE* ComReleaseProc)(void* self);
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
typedef struct HookEntry {
  void** vtable;
  int slot;
  void* original;
} HookEntry;

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

typedef struct DepthRangeMap {
  float min_z;
  float max_z;
  int valid;
} DepthRangeMap;

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
static volatile LONG g_draw_total = 0;
static volatile LONG g_draw_model_accepted = 0;
static volatile LONG g_draw_indexed_model_accepted = 0;
static volatile LONG g_draw_transparent_accepted = 0;
static volatile LONG g_draw_rejected = 0;
static volatile LONG g_zfight_sample_logged = 0;
static volatile LONG g_lower_probe_logged = 0;
static volatile LONG g_scene_counter = 0;
static volatile LONG g_crow_wing_fix_seen = 0;
static volatile LONG g_crow_wing_fix_changed = 0;
static volatile LONG g_crow_wing_fix_logged = 0;
static volatile LONG g_model_uv_corrected_draws = 0;
static volatile LONG g_model_uv_corrected_coords = 0;
static volatile LONG g_model_uv_correction_logged = 0;
static volatile LONG g_model_depth_prepass_draws = 0;
static volatile LONG g_model_depth_prepass_failures = 0;
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

static const int g_enabled = 1;
static const int g_diagnostics = 1;
static const int g_callsite_diagnostics = 1;
static const int g_lower_level_probe = 0;
static const int g_precise_model_depth_pass = 1;
static const int g_model_depth_normalize = 0;
static const int g_model_depth_centered = 1;
static const int g_force_z_enable = 1;
static const int g_force_z_write = 1;
static const int g_force_z_func = D3DCMP_LESSEQUAL;
static const int g_force_z_bias = 0;
static const int g_model_depth_prepass = 1;
static const int g_model_color_pass_z_write = 0;
static const int g_model_color_pass_z_func = D3DCMP_EQUAL;
static const int g_restore_depth_state = 1;
static const int g_clear_depth_each_scene = 1;
static const int g_skip_axis_tile_draws = 1;
static const int g_precise_min_vertex_alpha = 250;
static const int g_single_triangle_dp_only = 1;
static const int g_allow_indexed_model_draws = 1;
static const int g_require_model_callsite = 1;
static const int g_use_previous_depth_range = 0;
static const int g_clear_model_depth_before_2d = 1;
static const int g_close_model_depth_softening = 0;
static const int g_transparent_model_z_test = 1;
static const int g_transparent_model_depth_adjust = 0;
static const int g_transparent_model_z_write = 0;
static const int g_transparent_model_z_func = D3DCMP_LESSEQUAL;
static const int g_transparent_model_z_bias = ZFIX_SKIP_STATE;
static const int g_model_texture_perspective = 1;
static const int g_model_alpha_test = 1;
static const DWORD g_model_alpha_ref = 8u;
static const DWORD g_model_alpha_func = D3DCMP_GREATER;
static const int g_model_uv_correction = 1;
static const int g_model_gouraud_shading = 1;
static const int g_model_dither = 1;
static const int g_model_subpixel = 1;
static const int g_upgrade_zbuffer_format = 1;
static const int g_upgrade_zbuffer_pixel_format = 1;
static const int g_zbuffer_upgrade_variants = 1;
static const int g_preferred_zbuffer_depth = 32;
static const int g_fallback_zbuffer_depth = 24;
static const int g_texture_handle_trace = 1;
static const int g_crow_wing_depth_fix = 1;

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
static const float g_model_depth_near = 0.12f;
static const float g_model_depth_far = 0.88f;
static const float g_model_depth_min_span = 0.00005f;
static const float g_model_depth_bias = 0.0f;
static const float g_model_depth_range_padding = 0.020f;
static const float g_model_depth_range_floor = 0.12f;
static const float g_model_depth_blend = 0.34f;
static const float g_model_depth_max_expansion = 1.78f;
static const float g_model_depth_centered_scale = 1.78f;
static const float g_model_depth_centered_min = 0.0001f;
static const float g_model_depth_centered_max = 0.9990f;
static const float g_previous_depth_center_tolerance = 0.10f;
static const float g_close_model_rhw_start = 0.0010f;
static const float g_close_model_rhw_end = 0.0060f;
static const float g_close_model_extent_start = 96.0f;
static const float g_close_model_extent_end = 320.0f;
static const float g_close_model_blend_scale = 0.35f;
static const float g_close_model_max_expansion = 1.10f;
static const float g_model_uv_snap_grid = 255.0f;
static const float g_model_uv_center_grid = 256.0f;
static const float g_model_uv_snap_epsilon = 0.015f;
static const DWORD g_crow_wing_callsite = 0x0040EC01u;
static const DWORD g_crow_wing_texture_handle = 0x0000000Au;
static const DWORD g_crow_wing_texture_width = 128u;
static const DWORD g_crow_wing_texture_height = 128u;
static const float g_crow_wing_min_area = 1800.0f;
static const float g_crow_wing_min_extent = 32.0f;
static const float g_crow_wing_max_extent = 360.0f;
static const float g_crow_wing_rhw_max = 0.00075f;
static const float g_crow_wing_rhw_span_max = 0.00000240f;
static const float g_crow_wing_z_span_max = 0.000250f;
static const float g_crow_wing_depth_floor_span = 0.000340f;
static const float g_crow_wing_depth_max_span = 0.000380f;
static const float g_crow_wing_depth_max_shift = 0.000160f;
static const float g_zfight_tiny_span = 0.000080f;
static const float g_zfight_tiny_delta = 0.000030f;
static float g_model_z_min_current = 1.0e30f;
static float g_model_z_max_current = -1.0e30f;
static float g_model_z_min_previous = 0.0f;
static float g_model_z_max_previous = 1.0f;
static const DWORD g_model_callsite_min = 0x0040E000u;
static const DWORD g_model_callsite_max = 0x0040F800u;
static const DWORD g_re2_batched_model_callsite = 0x004080D8u;
static const DWORD g_batched_model_max_vertices = 768u;
static const float g_batched_model_max_screen_extent = 960.0f;
static const float g_batched_model_max_screen_area = 420000.0f;

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
static int g_model_z_current_valid = 0;
static int g_model_z_previous_valid = 0;
static int g_model_depth_written_this_scene = 0;

// Forward declarations.
static void* GetOriginal(void** vtable, int slot);
static void LogLine(const char* fmt, ...);
static TextureSurfaceTrace* FindTextureSurface(void* surface);
static void PatchD3DDevice2(void* device);
static void ClearDepthBuffer(void* self);
static void ClearDepthBufferForScene(void* self);
static void ClearModelDepthBeforeKnown2D(void* self, DWORD caller, DWORD vertex_type,
                                         const void* vertices, DWORD vertex_count,
                                         const WORD* indices, DWORD index_count);
static TextureHandleTrace* FindTextureHandleTraceByHandle(DWORD handle);
static HRESULT DrawPrimitiveWithModelDepth(void* self, D3DDevice2DrawPrimitiveProc orig, DWORD primitive_type,
                                           DWORD vertex_type, void* vertices, DWORD vertex_count, DWORD flags,
                                           DWORD caller, DWORD tris, const DrawBounds* bounds);
// Hook entry points.
static HRESULT STDMETHODCALLTYPE Hook_DD_CreateSurface(void* self, DDSURFACEDESC_COMPAT* desc,
                                                       void** surface, void* outer);
static HRESULT STDMETHODCALLTYPE Hook_QueryInterface(void* self, REFIID riid, void** ppvObj);
static HRESULT STDMETHODCALLTYPE Hook_D3D2_CreateDevice(void* self, REFCLSID rclsid, void* surface, void** device);
static HRESULT STDMETHODCALLTYPE Hook_D3DTexture2_GetHandle(void* self, void* device, DWORD* handle);

// Logging and callsite diagnostics.
static const char* BaseNameFromPath(const char* path)
{
  const char* base = path;
  if (!path)
    return "";
  for (const char* p = path; *p; p++)
  {
    if (*p == '\\' || *p == '/')
      base = p + 1;
  }
  return base;
}

static void BuildLogPath(HINSTANCE instance)
{
  DWORD len = GetModuleFileNameA(instance, g_log_path, sizeof(g_log_path));
  if (!len || len >= sizeof(g_log_path))
  {
    g_game_dir[0] = '\0';
    g_log_path[0] = '\0';
    return;
  }
  for (char* p = g_log_path + strlen(g_log_path); p > g_log_path; --p)
  {
    if (p[-1] == '\\' || p[-1] == '/')
    {
      *p = '\0';
      break;
    }
  }
  strncpy(g_game_dir, g_log_path, sizeof(g_game_dir) - 1);
  g_game_dir[sizeof(g_game_dir) - 1] = '\0';
  strncat(g_log_path, "re2_zfix.log", sizeof(g_log_path) - strlen(g_log_path) - 1);
}

static void LogLine(const char* fmt, ...)
{
  if (!g_diagnostics || !g_log_path[0])
    return;

  char line[1400];
  SYSTEMTIME st;
  GetLocalTime(&st);
  int prefix = snprintf(line, sizeof(line), "[%02u:%02u:%02u.%03u] ",
                        st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
  if (prefix < 0 || prefix >= (int)sizeof(line))
    return;

  va_list args;
  va_start(args, fmt);
  int body = vsnprintf(line + prefix, sizeof(line) - (size_t)prefix, fmt, args);
  va_end(args);
  if (body < 0)
    return;

  size_t len = strlen(line);
  if (len + 2 < sizeof(line))
  {
    line[len++] = '\r';
    line[len++] = '\n';
    line[len] = '\0';
  }

  HANDLE file = CreateFileA(g_log_path, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
                            NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if (file == INVALID_HANDLE_VALUE)
    return;
  DWORD written = 0;
  WriteFile(file, line, (DWORD)len, &written, NULL);
  CloseHandle(file);
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
  const char* base = (len > 0 && len < sizeof(path)) ? BaseNameFromPath(path) : "module";
  strncpy(info->module_name, base, sizeof(info->module_name) - 1);
  info->module_name[sizeof(info->module_name) - 1] = '\0';
  info->module_offset = address - (DWORD)(uintptr_t)module;
}

static int IsReadableCodeAddress(DWORD address, MEMORY_BASIC_INFORMATION* mbi)
{
  if (!address || !VirtualQuery((const void*)(uintptr_t)address, mbi, sizeof(*mbi)))
    return 0;
  if (mbi->State != MEM_COMMIT || (mbi->Protect & (PAGE_GUARD | PAGE_NOACCESS)))
    return 0;
  return 1;
}

static void LogCodeBytesAround(DWORD caller)
{
  MEMORY_BASIC_INFORMATION mbi;
  if (!IsReadableCodeAddress(caller, &mbi))
    return;

  DWORD region_start = (DWORD)(uintptr_t)mbi.BaseAddress;
  DWORD region_end = region_start + (DWORD)mbi.RegionSize;
  DWORD start = caller >= 24 ? caller - 24 : caller;
  DWORD end = caller + 40;
  if (start < region_start)
    start = region_start;
  if (end > region_end)
    end = region_end;
  if (end <= start)
    return;

  char bytes[320];
  size_t used = 0;
  bytes[0] = '\0';
  for (DWORD p = start; p < end && used + 4 < sizeof(bytes); p++)
  {
    int written = snprintf(bytes + used, sizeof(bytes) - used, "%02X ",
                           (unsigned)*(const BYTE*)(uintptr_t)p);
    if (written <= 0)
      break;
    used += (size_t)written;
  }

  ModuleAddressInfo info;
  ResolveModuleForAddress(caller, &info);
  LogLine("lower-probe bytes caller=%s+0x%08lX raw=0x%08lX range=0x%08lX..0x%08lX %s",
          info.module_name, info.module_offset, caller, start, end, bytes);
}

static void LogCallStackForProbe(DWORD caller)
{
  void* stack[12];
  USHORT frames = RtlCaptureStackBackTrace(0, (ULONG)ARRAYSIZE(stack), stack, NULL);
  if (!frames)
    return;

  char line[900];
  size_t used = (size_t)snprintf(line, sizeof(line), "lower-probe stack caller=0x%08lX", caller);
  for (USHORT i = 0; i < frames && used + 64 < sizeof(line); i++)
  {
    DWORD addr = (DWORD)(uintptr_t)stack[i];
    ModuleAddressInfo info;
    ResolveModuleForAddress(addr, &info);
    int written = snprintf(line + used, sizeof(line) - used, " #%u=%s+0x%08lX",
                           (unsigned)i, info.module_name, info.module_offset);
    if (written <= 0)
      break;
    used += (size_t)written;
  }
  LogLine("%s", line);
}

static void ProbeLowerLevelCallsiteOnce(DWORD caller)
{
  if (!g_lower_level_probe)
    return;

  const LONG logged = InterlockedIncrement(&g_lower_probe_logged);
  if (logged > 48)
    return;

  LogCodeBytesAround(caller);
  LogCallStackForProbe(caller);
}

static DrawCallsiteStats* FindOrCreateDrawCallsite(DWORD caller)
{
  if (!g_callsite_diagnostics || !caller)
    return NULL;

  LONG count = g_draw_callsite_count;
  if (count > (LONG)ARRAYSIZE(g_draw_callsites))
    count = (LONG)ARRAYSIZE(g_draw_callsites);
  for (LONG i = 0; i < count; i++)
  {
    if (g_draw_callsites[i].caller == caller)
      return &g_draw_callsites[i];
  }

  LONG index = InterlockedIncrement(&g_draw_callsite_count) - 1;
  if (index < 0 || index >= (LONG)ARRAYSIZE(g_draw_callsites))
    return NULL;

  DrawCallsiteStats* site = &g_draw_callsites[index];
  memset(site, 0, sizeof(*site));
  site->caller = caller;
  ResolveModuleForAddress(caller, &site->address);
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
    ProbeLowerLevelCallsiteOnce(caller);
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
                            const DrawBounds* after, const DepthRangeMap* map,
                            DWORD model_changed, DWORD secondary_changed, DWORD targeted_changed)
{
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
          "model=%lu secondary=%lu targeted=%lu z %.6f..%.6f span=%.8f -> %.6f..%.6f span=%.8f "
          "rhw=[%.8f..%.8f] xy=[%.2f..%.2f %.2f..%.2f] map=%d %.6f..%.6f",
          sample, kind ? kind : "draw", info.module_name, info.module_offset, indexed,
          primitive_type, vertex_count, index_count, model_changed, secondary_changed, targeted_changed,
          before->min_z, before->max_z, before_span,
          after->min_z, after->max_z, after_span,
          before->min_rhw, before->max_rhw,
          before->min_x, before->max_x, before->min_y, before->max_y,
          map && map->valid, map ? map->min_z : 0.0f, map ? map->max_z : 0.0f);
}

static void LogDrawCallsiteSummary(const char* reason)
{
  LONG count = g_draw_callsite_count;
  if (count > (LONG)ARRAYSIZE(g_draw_callsites))
    count = (LONG)ARRAYSIZE(g_draw_callsites);

  LogLine("summary %s callsites=%ld total=%ld acceptDP=%ld acceptDIP=%ld transparent=%ld rejected=%ld",
          reason ? reason : "snapshot", count, g_draw_total, g_draw_model_accepted,
          g_draw_indexed_model_accepted, g_draw_transparent_accepted, g_draw_rejected);
  LogLine("summary depth prepass=%d draws=%ld failures=%ld colorZWrite=%d colorZFunc=%d "
          "clearEachScene=%d modelDepthWritten=%d",
          g_model_depth_prepass, g_model_depth_prepass_draws, g_model_depth_prepass_failures,
          g_model_color_pass_z_write, g_model_color_pass_z_func,
          g_clear_depth_each_scene, g_model_depth_written_this_scene);
  LogLine("summary crow_wing_fix enabled=%d seen=%ld changedVerts=%ld logged=%ld",
          g_crow_wing_depth_fix, g_crow_wing_fix_seen,
          g_crow_wing_fix_changed, g_crow_wing_fix_logged);
  LogLine("summary model_uv enabled=%d correctedDraws=%ld correctedCoords=%ld logged=%ld",
          g_model_uv_correction, g_model_uv_corrected_draws,
          g_model_uv_corrected_coords, g_model_uv_correction_logged);
  LogLine("summary texture_trace enabled=%d surfaces=%ld surfaceSlots=%ld surfaceLogged=%ld "
          "textureQI=%ld bindings=%ld bindingLogged=%ld handleCalls=%ld handles=%ld handleLogged=%ld",
          g_texture_handle_trace, g_texture_surface_seen, g_texture_surface_count,
          g_texture_surface_logged, g_texture_qi_seen, g_texture_binding_count,
          g_texture_binding_logged, g_texture_handle_seen, g_texture_handle_count,
          g_texture_handle_logged);
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
static float AbsF(float v)
{
  return v < 0.0f ? -v : v;
}

static float ClampDepth(float z)
{
  if (z < 0.0f)
    return 0.0f;
  if (z > 1.0f)
    return 1.0f;
  return z;
}

static float Clamp01(float v)
{
  if (v < 0.0f)
    return 0.0f;
  if (v > 1.0f)
    return 1.0f;
  return v;
}

static float LerpF(float a, float b, float t)
{
  return a + ((b - a) * t);
}

static int NearF(float a, float b, float eps)
{
  return AbsF(a - b) <= eps;
}

static void* GetVTableSlot(void* obj, int slot)
{
  if (!obj)
    return NULL;

  void** vtable = *(void***)obj;
  return vtable ? vtable[slot] : NULL;
}

static DWORD TriangleCount(DWORD primitive_type, DWORD vertex_or_index_count)
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
    if (reason)
      *reason = "flat_depth";
    LogFlatDepthReject(caller, "opaque-dp", &local_bounds, primitive_type, vertex_count, 0);
    return 0;
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
    if (reason)
      *reason = "flat_depth";
    LogFlatDepthReject(caller, "opaque-dip", &local_bounds, primitive_type, vertex_count, index_count);
    return 0;
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
    if (reason)
      *reason = "transparent_flat_depth";
    return 0;
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
    if (reason)
      *reason = "transparent_flat_depth";
    return 0;
  }
  return 1;
}

// Model depth remapping.
static void ResetCurrentModelDepthRange(void)
{
  g_model_z_min_current = 1.0e30f;
  g_model_z_max_current = -1.0e30f;
  g_model_z_current_valid = 0;
}

static void ExpandModelDepthRange(float min_z, float max_z)
{
  if (min_z < 0.0f || max_z > 1.0f || max_z < min_z)
    return;

  if (!g_model_z_current_valid)
  {
    g_model_z_min_current = min_z;
    g_model_z_max_current = max_z;
    g_model_z_current_valid = 1;
    return;
  }

  if (min_z < g_model_z_min_current)
    g_model_z_min_current = min_z;
  if (max_z > g_model_z_max_current)
    g_model_z_max_current = max_z;
}

static void AddRangeToMap(DepthRangeMap* map, float min_z, float max_z)
{
  if (!map || min_z < 0.0f || max_z > 1.0f || max_z < min_z)
    return;

  if (!map->valid)
  {
    map->min_z = min_z;
    map->max_z = max_z;
    map->valid = 1;
    return;
  }

  if (min_z < map->min_z)
    map->min_z = min_z;
  if (max_z > map->max_z)
    map->max_z = max_z;
}

static void PadAndFloorDepthMap(DepthRangeMap* map)
{
  if (!map || !map->valid)
    return;

  map->min_z -= g_model_depth_range_padding;
  map->max_z += g_model_depth_range_padding;
  if (map->min_z < 0.0f)
    map->min_z = 0.0f;
  if (map->max_z > 1.0f)
    map->max_z = 1.0f;

  float span = map->max_z - map->min_z;
  if (g_model_depth_range_floor > 0.0f && span < g_model_depth_range_floor)
  {
    const float center = (map->min_z + map->max_z) * 0.5f;
    const float half = g_model_depth_range_floor * 0.5f;
    map->min_z = center - half;
    map->max_z = center + half;

    if (map->min_z < 0.0f)
    {
      map->max_z -= map->min_z;
      map->min_z = 0.0f;
    }
    if (map->max_z > 1.0f)
    {
      const float over = map->max_z - 1.0f;
      map->min_z -= over;
      map->max_z = 1.0f;
      if (map->min_z < 0.0f)
        map->min_z = 0.0f;
    }
  }
}

static DepthRangeMap BuildDepthMapForDraw(const DrawBounds* bounds)
{
  DepthRangeMap map;
  map.min_z = 0.0f;
  map.max_z = 1.0f;
  map.valid = 0;

  if (g_use_previous_depth_range && g_model_z_previous_valid)
  {
    int use_previous = 1;
    if (bounds)
    {
      const float previous_center = (g_model_z_min_previous + g_model_z_max_previous) * 0.5f;
      const float draw_center = (bounds->min_z + bounds->max_z) * 0.5f;
      if (AbsF(previous_center - draw_center) > g_previous_depth_center_tolerance)
        use_previous = 0;
    }

    if (use_previous)
      AddRangeToMap(&map, g_model_z_min_previous, g_model_z_max_previous);
  }
  if (g_model_z_current_valid)
    AddRangeToMap(&map, g_model_z_min_current, g_model_z_max_current);
  if (bounds)
    AddRangeToMap(&map, bounds->min_z, bounds->max_z);

  PadAndFloorDepthMap(&map);
  if (!map.valid || (map.max_z - map.min_z) < g_model_depth_min_span)
    map.valid = 0;
  return map;
}

static float CloseModelFactorForBounds(const DrawBounds* bounds)
{
  if (!g_close_model_depth_softening || !bounds)
    return 0.0f;

  const float rhw_factor =
    Clamp01((bounds->max_rhw - g_close_model_rhw_start) /
            (g_close_model_rhw_end - g_close_model_rhw_start));
  const float extent = bounds->width > bounds->height ? bounds->width : bounds->height;
  const float extent_factor =
    Clamp01((extent - g_close_model_extent_start) /
            (g_close_model_extent_end - g_close_model_extent_start));
  return rhw_factor > extent_factor ? rhw_factor : extent_factor;
}

static float EffectiveDepthBlendForSpan(float map_span, const DrawBounds* bounds)
{
  float blend = g_model_depth_blend;
  const float close_factor = CloseModelFactorForBounds(bounds);
  float max_expansion = g_model_depth_max_expansion;

  if (close_factor > 0.0f)
  {
    blend *= LerpF(1.0f, g_close_model_blend_scale, close_factor);
    max_expansion = LerpF(g_model_depth_max_expansion, g_close_model_max_expansion, close_factor);
  }

  const float target_span = g_model_depth_far - g_model_depth_near;
  if (map_span <= 0.0f || target_span <= 0.0f || max_expansion <= 1.0f)
    return blend;

  const float normalized_scale = target_span / map_span;
  if (normalized_scale <= max_expansion || normalized_scale <= 1.0f)
    return blend;

  float max_blend = (max_expansion - 1.0f) / (normalized_scale - 1.0f);
  if (max_blend < 0.0f)
    max_blend = 0.0f;
  if (max_blend > 1.0f)
    max_blend = 1.0f;
  if (blend > max_blend)
    blend = max_blend;
  return blend;
}

static float EffectiveCenteredDepthScale(const DrawBounds* bounds)
{
  float scale = g_model_depth_centered_scale;
  const float close_factor = CloseModelFactorForBounds(bounds);

  if (close_factor > 0.0f)
  {
    scale = LerpF(g_model_depth_centered_scale, g_close_model_max_expansion, close_factor);
  }

  if (scale < 1.0f)
    scale = 1.0f;
  if (scale > g_model_depth_max_expansion)
    scale = g_model_depth_max_expansion;
  return scale;
}

static DWORD ApplyModelDepth(D3DTLVERTEX_COMPAT* vertices, DWORD vertex_count, const DepthRangeMap* map,
                             const DrawBounds* bounds)
{
  if (!vertices || vertex_count == 0)
    return 0;

  DWORD changed = 0;
  const int normalize = g_model_depth_normalize && map && map->valid &&
                        g_model_depth_far > g_model_depth_near;
  const float span = normalize ? (map->max_z - map->min_z) : 1.0f;
  float effective_scale = (normalize && span >= g_model_depth_min_span && g_model_depth_centered) ?
                          EffectiveCenteredDepthScale(bounds) : 1.0f;
  const float effective_blend = (normalize && span >= g_model_depth_min_span && !g_model_depth_centered) ?
                                EffectiveDepthBlendForSpan(span, bounds) : 0.0f;
  float input_min = vertices[0].sz;
  float input_max = vertices[0].sz;
  for (DWORD i = 1; i < vertex_count; i++)
  {
    if (vertices[i].sz < input_min)
      input_min = vertices[i].sz;
    if (vertices[i].sz > input_max)
      input_max = vertices[i].sz;
  }
  const float centered_depth_center = (input_min + input_max) * 0.5f;
  if (normalize && span >= g_model_depth_min_span && g_model_depth_centered)
  {
    const float below_center = centered_depth_center - input_min;
    const float above_center = input_max - centered_depth_center;
    if (below_center > 0.0f)
    {
      const float max_scale_low = (centered_depth_center - g_model_depth_centered_min) / below_center;
      if (max_scale_low < effective_scale)
        effective_scale = max_scale_low;
    }
    if (above_center > 0.0f)
    {
      const float max_scale_high = (g_model_depth_centered_max - centered_depth_center) / above_center;
      if (max_scale_high < effective_scale)
        effective_scale = max_scale_high;
    }
    if (effective_scale < 0.0f)
      effective_scale = 0.0f;
  }
  for (DWORD i = 0; i < vertex_count; i++)
  {
    const float original_z = vertices[i].sz;
    float z = original_z;
    if (normalize && span >= g_model_depth_min_span)
    {
      if (g_model_depth_centered)
      {
        z = centered_depth_center + ((original_z - centered_depth_center) * effective_scale);
      }
      else
      {
        const float t = (z - map->min_z) / span;
        const float mapped = g_model_depth_near + (t * (g_model_depth_far - g_model_depth_near));
        z = original_z + ((mapped - original_z) * effective_blend);
      }
    }
    vertices[i].sz = ClampDepth(z + g_model_depth_bias);
    changed++;
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

// Targeted model fixes.
static int CurrentTextureMatchesCrowWing(void)
{
  const DWORD texture = TrackedRenderStateValue(D3DRENDERSTATE_TEXTUREHANDLE);
  if (!texture || texture == 0xFFFFFFFFu || texture != g_crow_wing_texture_handle)
    return 0;

  const TextureHandleTrace* texture_trace = FindTextureHandleTraceByHandle(texture);
  if (!texture_trace)
    return 1;

  if (g_crow_wing_texture_width && texture_trace->width &&
      texture_trace->width != g_crow_wing_texture_width)
    return 0;
  if (g_crow_wing_texture_height && texture_trace->height &&
      texture_trace->height != g_crow_wing_texture_height)
    return 0;
  return 1;
}

static DWORD ApplyCrowWingDepthFix(DWORD caller, DWORD primitive_type, DWORD vertex_type,
                                   D3DTLVERTEX_COMPAT* vertices, DWORD vertex_count,
                                   const DrawBounds* bounds)
{
  if (!g_crow_wing_depth_fix || !vertices || vertex_count != 3 || !bounds)
    return 0;
  if (caller != g_crow_wing_callsite || primitive_type != D3DPT_TRIANGLELIST ||
      vertex_type != D3DVT_TLVERTEX)
    return 0;
  if (!CurrentTextureMatchesCrowWing())
    return 0;

  const float width = AbsF(bounds->width);
  const float height = AbsF(bounds->height);
  const float extent = width > height ? width : height;
  const float area = width * height;
  const float z_span = bounds->max_z - bounds->min_z;
  const float rhw_span = bounds->max_rhw - bounds->min_rhw;
  if (z_span < 0.0f || z_span > g_crow_wing_z_span_max)
    return 0;
  if (bounds->max_rhw > g_crow_wing_rhw_max || rhw_span > g_crow_wing_rhw_span_max)
    return 0;
  if (area < g_crow_wing_min_area || extent < g_crow_wing_min_extent ||
      extent > g_crow_wing_max_extent)
    return 0;

  float target_span = z_span;
  if (target_span < g_crow_wing_depth_floor_span)
    target_span = g_crow_wing_depth_floor_span;
  if (target_span > g_crow_wing_depth_max_span)
    target_span = g_crow_wing_depth_max_span;
  if (target_span <= z_span || target_span <= 0.0f)
    return 0;

  const float center_z = (bounds->min_z + bounds->max_z) * 0.5f;
  const int use_existing_z = z_span >= 0.00000050f;
  const int use_rhw = !use_existing_z && rhw_span >= 0.00000001f;
  const int use_x = width >= height;
  const float axis_min = use_x ? bounds->min_x : bounds->min_y;
  const float axis_max = use_x ? bounds->max_x : bounds->max_y;
  const float axis_span = axis_max - axis_min;
  if (!use_existing_z && !use_rhw && axis_span <= 1.0f)
    return 0;

  InterlockedIncrement(&g_crow_wing_fix_seen);

  DWORD changed = 0;
  for (DWORD i = 0; i < vertex_count; i++)
  {
    const float old_z = vertices[i].sz;
    float target_z = old_z;
    if (use_existing_z)
    {
      target_z = center_z + ((old_z - center_z) * (target_span / z_span));
    }
    else if (use_rhw)
    {
      const float close01 = Clamp01((vertices[i].rhw - bounds->min_rhw) / rhw_span);
      target_z = center_z + ((0.5f - close01) * target_span);
    }
    else
    {
      const float axis = use_x ? vertices[i].sx : vertices[i].sy;
      target_z = center_z + ((Clamp01((axis - axis_min) / axis_span) - 0.5f) * target_span);
    }

    float delta = target_z - old_z;
    if (delta > g_crow_wing_depth_max_shift)
      delta = g_crow_wing_depth_max_shift;
    if (delta < -g_crow_wing_depth_max_shift)
      delta = -g_crow_wing_depth_max_shift;
    vertices[i].sz = ClampDepth(old_z + delta);
    if (vertices[i].sz != old_z)
      changed++;
  }

  if (changed)
  {
    InterlockedExchangeAdd(&g_crow_wing_fix_changed, (LONG)changed);
    const LONG logged = InterlockedIncrement(&g_crow_wing_fix_logged);
    if (logged <= 48)
    {
      const DWORD texture = TrackedRenderStateValue(D3DRENDERSTATE_TEXTUREHANDLE);
      LogLine("crow-wing-fix #%ld caller=0x%08lX texture=0x%08lX verts=%lu changed=%lu "
              "zSpan=%.9f targetSpan=%.9f rhw=[%.9f..%.9f] area=%.1f extent=%.1f",
              logged, caller, texture, vertex_count, changed, z_span, target_span,
              bounds->min_rhw, bounds->max_rhw, area, extent);
    }
  }
  return changed;
}

static float AdjustModelTexCoord(float value)
{
  if (g_model_uv_snap_grid <= 0.0f || g_model_uv_center_grid <= 0.0f)
    return value;
  if (value < -0.001f || value > 1.001f)
    return value;

  const float scaled = value * g_model_uv_snap_grid;
  int texel = (int)(scaled + 0.5f);
  if (texel < 0)
    texel = 0;
  if (texel > (int)g_model_uv_snap_grid)
    texel = (int)g_model_uv_snap_grid;
  if (AbsF(scaled - (float)texel) > g_model_uv_snap_epsilon)
    return value;

  return ((float)texel + 0.5f) / g_model_uv_center_grid;
}

// Model texture coordinate cleanup.
static DWORD ApplyModelTexCoordCorrection(D3DTLVERTEX_COMPAT* vertices, DWORD vertex_count)
{
  if (!g_model_uv_correction || !vertices || vertex_count == 0)
    return 0;

  const float first_old_u = vertices[0].tu;
  const float first_old_v = vertices[0].tv;
  DWORD changed = 0;
  for (DWORD i = 0; i < vertex_count; i++)
  {
    const float old_u = vertices[i].tu;
    const float old_v = vertices[i].tv;
    const float new_u = AdjustModelTexCoord(old_u);
    const float new_v = AdjustModelTexCoord(old_v);
    vertices[i].tu = new_u;
    vertices[i].tv = new_v;
    if (new_u != old_u)
      changed++;
    if (new_v != old_v)
      changed++;
  }

  if (changed)
  {
    InterlockedIncrement(&g_model_uv_corrected_draws);
    InterlockedExchangeAdd(&g_model_uv_corrected_coords, (LONG)changed);
    const LONG logged = InterlockedIncrement(&g_model_uv_correction_logged);
    if (logged <= 64)
    {
      const DWORD texture = TrackedRenderStateValue(D3DRENDERSTATE_TEXTUREHANDLE);
      LogLine("model-uv-correct #%ld texture=0x%08lX verts=%lu coords=%lu "
              "firstUV %.7f,%.7f -> %.7f,%.7f",
              logged, texture, vertex_count, changed,
              first_old_u, first_old_v, vertices[0].tu, vertices[0].tv);
    }
  }
  return changed;
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
  ComReleaseProc release = vt ? (ComReleaseProc)vt[2] : NULL;
  if (clear)
    clear(viewport, 0, NULL, D3DCLEAR_ZBUFFER);
  if (release)
    release(viewport);
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
  ResetCurrentModelDepthRange();
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

static void ForceModelColorPassAfterPrepassState(void* self)
{
  SetOneRenderState(self, D3DRENDERSTATE_ZENABLE, 1);
  SetOneRenderState(self, D3DRENDERSTATE_ZWRITEENABLE, (DWORD)g_model_color_pass_z_write);
  SetOneRenderState(self, D3DRENDERSTATE_ZFUNC, (DWORD)g_model_color_pass_z_func);
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
    InterlockedIncrement(&g_model_depth_prepass_draws);
  else
    InterlockedIncrement(&g_model_depth_prepass_failures);
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
    InterlockedIncrement(&g_model_depth_prepass_draws);
  else
    InterlockedIncrement(&g_model_depth_prepass_failures);
}

static HRESULT DrawPrimitiveWithModelDepth(void* self, D3DDevice2DrawPrimitiveProc orig, DWORD primitive_type,
                                           DWORD vertex_type, void* vertices, DWORD vertex_count, DWORD flags,
                                           DWORD caller, DWORD tris, const DrawBounds* bounds)
{
  (void)tris;
  DepthStateSnapshot snapshot;
  CaptureDepthState(self, &snapshot);
  ForceDepthStateForDraw(self);
  ForceModelQualityStateForDraw(self, &snapshot);

  D3DTLVERTEX_COMPAT* copy =
    (D3DTLVERTEX_COMPAT*)HeapAlloc(GetProcessHeap(), 0, sizeof(D3DTLVERTEX_COMPAT) * vertex_count);
  if (!copy)
  {
    HRESULT hr = orig(self, primitive_type, vertex_type, vertices, vertex_count, flags);
    RestoreDepthStateForDraw(self, &snapshot);
    return hr;
  }

  memcpy(copy, vertices, sizeof(D3DTLVERTEX_COMPAT) * vertex_count);
  ApplyModelTexCoordCorrection(copy, vertex_count);
  ExpandModelDepthRange(bounds->min_z, bounds->max_z);
  const DepthRangeMap map = BuildDepthMapForDraw(bounds);
  const DWORD model_changed = ApplyModelDepth(copy, vertex_count, &map, bounds);
  DrawBounds adjusted_bounds;
  ComputeDrawBounds(copy, vertex_count, &adjusted_bounds);
  const DWORD crow_wing_changed =
    ApplyCrowWingDepthFix(caller, primitive_type, vertex_type, copy, vertex_count, &adjusted_bounds);
  if (crow_wing_changed)
    ComputeDrawBounds(copy, vertex_count, &adjusted_bounds);
  LogZFightSample(caller, "opaque-dp", 0, primitive_type, vertex_count, 0,
                  bounds, &adjusted_bounds, &map, model_changed, 0, crow_wing_changed);

  RunModelDepthPrepass(self, orig, primitive_type, vertex_type, copy, vertex_count, flags);
  ForceDepthStateForDraw(self);
  ForceModelQualityStateForDraw(self, &snapshot);
  if (g_model_depth_prepass)
    ForceModelColorPassAfterPrepassState(self);
  HRESULT hr = orig(self, primitive_type, vertex_type, copy, vertex_count, flags);
  RestoreDepthStateForDraw(self, &snapshot);
  HeapFree(GetProcessHeap(), 0, copy);
  g_model_depth_written_this_scene = 1;
  return hr;
}

static HRESULT DrawIndexedPrimitiveWithModelDepth(void* self, D3DDevice2DrawIndexedPrimitiveProc orig,
                                                  DWORD primitive_type, DWORD vertex_type, void* vertices,
                                                  DWORD vertex_count, WORD* indices, DWORD index_count, DWORD flags,
                                                  DWORD caller, const DrawBounds* bounds)
{
  DepthStateSnapshot snapshot;
  CaptureDepthState(self, &snapshot);
  ForceDepthStateForDraw(self);
  ForceModelQualityStateForDraw(self, &snapshot);

  D3DTLVERTEX_COMPAT* copy =
    (D3DTLVERTEX_COMPAT*)HeapAlloc(GetProcessHeap(), 0, sizeof(D3DTLVERTEX_COMPAT) * vertex_count);
  if (!copy)
  {
    HRESULT hr = orig(self, primitive_type, vertex_type, vertices, vertex_count, indices, index_count, flags);
    RestoreDepthStateForDraw(self, &snapshot);
    return hr;
  }

  memcpy(copy, vertices, sizeof(D3DTLVERTEX_COMPAT) * vertex_count);
  ApplyModelTexCoordCorrection(copy, vertex_count);
  ExpandModelDepthRange(bounds->min_z, bounds->max_z);
  const DepthRangeMap map = BuildDepthMapForDraw(bounds);
  const DWORD model_changed = ApplyModelDepth(copy, vertex_count, &map, bounds);
  DrawBounds adjusted_bounds;
  if (!ComputeIndexedDrawBounds(copy, vertex_count, indices, index_count, &adjusted_bounds))
    ComputeDrawBounds(copy, vertex_count, &adjusted_bounds);
  LogZFightSample(caller, "opaque-dip", 1, primitive_type, vertex_count, index_count,
                  bounds, &adjusted_bounds, &map, model_changed, 0, 0);

  RunIndexedModelDepthPrepass(self, orig, primitive_type, vertex_type, copy,
                              vertex_count, indices, index_count, flags);
  ForceDepthStateForDraw(self);
  ForceModelQualityStateForDraw(self, &snapshot);
  if (g_model_depth_prepass)
    ForceModelColorPassAfterPrepassState(self);
  HRESULT hr = orig(self, primitive_type, vertex_type, copy, vertex_count, indices, index_count, flags);
  RestoreDepthStateForDraw(self, &snapshot);
  HeapFree(GetProcessHeap(), 0, copy);
  g_model_depth_written_this_scene = 1;
  return hr;
}

static HRESULT DrawPrimitiveWithTransparentModelDepth(void* self, D3DDevice2DrawPrimitiveProc orig,
                                                      DWORD primitive_type, DWORD vertex_type, void* vertices,
                                                      DWORD vertex_count, DWORD flags, DWORD caller,
                                                      const DrawBounds* bounds)
{
  (void)caller;
  (void)bounds;
  DepthStateSnapshot snapshot;
  CaptureDepthState(self, &snapshot);
  ForceDepthStateForTransparentDraw(self);
  ForceModelQualityStateForDraw(self, &snapshot);

  D3DTLVERTEX_COMPAT* copy = NULL;
  void* draw_vertices = vertices;
  if (g_model_uv_correction)
  {
    copy = (D3DTLVERTEX_COMPAT*)HeapAlloc(GetProcessHeap(), 0, sizeof(D3DTLVERTEX_COMPAT) * vertex_count);
    if (copy)
    {
      memcpy(copy, vertices, sizeof(D3DTLVERTEX_COMPAT) * vertex_count);
      ApplyModelTexCoordCorrection(copy, vertex_count);
      draw_vertices = copy;
    }
  }

  HRESULT hr = orig(self, primitive_type, vertex_type, draw_vertices, vertex_count, flags);
  RestoreDepthStateForDraw(self, &snapshot);
  if (copy)
    HeapFree(GetProcessHeap(), 0, copy);
  return hr;
}

static HRESULT DrawIndexedPrimitiveWithTransparentModelDepth(void* self, D3DDevice2DrawIndexedPrimitiveProc orig,
                                                            DWORD primitive_type, DWORD vertex_type, void* vertices,
                                                            DWORD vertex_count, WORD* indices, DWORD index_count,
                                                            DWORD flags, DWORD caller, const DrawBounds* bounds)
{
  (void)caller;
  (void)bounds;
  DepthStateSnapshot snapshot;
  CaptureDepthState(self, &snapshot);
  ForceDepthStateForTransparentDraw(self);
  ForceModelQualityStateForDraw(self, &snapshot);

  D3DTLVERTEX_COMPAT* copy = NULL;
  void* draw_vertices = vertices;
  if (g_model_uv_correction)
  {
    copy = (D3DTLVERTEX_COMPAT*)HeapAlloc(GetProcessHeap(), 0, sizeof(D3DTLVERTEX_COMPAT) * vertex_count);
    if (copy)
    {
      memcpy(copy, vertices, sizeof(D3DTLVERTEX_COMPAT) * vertex_count);
      ApplyModelTexCoordCorrection(copy, vertex_count);
      draw_vertices = copy;
    }
  }

  HRESULT hr = orig(self, primitive_type, vertex_type, draw_vertices, vertex_count, indices, index_count, flags);
  RestoreDepthStateForDraw(self, &snapshot);
  if (copy)
    HeapFree(GetProcessHeap(), 0, copy);
  return hr;
}

static void* GetOriginal(void** vtable, int slot)
{
  LONG count = g_hook_count;
  for (LONG i = 0; i < count; i++)
  {
    if (g_hooks[i].vtable == vtable && g_hooks[i].slot == slot)
      return g_hooks[i].original;
  }
  return NULL;
}

static int PatchVTableSlot(void* obj, int slot, void* hook)
{
  if (!obj)
    return 0;

  void** vtable = *(void***)obj;
  if (!vtable || vtable[slot] == hook)
    return 1;

  for (LONG i = 0; i < g_hook_count; i++)
  {
    if (g_hooks[i].vtable == vtable && g_hooks[i].slot == slot)
      return 1;
  }

  if (g_hook_count >= (LONG)ARRAYSIZE(g_hooks))
    return 0;

  DWORD old_protect = 0;
  if (!VirtualProtect(&vtable[slot], sizeof(void*), PAGE_EXECUTE_READWRITE, &old_protect))
    return 0;

  LONG idx = InterlockedIncrement(&g_hook_count) - 1;
  if (idx < 0 || idx >= (LONG)ARRAYSIZE(g_hooks))
  {
    DWORD ignored = 0;
    VirtualProtect(&vtable[slot], sizeof(void*), old_protect, &ignored);
    return 0;
  }

  g_hooks[idx].vtable = vtable;
  g_hooks[idx].slot = slot;
  g_hooks[idx].original = vtable[slot];
  vtable[slot] = hook;

  DWORD ignored = 0;
  VirtualProtect(&vtable[slot], sizeof(void*), old_protect, &ignored);
  FlushInstructionCache(GetCurrentProcess(), &vtable[slot], sizeof(void*));
  return 1;
}

static int IsGuid(REFIID a, const GUID* b)
{
  return a && b && memcmp(a, b, sizeof(GUID)) == 0;
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

  if (IsGuid(riid, &kIID_IDirect3D2))
  {
    PatchVTableSlot(obj, 8, (void*)Hook_D3D2_CreateDevice);
  }
  else if (IsGuid(riid, &kIID_IDirect3DDevice2))
  {
    PatchD3DDevice2(obj);
  }
  else if (IsGuid(riid, &kIID_IDirect3DTexture2))
  {
    PatchVTableSlot(obj, 3, (void*)Hook_D3DTexture2_GetHandle);
  }
  else if (IsGuid(riid, &kIID_IDirectDraw2))
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
    if (IsGuid(riid, &kIID_IDirect3DTexture2))
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
  ResetCurrentModelDepthRange();
  ClearDepthBufferForScene(self);
  D3DDevice2BeginSceneProc orig = (D3DDevice2BeginSceneProc)GetOriginal(*(void***)self, 10);
  return orig ? orig(self) : D3D_OK;
}

static HRESULT STDMETHODCALLTYPE Hook_D3DDevice2_EndScene(void* self)
{
  const LONG scene = InterlockedIncrement(&g_scene_counter);
  if (scene <= 8 || (scene % 120) == 0)
  {
    LogLine("scene=%ld total=%ld acceptDP=%ld acceptDIP=%ld transparent=%ld rejected=%ld zsamples=%ld prepass=%ld/%ld",
            scene, g_draw_total, g_draw_model_accepted, g_draw_indexed_model_accepted,
            g_draw_transparent_accepted, g_draw_rejected, g_zfight_sample_logged,
            g_model_depth_prepass_draws, g_model_depth_prepass_failures);
  }
  if (scene == 60 || scene == 240 || (scene % 720) == 0)
    LogDrawCallsiteSummary("scene");

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
  InterlockedIncrement(&g_draw_total);

  if (!g_enabled || vertex_type != D3DVT_TLVERTEX || !vertices || vertex_count == 0)
  {
    ClearModelDepthBeforeKnown2D(self, caller, vertex_type, vertices, vertex_count, NULL, 0);
    return orig(self, primitive_type, vertex_type, vertices, vertex_count, flags);
  }

  const DWORD tris = TriangleCount(primitive_type, vertex_count);
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
      InterlockedIncrement(&g_draw_transparent_accepted);
      TrackDrawCallsite(caller, "accept", 0, 1, primitive_type, vertex_count, 0, tris,
                        "transparent", &transparent_bounds);
      return DrawPrimitiveWithTransparentModelDepth(self, orig, primitive_type, vertex_type, vertices,
                                                   vertex_count, flags, caller, &transparent_bounds);
    }
    InterlockedIncrement(&g_draw_rejected);
    ComputeDrawBounds((const D3DTLVERTEX_COMPAT*)vertices, vertex_count, &bounds);
    TrackDrawCallsite(caller, "reject", 0, 0, primitive_type, vertex_count, 0, tris,
                      reason, &bounds);
    ClearModelDepthBeforeKnown2D(self, caller, vertex_type, vertices, vertex_count, NULL, 0);
    return orig(self, primitive_type, vertex_type, vertices, vertex_count, flags);
  }

  InterlockedIncrement(&g_draw_model_accepted);
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
  InterlockedIncrement(&g_draw_total);

  if (!g_enabled || vertex_type != D3DVT_TLVERTEX || !vertices || !indices || vertex_count == 0 || index_count == 0)
  {
    ClearModelDepthBeforeKnown2D(self, caller, vertex_type, vertices, vertex_count, indices, index_count);
    return orig(self, primitive_type, vertex_type, vertices, vertex_count, indices, index_count, flags);
  }

  const DWORD tris = TriangleCount(primitive_type, index_count);
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
      InterlockedIncrement(&g_draw_transparent_accepted);
      TrackDrawCallsite(caller, "accept", 1, 1, primitive_type, vertex_count, index_count, tris,
                        "transparent", &transparent_bounds);
      return DrawIndexedPrimitiveWithTransparentModelDepth(self, orig, primitive_type, vertex_type, vertices,
                                                          vertex_count, indices, index_count, flags,
                                                          caller, &transparent_bounds);
    }
    InterlockedIncrement(&g_draw_rejected);
    if (indices_ok && ComputeIndexedDrawBounds((const D3DTLVERTEX_COMPAT*)vertices, vertex_count,
                                               indices, index_count, &bounds))
    {
      TrackDrawCallsite(caller, "reject", 1, 0, primitive_type, vertex_count, index_count, tris,
                        reason, &bounds);
    }
    else
    {
      ComputeDrawBounds((const D3DTLVERTEX_COMPAT*)vertices, vertex_count, &bounds);
      TrackDrawCallsite(caller, "reject", 1, 0, primitive_type, vertex_count, index_count, tris,
                        reason ? reason : "bad_indices", &bounds);
    }
    ClearModelDepthBeforeKnown2D(self, caller, vertex_type, vertices, vertex_count, indices, index_count);
    return orig(self, primitive_type, vertex_type, vertices, vertex_count, indices, index_count, flags);
  }

  InterlockedIncrement(&g_draw_indexed_model_accepted);
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

static void* RvaToPtr(BYTE* module, DWORD rva)
{
  if (!rva)
    return NULL;
  return module + rva;
}

static int PatchDirectDrawCreateIAT(void)
{
  BYTE* base = (BYTE*)GetModuleHandleA(NULL);
  if (!base)
    return 0;

  IMAGE_DOS_HEADER* dos = (IMAGE_DOS_HEADER*)base;
  if (dos->e_magic != IMAGE_DOS_SIGNATURE)
    return 0;

  IMAGE_NT_HEADERS* nt = (IMAGE_NT_HEADERS*)(base + dos->e_lfanew);
  if (nt->Signature != IMAGE_NT_SIGNATURE)
    return 0;

  IMAGE_DATA_DIRECTORY dir = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
  if (!dir.VirtualAddress)
    return 0;

  IMAGE_IMPORT_DESCRIPTOR* desc = (IMAGE_IMPORT_DESCRIPTOR*)RvaToPtr(base, dir.VirtualAddress);
  for (; desc->Name; desc++)
  {
    const char* dll = (const char*)RvaToPtr(base, desc->Name);
    if (!dll || _stricmp(dll, "DDRAW.dll") != 0)
      continue;

    IMAGE_THUNK_DATA* orig_thunk = (IMAGE_THUNK_DATA*)RvaToPtr(base, desc->OriginalFirstThunk);
    IMAGE_THUNK_DATA* thunk = (IMAGE_THUNK_DATA*)RvaToPtr(base, desc->FirstThunk);
    if (!orig_thunk)
      orig_thunk = thunk;

    for (; orig_thunk && orig_thunk->u1.AddressOfData; orig_thunk++, thunk++)
    {
#ifdef IMAGE_ORDINAL_FLAG32
      if (orig_thunk->u1.Ordinal & IMAGE_ORDINAL_FLAG32)
#else
      if (orig_thunk->u1.Ordinal & IMAGE_ORDINAL_FLAG)
#endif
        continue;

      IMAGE_IMPORT_BY_NAME* by_name = (IMAGE_IMPORT_BY_NAME*)RvaToPtr(base, (DWORD)orig_thunk->u1.AddressOfData);
      if (!by_name || strcmp((const char*)by_name->Name, "DirectDrawCreate") != 0)
        continue;

      DWORD old_protect = 0;
      if (!VirtualProtect(&thunk->u1.Function, sizeof(void*), PAGE_READWRITE, &old_protect))
        return 0;

      g_real_direct_draw_create = (DirectDrawCreateProc)(uintptr_t)thunk->u1.Function;
      thunk->u1.Function = (ULONG_PTR)(uintptr_t)Hook_DirectDrawCreate;

      DWORD ignored = 0;
      VirtualProtect(&thunk->u1.Function, sizeof(void*), old_protect, &ignored);
      return 1;
    }
  }

  return 0;
}

// DLL entry point.
BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
  (void)reserved;
  if (reason == DLL_PROCESS_ATTACH)
  {
    g_module = instance;
    BuildLogPath(instance);
    DeleteFileA(g_log_path);
    DisableThreadLibraryCalls(instance);
    for (DWORD i = 0; i < ARRAYSIZE(g_render_state_cache); i++)
      g_render_state_cache[i] = 0xFFFFFFFFu;
    LogLine("re2_zfix loaded diagnostics=%d lower_probe=%d model_callsite=0x%08lX..0x%08lX",
            g_diagnostics, g_lower_level_probe, g_model_callsite_min, g_model_callsite_max);
    LogLine("depth zbuffer=%d/%d clearEachScene=%d prepass=%d colorZWrite=%d colorZFunc=%d alphaRef=%lu",
            g_preferred_zbuffer_depth, g_fallback_zbuffer_depth, g_clear_depth_each_scene,
            g_model_depth_prepass, g_model_color_pass_z_write, g_model_color_pass_z_func,
            g_model_alpha_ref);
    LogLine("crow_wing_fix enabled=%d callsite=0x%08lX texture=0x%08lX wh=%lux%lu "
            "areaMin=%.1f extent=%.1f..%.1f rhwMax=%.8f rhwSpanMax=%.8f zSpanMax=%.8f "
            "floorSpan=%.8f maxSpan=%.8f maxShift=%.8f",
            g_crow_wing_depth_fix, g_crow_wing_callsite, g_crow_wing_texture_handle,
            g_crow_wing_texture_width, g_crow_wing_texture_height,
            g_crow_wing_min_area, g_crow_wing_min_extent, g_crow_wing_max_extent,
            g_crow_wing_rhw_max, g_crow_wing_rhw_span_max, g_crow_wing_z_span_max,
            g_crow_wing_depth_floor_span, g_crow_wing_depth_max_span,
            g_crow_wing_depth_max_shift);
    LogLine("transparent_model zTest=%d depthAdjust=%d zWrite=%d zFunc=%d zBias=%d",
            g_transparent_model_z_test, g_transparent_model_depth_adjust,
            g_transparent_model_z_write, g_transparent_model_z_func,
            g_transparent_model_z_bias);
    LogLine("model_quality perspective=%d alphaTest=%d alphaRef=%lu alphaFunc=%lu "
            "uvCorrection=%d uvGrid=%.1f centerGrid=%.1f snapEps=%.4f gouraud=%d dither=%d subpixel=%d",
            g_model_texture_perspective, g_model_alpha_test, g_model_alpha_ref, g_model_alpha_func,
            g_model_uv_correction, g_model_uv_snap_grid,
            g_model_uv_center_grid, g_model_uv_snap_epsilon,
            g_model_gouraud_shading, g_model_dither, g_model_subpixel);
    LogLine("patch DirectDrawCreateIAT=%d", PatchDirectDrawCreateIAT());
  }
  else if (reason == DLL_PROCESS_DETACH)
  {
    LogDrawCallsiteSummary("detach");
  }
  return TRUE;
}
