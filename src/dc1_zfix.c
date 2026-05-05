#include <windows.h>
#include <tlhelp32.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

#ifndef ARRAYSIZE
#define ARRAYSIZE(a) (sizeof(a) / sizeof((a)[0]))
#endif

#define D3D_OK 0
#define D3DPT_TRIANGLELIST 4
#define D3DPT_TRIANGLESTRIP 5
#define D3DPT_TRIANGLEFAN 6
#define D3DCMP_LESS 2
#define D3DCMP_LESSEQUAL 4
#define D3DRS_ZENABLE 7
#define D3DRS_SHADEMODE 9
#define D3DRS_ZWRITEENABLE 14
#define D3DRS_ZFUNC 23
#define D3DRS_DITHERENABLE 26
#define D3DRS_ALPHABLENDENABLE 27
#define D3DRS_COLORWRITEENABLE 168
#define D3D_COLORWRITE_ALL 0x0000000Fu
#define D3DCLEAR_ZBUFFER 0x00000002u
#define D3DSHADE_GOURAUD 2
#define D3DSAMP_MAGFILTER 5
#define D3DSAMP_MINFILTER 6
#define D3DSAMP_MIPFILTER 7
#define D3DTEXF_LINEAR 2
#define D3DBACKBUFFER_TYPE_MONO 0
#define D3DMULTISAMPLE_NONE 0
#define D3DFMT_INDEX16 101
#define D3DFMT_INDEX32 102
#define D3DFMT_D32 71
#define D3DFMT_D24S8 75
#define D3DFMT_D24X8 77
#define D3DFMT_D16 80
#define D3DFMT_D32F_LOCKABLE 82

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
typedef HRESULT(STDMETHODCALLTYPE* D3D9GetSamplerStateProc)(void* self, DWORD sampler, DWORD type, DWORD* value);
typedef HRESULT(STDMETHODCALLTYPE* D3D9SetSamplerStateProc)(void* self, DWORD sampler, DWORD type, DWORD value);
typedef HRESULT(STDMETHODCALLTYPE* D3D9SetTextureProc)(void* self, DWORD stage, void* texture);
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

typedef struct D3D9StateSnapshot {
  DWORD z_enable;
  DWORD z_write;
  DWORD z_func;
  DWORD alpha_blend;
  DWORD color_write;
  DWORD shade_mode;
  DWORD dither_enable;
  DWORD sampler_mag;
  DWORD sampler_min;
  DWORD sampler_mip;
  int has_z_enable;
  int has_z_write;
  int has_z_func;
  int has_alpha_blend;
  int has_color_write;
  int has_shade_mode;
  int has_dither_enable;
  int has_sampler_mag;
  int has_sampler_min;
  int has_sampler_mip;
} D3D9StateSnapshot;

typedef struct HookEntry {
  void** vtable;
  int slot;
  void* original;
} HookEntry;

static HookEntry g_hooks[128];
static volatile LONG g_hook_count = 0;
static char g_log_path[MAX_PATH];
static Direct3DCreate9Proc g_real_direct3d_create9 = NULL;
static int g_scene_open = 0;
static DWORD g_current_fvf = 0;
static void* g_current_texture0 = NULL;
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
static volatile LONG g_dipup_transparent_accepted = 0;
static volatile LONG g_frame_counter = 0;
static volatile LONG g_model_uv_corrected_draws = 0;
static volatile LONG g_model_uv_corrected_coords = 0;
static volatile LONG g_model_subpixel_stabilized_draws = 0;
static volatile LONG g_model_subpixel_stabilized_coords = 0;
static volatile LONG g_model_depth_biased_draws = 0;
static volatile LONG g_model_depth_biased_vertices = 0;
static volatile LONG g_transparent_thin_stabilized_draws = 0;
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
static const int g_model_linear_filter = 1;
static const int g_model_gouraud_shading = 1;
static const int g_model_dither = 1;
static const int g_model_uv_correction = 1;
static const int g_model_subpixel_stabilization = 1;
static const int g_model_screen_expand = 0;
static const int g_skip_axis_tile_draws = 1;
static const int g_min_vertex_alpha = 250;
static const int g_reject_alpha_only_when_blending = 1;
static const int g_diagnostics = 1;
static const int g_opaque_model_disable_alpha_blend = 0;
static const int g_model_depth_bias_enabled = 1;
static const int g_transparent_model_z_test = 1;
static const int g_transparent_model_depth_adjust = 1;
static const int g_transparent_model_z_write_soft_opaque = 0;
static const int g_transparent_thin_stabilization = 1;
static const int g_dc1_accept_flat_3d_geometry = 1;
static const int g_dc1_half_pixel_correction = 1;
static const int g_dc1_fine_vertex_snap = 0;
static const LONG g_initial_frame_summaries = 3;
static const LONG g_frame_summary_interval = 300;

static const float g_max_screen_extent = 8192.0f;
static const float g_max_screen_area = 12000000.0f;
static const float g_min_model_rhw = 0.000001f;
static const float g_max_model_rhw = 2.500f;
static const float g_max_model_rhw_ratio = 4096.0f;
static const float g_max_triangle_aspect = 100.0f;
static const float g_spike_long_extent = 1400.0f;
static const float g_spike_thin_extent = 2.0f;
static const float g_min_depth_variance = 0.000001f;
static const float g_min_rhw_variance = 0.00000001f;
static const float g_model_depth_bias = -0.000012f;
static const float g_transparent_model_depth_bias = -0.000004f;
static const float g_transparent_thin_depth_bias = -0.000007f;
static const float g_model_uv_snap_grid = 255.0f;
static const float g_model_uv_center_grid = 256.0f;
static const float g_model_uv_snap_epsilon = 0.015f;
static const float g_model_subpixel_grid = 16.0f;
static const float g_model_subpixel_epsilon = 0.0315f;
static const float g_dc1_half_pixel_offset = -0.5f;
static const int g_dc1_anchor_stabilization = 0;
static const float g_dc1_anchor_grid = 4.0f;
static const float g_dc1_anchor_max_extent = 520.0f;
static const float g_dc1_anchor_max_area = 180000.0f;
static const float g_dc1_anchor_max_delta = 0.125f;
static const float g_model_screen_expand_pixels = 0.35f;
static const float g_transparent_thin_max_long_extent = 1400.0f;
static const float g_transparent_thin_max_short_extent = 4.0f;
static const float g_transparent_thin_max_area = 24000.0f;
static const float g_transparent_thin_expand_pixels = 0.12f;
static const int g_soft_opaque_alpha_min = 224;

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
  if (!g_diagnostics || !g_log_path[0])
    return;

  char line[1024];
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

static void BuildLogPath(HINSTANCE instance)
{
  DWORD len = GetModuleFileNameA(instance, g_log_path, sizeof(g_log_path));
  if (!len || len >= sizeof(g_log_path))
  {
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
  strncat(g_log_path, "dc1_zfix.log", sizeof(g_log_path) - strlen(g_log_path) - 1);
}

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

  LONG idx = InterlockedIncrement(&g_hook_count) - 1;
  if (idx < 0 || idx >= (LONG)ARRAYSIZE(g_hooks))
    return 0;

  DWORD old_protect = 0;
  if (!VirtualProtect(&vtable[slot], sizeof(void*), PAGE_EXECUTE_READWRITE, &old_protect))
    return 0;

  g_hooks[idx].vtable = vtable;
  g_hooks[idx].slot = slot;
  g_hooks[idx].original = vtable[slot];
  vtable[slot] = hook;

  DWORD ignored = 0;
  VirtualProtect(&vtable[slot], sizeof(void*), old_protect, &ignored);
  FlushInstructionCache(GetCurrentProcess(), &vtable[slot], sizeof(void*));
  LogLine("hook slot=%d vtable=%p original=%p hook=%p", slot, vtable,
          g_hooks[idx].original, hook);
  return 1;
}

static void ReleaseComObject(void* obj)
{
  if (!obj)
    return;
  ComReleaseProc release = (ComReleaseProc)GetVTableSlot(obj, 2);
  if (release)
    release(obj);
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
    const LONG failures = InterlockedIncrement(&g_owned_depth_failures);
    if (failures <= 16)
      LogLine("owned-depth backbuffer fail #%ld hr=0x%08X", failures, (unsigned)hr);
    return 0;
  }

  const int ok = GetSurfaceDescCompat(back_buffer, desc);
  if (!ok)
  {
    const LONG failures = InterlockedIncrement(&g_owned_depth_failures);
    if (failures <= 16)
      LogLine("owned-depth backbuffer desc fail #%ld surface=%p", failures, back_buffer);
  }
  ReleaseComObject(back_buffer);
  return ok;
}

static void LogCurrentDepthSurface(void* self, const char* reason)
{
  static volatile LONG logs = 0;
  const LONG log_id = InterlockedIncrement(&logs);
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
    const LONG sets = InterlockedIncrement(&g_owned_depth_sets);
    if (sets <= 32)
      LogLine("owned-depth set #%ld %s surface=%p size=%ux%u fmt=%lu",
              sets, reason ? reason : "unknown", g_owned_depth_surface,
              g_owned_depth_width, g_owned_depth_height, g_owned_depth_format);
    return 1;
  }

  const LONG failures = InterlockedIncrement(&g_owned_depth_failures);
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
      const LONG failures = InterlockedIncrement(&g_owned_depth_failures);
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

    const LONG creates = InterlockedIncrement(&g_owned_depth_creates);
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

static int IsSupportedModelPrimitive(DWORD primitive_type)
{
  return primitive_type == D3DPT_TRIANGLELIST ||
         primitive_type == D3DPT_TRIANGLESTRIP ||
         primitive_type == D3DPT_TRIANGLEFAN;
}

static int ExceedsModelScreenLimits(const DrawBounds* bounds)
{
  if (!bounds)
    return 1;
  if (g_max_screen_extent > 0.0f &&
      (bounds->width > g_max_screen_extent || bounds->height > g_max_screen_extent))
    return 1;
  if (g_max_screen_area > 0.0f && bounds->area > g_max_screen_area)
    return 1;
  return 0;
}

static int HasValidModelDepthRange(const DrawBounds* bounds)
{
  return bounds && bounds->min_z >= 0.0f && bounds->max_z <= 1.0f;
}

static int HasUsableModelDepthVariance(const DrawBounds* bounds)
{
  if (!bounds)
    return 0;
  if (g_dc1_accept_flat_3d_geometry &&
      (bounds->min_rhw < 0.95f || bounds->max_rhw > 1.05f))
    return 1;
  return (bounds->max_z - bounds->min_z) >= g_min_depth_variance ||
         (bounds->max_rhw - bounds->min_rhw) >= g_min_rhw_variance;
}

static int IsSpikeLikeTriangle(const DrawBounds* bounds)
{
  if (!bounds)
    return 0;

  const float width = AbsF(bounds->width);
  const float height = AbsF(bounds->height);
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

static int IsSoftTransparentThinTriangle(const DrawBounds* bounds)
{
  if (!g_transparent_thin_stabilization || !bounds)
    return 0;

  const float width = AbsF(bounds->width);
  const float height = AbsF(bounds->height);
  const float long_extent = width > height ? width : height;
  const float short_extent = width > height ? height : width;

  if (short_extent <= 0.0001f)
    return 0;
  if (g_transparent_thin_max_long_extent > 0.0f &&
      long_extent > g_transparent_thin_max_long_extent)
    return 0;
  if (g_transparent_thin_max_short_extent > 0.0f &&
      short_extent > g_transparent_thin_max_short_extent)
    return 0;
  if (g_transparent_thin_max_area > 0.0f &&
      bounds->area > g_transparent_thin_max_area)
    return 0;
  return 1;
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
  if (!IsSupportedModelPrimitive(primitive_type))
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

  if (ExceedsModelScreenLimits(&local_bounds))
  {
    if (reason)
      *reason = "transparent_large";
    return 0;
  }
  if (!HasValidModelDepthRange(&local_bounds))
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
  if (IsSpikeLikeTriangle(&local_bounds) &&
      !IsSoftTransparentThinTriangle(&local_bounds))
  {
    if (reason)
      *reason = "transparent_spike";
    return 0;
  }

  if (!HasUsableModelDepthVariance(&local_bounds))
  {
    if (reason)
      *reason = "transparent_flat";
    return 0;
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
  if (!IsSupportedModelPrimitive(primitive_type))
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

  if (ExceedsModelScreenLimits(&local_bounds))
  {
    if (reason)
      *reason = "large";
    return 0;
  }
  if (!HasValidModelDepthRange(&local_bounds))
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

  if (!HasUsableModelDepthVariance(&local_bounds))
  {
    if (reason)
      *reason = "flat";
    return 0;
  }
  return 1;
}

static float ModelDepthBiasForDraw(const DrawBounds* bounds, int transparent, int thin_transparent)
{
  if (!g_model_depth_bias_enabled)
    return 0.0f;

  float bias = transparent ? g_transparent_model_depth_bias : g_model_depth_bias;
  if (thin_transparent)
    bias = g_transparent_thin_depth_bias;

  if (bounds)
  {
    const float z_span = bounds->max_z - bounds->min_z;
    if (z_span < (g_min_depth_variance * 4.0f))
      bias *= 0.5f;
    if (g_max_screen_area > 0.0f && bounds->area > (g_max_screen_area * 0.5f))
      bias *= 0.5f;
  }
  return bias;
}

static DWORD ApplyModelDepth(D3D9TLVERTEX* vertices, DWORD vertex_count, float depth_bias)
{
  if (!vertices || vertex_count == 0 || AbsF(depth_bias) <= 0.0f)
    return 0;

  DWORD changed = 0;
  for (DWORD i = 0; i < vertex_count; i++)
  {
    const float original_z = vertices[i].sz;
    const float new_z = ClampDepth(original_z + depth_bias);
    vertices[i].sz = new_z;
    if (new_z != original_z)
      changed++;
  }

  if (changed)
  {
    InterlockedIncrement(&g_model_depth_biased_draws);
    InterlockedExchangeAdd(&g_model_depth_biased_vertices, (LONG)changed);
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

static DWORD ApplyModelTexCoordCorrection(D3D9TLVERTEX* vertices, DWORD vertex_count)
{
  if (!g_model_uv_correction || !vertices || vertex_count == 0)
    return 0;

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
  }
  return changed;
}

static float SnapScreenCoordToGrid(float value, float grid)
{
  if (grid <= 0.0f)
    return value;

  const float scaled = value * grid;
  const int step = (int)(scaled >= 0.0f ? scaled + 0.5f : scaled - 0.5f);
  return (float)step / grid;
}

static float SnapModelScreenCoord(float value)
{
  const float snapped = SnapScreenCoordToGrid(value, g_model_subpixel_grid);
  if (g_model_subpixel_epsilon >= 0.0f &&
      AbsF(snapped - value) > g_model_subpixel_epsilon)
    return value;
  return snapped;
}

static int IsDc1AnchorStabilizationCandidate(const DrawBounds* bounds)
{
  if (!g_dc1_anchor_stabilization || !bounds || g_dc1_anchor_grid <= 0.0f)
    return 0;

  const float width = AbsF(bounds->width);
  const float height = AbsF(bounds->height);
  const float extent = width > height ? width : height;
  const float area = width * height;
  if (g_dc1_anchor_max_extent > 0.0f && extent <= g_dc1_anchor_max_extent)
    return 1;
  if (g_dc1_anchor_max_area > 0.0f && area <= g_dc1_anchor_max_area)
    return 1;
  return 0;
}

static DWORD ApplyDc1AnchorStabilization(D3D9TLVERTEX* vertices, DWORD vertex_count,
                                         const DrawBounds* bounds)
{
  if (!vertices || vertex_count == 0 || !IsDc1AnchorStabilizationCandidate(bounds))
    return 0;

  const float center_x = (bounds->min_x + bounds->max_x) * 0.5f;
  const float center_y = (bounds->min_y + bounds->max_y) * 0.5f;
  const float snapped_x = SnapScreenCoordToGrid(center_x, g_dc1_anchor_grid);
  const float snapped_y = SnapScreenCoordToGrid(center_y, g_dc1_anchor_grid);
  const float dx = snapped_x - center_x;
  const float dy = snapped_y - center_y;

  if (AbsF(dx) <= 0.00001f && AbsF(dy) <= 0.00001f)
    return 0;
  if (g_dc1_anchor_max_delta > 0.0f &&
      (AbsF(dx) > g_dc1_anchor_max_delta || AbsF(dy) > g_dc1_anchor_max_delta))
    return 0;

  for (DWORD i = 0; i < vertex_count; i++)
  {
    vertices[i].sx += dx;
    vertices[i].sy += dy;
  }
  return vertex_count;
}

static DWORD ApplyDc1HalfPixelCorrection(D3D9TLVERTEX* vertices, DWORD vertex_count)
{
  if (!g_dc1_half_pixel_correction || !vertices || vertex_count == 0 ||
      AbsF(g_dc1_half_pixel_offset) <= 0.0f)
    return 0;

  for (DWORD i = 0; i < vertex_count; i++)
  {
    vertices[i].sx += g_dc1_half_pixel_offset;
    vertices[i].sy += g_dc1_half_pixel_offset;
  }
  return vertex_count;
}

static DWORD ApplyModelSubpixelStabilization(D3D9TLVERTEX* vertices, DWORD vertex_count,
                                             const DrawBounds* bounds)
{
  if (!g_model_subpixel_stabilization || !vertices || vertex_count == 0)
    return 0;

  DWORD changed = ApplyDc1HalfPixelCorrection(vertices, vertex_count);
  changed += ApplyDc1AnchorStabilization(vertices, vertex_count, bounds);
  if (g_dc1_fine_vertex_snap)
  {
    for (DWORD i = 0; i < vertex_count; i++)
    {
      const float old_x = vertices[i].sx;
      const float old_y = vertices[i].sy;
      const float new_x = SnapModelScreenCoord(old_x);
      const float new_y = SnapModelScreenCoord(old_y);
      vertices[i].sx = new_x;
      vertices[i].sy = new_y;
      if (new_x != old_x)
        changed++;
      if (new_y != old_y)
        changed++;
    }
  }

  if (changed)
  {
    InterlockedIncrement(&g_model_subpixel_stabilized_draws);
    InterlockedExchangeAdd(&g_model_subpixel_stabilized_coords, (LONG)changed);
  }
  return changed;
}

static DWORD ApplyModelScreenExpansion(D3D9TLVERTEX* vertices, DWORD vertex_count, float pixels)
{
  if (!vertices || vertex_count < 3 || pixels <= 0.0f)
    return 0;

  float center_x = 0.0f;
  float center_y = 0.0f;
  for (DWORD i = 0; i < vertex_count; i++)
  {
    center_x += vertices[i].sx;
    center_y += vertices[i].sy;
  }
  center_x /= (float)vertex_count;
  center_y /= (float)vertex_count;

  DWORD changed = 0;
  for (DWORD i = 0; i < vertex_count; i++)
  {
    const float dx = vertices[i].sx - center_x;
    const float dy = vertices[i].sy - center_y;
    const float len_sq = (dx * dx) + (dy * dy);
    if (len_sq <= 0.000001f)
      continue;

    const float inv_len = 1.0f / (float)sqrt(len_sq);
    vertices[i].sx += dx * inv_len * pixels;
    vertices[i].sy += dy * inv_len * pixels;
    changed++;
  }
  return changed;
}

static void ApplyModelVertexTweaks(D3D9TLVERTEX* vertices, DWORD vertex_count,
                                   const DrawBounds* bounds, int transparent,
                                   int thin_transparent)
{
  DWORD expanded = 0;
  ApplyModelTexCoordCorrection(vertices, vertex_count);

  if (transparent)
  {
    if (thin_transparent)
      expanded = ApplyModelScreenExpansion(vertices, vertex_count,
                                           g_transparent_thin_expand_pixels);
  }
  else if (g_model_screen_expand)
  {
    expanded = ApplyModelScreenExpansion(vertices, vertex_count,
                                         g_model_screen_expand_pixels);
  }

  if (thin_transparent && expanded)
    InterlockedIncrement(&g_transparent_thin_stabilized_draws);

  ApplyModelSubpixelStabilization(vertices, vertex_count, bounds);
  ApplyModelDepth(vertices, vertex_count,
                  ModelDepthBiasForDraw(bounds, transparent, thin_transparent));
}

static int CaptureRenderState(void* self, DWORD state, DWORD* value)
{
  D3D9GetRenderStateProc get_rs = (D3D9GetRenderStateProc)GetVTableSlot(self, 58);
  return get_rs && SUCCEEDED(get_rs(self, state, value));
}

static int CaptureAlphaBlendEnabled(void* self)
{
  DWORD alpha_blend = 0;
  return CaptureRenderState(self, D3DRS_ALPHABLENDENABLE, &alpha_blend) && alpha_blend != 0;
}

static int CaptureSamplerState(void* self, DWORD sampler, DWORD type, DWORD* value)
{
  D3D9GetSamplerStateProc get_ss = (D3D9GetSamplerStateProc)GetVTableSlot(self, 68);
  return get_ss && SUCCEEDED(get_ss(self, sampler, type, value));
}

static void SetOneRenderState(void* self, DWORD state, DWORD value)
{
  D3D9SetRenderStateProc set_rs = (D3D9SetRenderStateProc)GetVTableSlot(self, 57);
  if (set_rs)
    set_rs(self, state, value);
}

static void SetOneSamplerState(void* self, DWORD sampler, DWORD type, DWORD value)
{
  D3D9SetSamplerStateProc set_ss = (D3D9SetSamplerStateProc)GetVTableSlot(self, 69);
  if (set_ss)
    set_ss(self, sampler, type, value);
}

static void CaptureState(void* self, D3D9StateSnapshot* snapshot)
{
  memset(snapshot, 0, sizeof(*snapshot));
  snapshot->has_z_enable = CaptureRenderState(self, D3DRS_ZENABLE, &snapshot->z_enable);
  snapshot->has_z_write = CaptureRenderState(self, D3DRS_ZWRITEENABLE, &snapshot->z_write);
  snapshot->has_z_func = CaptureRenderState(self, D3DRS_ZFUNC, &snapshot->z_func);
  snapshot->has_alpha_blend = CaptureRenderState(self, D3DRS_ALPHABLENDENABLE, &snapshot->alpha_blend);
  snapshot->has_color_write = CaptureRenderState(self, D3DRS_COLORWRITEENABLE, &snapshot->color_write);
  snapshot->has_shade_mode = CaptureRenderState(self, D3DRS_SHADEMODE, &snapshot->shade_mode);
  snapshot->has_dither_enable = CaptureRenderState(self, D3DRS_DITHERENABLE, &snapshot->dither_enable);
  snapshot->has_sampler_mag = CaptureSamplerState(self, 0, D3DSAMP_MAGFILTER, &snapshot->sampler_mag);
  snapshot->has_sampler_min = CaptureSamplerState(self, 0, D3DSAMP_MINFILTER, &snapshot->sampler_min);
  snapshot->has_sampler_mip = CaptureSamplerState(self, 0, D3DSAMP_MIPFILTER, &snapshot->sampler_mip);
}

static void ForceModelState(void* self, const D3D9StateSnapshot* snapshot)
{
  if (g_force_z_enable && snapshot->has_z_enable)
    SetOneRenderState(self, D3DRS_ZENABLE, 1);
  if (g_force_z_write && snapshot->has_z_write)
    SetOneRenderState(self, D3DRS_ZWRITEENABLE, 1);
  if (snapshot->has_z_func)
    SetOneRenderState(self, D3DRS_ZFUNC, (DWORD)g_force_z_func);
  if (g_opaque_model_disable_alpha_blend && snapshot->has_alpha_blend)
    SetOneRenderState(self, D3DRS_ALPHABLENDENABLE, 0);
  if (g_model_gouraud_shading && snapshot->has_shade_mode)
    SetOneRenderState(self, D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
  if (g_model_dither && snapshot->has_dither_enable)
    SetOneRenderState(self, D3DRS_DITHERENABLE, 1);
  if (g_model_linear_filter && snapshot->has_sampler_mag)
    SetOneSamplerState(self, 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
  if (g_model_linear_filter && snapshot->has_sampler_min)
    SetOneSamplerState(self, 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
  if (g_model_linear_filter && snapshot->has_sampler_mip)
    SetOneSamplerState(self, 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
}

static void ForceTransparentModelState(void* self, const D3D9StateSnapshot* snapshot, int write_depth)
{
  if (g_force_z_enable && snapshot->has_z_enable)
    SetOneRenderState(self, D3DRS_ZENABLE, 1);
  if (snapshot->has_z_write)
    SetOneRenderState(self, D3DRS_ZWRITEENABLE, write_depth ? 1 : 0);
  if (snapshot->has_z_func)
    SetOneRenderState(self, D3DRS_ZFUNC, (DWORD)g_force_z_func);
  if (g_model_gouraud_shading && snapshot->has_shade_mode)
    SetOneRenderState(self, D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
  if (g_model_dither && snapshot->has_dither_enable)
    SetOneRenderState(self, D3DRS_DITHERENABLE, 1);
  if (g_model_linear_filter && snapshot->has_sampler_mag)
    SetOneSamplerState(self, 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
  if (g_model_linear_filter && snapshot->has_sampler_min)
    SetOneSamplerState(self, 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
  if (g_model_linear_filter && snapshot->has_sampler_mip)
    SetOneSamplerState(self, 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
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
  if (snapshot->has_alpha_blend)
    SetOneRenderState(self, D3DRS_ALPHABLENDENABLE, snapshot->alpha_blend);
  if (snapshot->has_color_write)
    SetOneRenderState(self, D3DRS_COLORWRITEENABLE, snapshot->color_write);
  if (snapshot->has_shade_mode)
    SetOneRenderState(self, D3DRS_SHADEMODE, snapshot->shade_mode);
  if (snapshot->has_dither_enable)
    SetOneRenderState(self, D3DRS_DITHERENABLE, snapshot->dither_enable);
  if (snapshot->has_sampler_mag)
    SetOneSamplerState(self, 0, D3DSAMP_MAGFILTER, snapshot->sampler_mag);
  if (snapshot->has_sampler_min)
    SetOneSamplerState(self, 0, D3DSAMP_MINFILTER, snapshot->sampler_min);
  if (snapshot->has_sampler_mip)
    SetOneSamplerState(self, 0, D3DSAMP_MIPFILTER, snapshot->sampler_mip);
}

static HRESULT DrawPrimitiveUPWithModelDepth(void* self, D3D9DrawPrimitiveUPProc orig, DWORD primitive_type,
                                              UINT primitive_count, const void* vertex_data, UINT vertex_stride,
                                              DWORD vertex_count, const DrawBounds* bounds)
{
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
  ApplyModelVertexTweaks(copy, vertex_count, bounds, 0, 0);

  ForceModelState(self, &snapshot);
  HRESULT hr = orig(self, primitive_type, primitive_count, copy, vertex_stride);
  RestoreState(self, &snapshot);
  HeapFree(GetProcessHeap(), 0, copy);
  return hr;
}

static HRESULT DrawPrimitiveUPWithTransparentModelDepth(void* self, D3D9DrawPrimitiveUPProc orig,
                                                        DWORD primitive_type, UINT primitive_count,
                                                        const void* vertex_data, UINT vertex_stride,
                                                        DWORD vertex_count, int min_alpha,
                                                        const DrawBounds* bounds)
{
  D3D9StateSnapshot snapshot;
  CaptureState(self, &snapshot);
  const int write_depth =
    g_transparent_model_z_write_soft_opaque && min_alpha >= g_soft_opaque_alpha_min;
  ForceTransparentModelState(self, &snapshot, write_depth);

  const SIZE_T bytes = (SIZE_T)vertex_stride * (SIZE_T)vertex_count;
  D3D9TLVERTEX* copy = NULL;
  const void* draw_vertices = vertex_data;

  if (g_transparent_model_depth_adjust)
  {
    copy = (D3D9TLVERTEX*)HeapAlloc(GetProcessHeap(), 0, bytes);
    if (copy)
    {
      memcpy(copy, vertex_data, bytes);
      ApplyModelVertexTweaks(copy, vertex_count, bounds, 1,
                             IsSoftTransparentThinTriangle(bounds));
      draw_vertices = copy;
    }
  }

  HRESULT hr = orig(self, primitive_type, primitive_count, draw_vertices, vertex_stride);
  RestoreState(self, &snapshot);
  if (copy)
    HeapFree(GetProcessHeap(), 0, copy);
  return hr;
}

static HRESULT DrawPrimitiveUPWithDc1HalfPixelOnly(void* self, D3D9DrawPrimitiveUPProc orig,
                                                   DWORD primitive_type, UINT primitive_count,
                                                   const void* vertex_data, UINT vertex_stride,
                                                   DWORD vertex_count)
{
  if (!g_dc1_half_pixel_correction || !vertex_data || vertex_count == 0 ||
      vertex_count > 4096 || vertex_stride != sizeof(D3D9TLVERTEX))
    return orig(self, primitive_type, primitive_count, vertex_data, vertex_stride);

  const SIZE_T bytes = (SIZE_T)vertex_stride * (SIZE_T)vertex_count;
  D3D9TLVERTEX* copy = (D3D9TLVERTEX*)HeapAlloc(GetProcessHeap(), 0, bytes);
  if (!copy)
    return orig(self, primitive_type, primitive_count, vertex_data, vertex_stride);

  memcpy(copy, vertex_data, bytes);
  ApplyDc1HalfPixelCorrection(copy, vertex_count);
  HRESULT hr = orig(self, primitive_type, primitive_count, copy, vertex_stride);
  HeapFree(GetProcessHeap(), 0, copy);
  return hr;
}

static HRESULT DrawIndexedPrimitiveUPWithModelDepth(void* self, D3D9DrawIndexedPrimitiveUPProc orig,
                                                    DWORD primitive_type, UINT min_vertex_index,
                                                    UINT num_vertices, UINT primitive_count,
                                                    const void* index_data, DWORD index_format,
                                                    const void* vertex_data, UINT vertex_stride,
                                                    const DrawBounds* bounds)
{
  D3D9StateSnapshot snapshot;
  CaptureState(self, &snapshot);

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
  ApplyModelVertexTweaks(copy, num_vertices, bounds, 0, 0);

  ForceModelState(self, &snapshot);
  HRESULT hr = orig(self, primitive_type, min_vertex_index, num_vertices, primitive_count,
                    index_data, index_format, copy, vertex_stride);
  RestoreState(self, &snapshot);
  HeapFree(GetProcessHeap(), 0, copy);
  return hr;
}

static HRESULT DrawIndexedPrimitiveUPWithTransparentModelDepth(void* self,
                                                              D3D9DrawIndexedPrimitiveUPProc orig,
                                                              DWORD primitive_type,
                                                              UINT min_vertex_index,
                                                              UINT num_vertices,
                                                              UINT primitive_count,
                                                              const void* index_data,
                                                              DWORD index_format,
                                                              const void* vertex_data,
                                                              UINT vertex_stride,
                                                              int min_alpha,
                                                              const DrawBounds* bounds)
{
  D3D9StateSnapshot snapshot;
  CaptureState(self, &snapshot);
  const int write_depth =
    g_transparent_model_z_write_soft_opaque && min_alpha >= g_soft_opaque_alpha_min;
  ForceTransparentModelState(self, &snapshot, write_depth);

  const SIZE_T bytes = (SIZE_T)vertex_stride * (SIZE_T)num_vertices;
  D3D9TLVERTEX* copy = NULL;
  const void* draw_vertices = vertex_data;

  if (g_transparent_model_depth_adjust)
  {
    copy = (D3D9TLVERTEX*)HeapAlloc(GetProcessHeap(), 0, bytes);
    if (copy)
    {
      memcpy(copy, vertex_data, bytes);
      ApplyModelVertexTweaks(copy, num_vertices, bounds, 1,
                             IsSoftTransparentThinTriangle(bounds));
      draw_vertices = copy;
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
  InterlockedIncrement(&g_dpup_total);
  D3D9DrawPrimitiveUPProc orig = (D3D9DrawPrimitiveUPProc)GetOriginal(*(void***)self, 83);
  if (!orig)
    return D3D_OK;

  if (!g_enabled || !vertex_data || vertex_stride != sizeof(D3D9TLVERTEX))
  {
    if (vertex_stride != sizeof(D3D9TLVERTEX))
      InterlockedIncrement(&g_dpup_stride_rejected);
    return orig(self, primitive_type, primitive_count, vertex_data, vertex_stride);
  }

  const DWORD vertex_count = VertexCountForPrimitive(primitive_type, primitive_count);
  if (vertex_count == 0 || vertex_count > 4096)
  {
    InterlockedIncrement(&g_dpup_other_rejected);
    return orig(self, primitive_type, primitive_count, vertex_data, vertex_stride);
  }

  DrawBounds bounds;
  const char* reason = NULL;
  if (!IsModelDepthDraw((const D3D9TLVERTEX*)vertex_data, primitive_type, vertex_count, primitive_count,
                        CaptureAlphaBlendEnabled(self), &bounds, &reason))
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
      InterlockedIncrement(&g_dpup_transparent_accepted);
      return DrawPrimitiveUPWithTransparentModelDepth(self, orig, primitive_type, primitive_count,
                                                     vertex_data, vertex_stride, vertex_count,
                                                     min_alpha, &transparent_bounds);
    }

    if (reason && strcmp(reason, "alpha") == 0)
      InterlockedIncrement(&g_dpup_alpha_rejected);
    else if (reason && strcmp(reason, "axis") == 0)
      InterlockedIncrement(&g_dpup_axis_rejected);
    else if (reason && strcmp(reason, "rhw") == 0)
      InterlockedIncrement(&g_dpup_rhw_rejected);
    else
      InterlockedIncrement(&g_dpup_other_rejected);
    return DrawPrimitiveUPWithDc1HalfPixelOnly(self, orig, primitive_type, primitive_count,
                                               vertex_data, vertex_stride, vertex_count);
  }

  InterlockedIncrement(&g_dpup_accepted);
  return DrawPrimitiveUPWithModelDepth(self, orig, primitive_type, primitive_count, vertex_data,
                                       vertex_stride, vertex_count, &bounds);
}

static HRESULT STDMETHODCALLTYPE Hook_D3D9_SetRenderState(void* self, DWORD state, DWORD value)
{
  InterlockedIncrement(&g_set_render_state_calls);
  D3D9SetRenderStateProc orig = (D3D9SetRenderStateProc)GetOriginal(*(void***)self, 57);
  return orig ? orig(self, state, value) : D3D_OK;
}

static HRESULT STDMETHODCALLTYPE Hook_D3D9_SetTexture(void* self, DWORD stage, void* texture)
{
  InterlockedIncrement(&g_set_texture_calls);
  D3D9SetTextureProc orig = (D3D9SetTextureProc)GetOriginal(*(void***)self, 65);
  if (stage == 0)
  {
    if (g_current_texture0 != texture)
    {
      g_current_texture0 = texture;
      InterlockedIncrement(&g_set_texture0_changes);
    }
  }
  return orig ? orig(self, stage, texture) : D3D_OK;
}

static HRESULT STDMETHODCALLTYPE Hook_D3D9_SetFVF(void* self, DWORD fvf)
{
  InterlockedIncrement(&g_set_fvf_calls);
  D3D9SetFVFProc orig = (D3D9SetFVFProc)GetOriginal(*(void***)self, 89);
  if (g_current_fvf != fvf)
    g_current_fvf = fvf;
  return orig ? orig(self, fvf) : D3D_OK;
}

static HRESULT STDMETHODCALLTYPE Hook_D3D9_DrawPrimitive(void* self, DWORD primitive_type, UINT start_vertex,
                                                        UINT primitive_count)
{
  InterlockedIncrement(&g_dp_calls);
  D3D9DrawPrimitiveProc orig = (D3D9DrawPrimitiveProc)GetOriginal(*(void***)self, 81);
  return orig ? orig(self, primitive_type, start_vertex, primitive_count) : D3D_OK;
}

static HRESULT STDMETHODCALLTYPE Hook_D3D9_DrawIndexedPrimitive(void* self, DWORD primitive_type,
                                                               INT base_vertex_index, UINT min_vertex_index,
                                                               UINT num_vertices, UINT start_index,
                                                               UINT primitive_count)
{
  InterlockedIncrement(&g_dip_calls);
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
  InterlockedIncrement(&g_dipup_total);
  D3D9DrawIndexedPrimitiveUPProc orig =
    (D3D9DrawIndexedPrimitiveUPProc)GetOriginal(*(void***)self, 84);
  if (!orig)
    return D3D_OK;

  const DWORD index_count = VertexCountForPrimitive(primitive_type, primitive_count);
  if (!g_enabled || !vertex_data || !index_data || vertex_stride != sizeof(D3D9TLVERTEX) ||
      (index_format != D3DFMT_INDEX16 && index_format != D3DFMT_INDEX32) ||
      num_vertices == 0 || num_vertices > 4096 || index_count == 0 || index_count > 8192)
  {
    InterlockedIncrement(&g_dipup_rejected);
    return orig(self, primitive_type, min_vertex_index, num_vertices, primitive_count,
                index_data, index_format, vertex_data, vertex_stride);
  }

  const D3D9TLVERTEX* vertices = (const D3D9TLVERTEX*)vertex_data;
  D3D9TLVERTEX* indexed = BuildIndexedVertexList(vertices, num_vertices, index_data,
                                                 index_format, index_count);
  if (!indexed)
  {
    InterlockedIncrement(&g_dipup_rejected);
    return orig(self, primitive_type, min_vertex_index, num_vertices, primitive_count,
                index_data, index_format, vertex_data, vertex_stride);
  }

  DrawBounds bounds;
  const char* reason = NULL;
  if (!IsModelDepthDraw(indexed, primitive_type, index_count, primitive_count,
                        CaptureAlphaBlendEnabled(self), &bounds, &reason))
  {
    DrawBounds transparent_bounds;
    int min_alpha = 255;
    const char* transparent_reason = NULL;
    if (reason && strcmp(reason, "alpha") == 0 &&
        IsTransparentModelDepthDraw(indexed, primitive_type, index_count,
                                    primitive_count, &transparent_bounds, &min_alpha,
                                    &transparent_reason))
    {
      (void)transparent_reason;
      InterlockedIncrement(&g_dipup_transparent_accepted);
      HeapFree(GetProcessHeap(), 0, indexed);
      return DrawIndexedPrimitiveUPWithTransparentModelDepth(self, orig, primitive_type,
                                                            min_vertex_index, num_vertices,
                                                            primitive_count, index_data,
                                                            index_format, vertex_data,
                                                            vertex_stride, min_alpha,
                                                            &transparent_bounds);
    }

    InterlockedIncrement(&g_dipup_rejected);
    HeapFree(GetProcessHeap(), 0, indexed);
    return orig(self, primitive_type, min_vertex_index, num_vertices, primitive_count,
                index_data, index_format, vertex_data, vertex_stride);
  }

  InterlockedIncrement(&g_dipup_accepted);
  HeapFree(GetProcessHeap(), 0, indexed);
  return DrawIndexedPrimitiveUPWithModelDepth(self, orig, primitive_type, min_vertex_index,
                                              num_vertices, primitive_count, index_data,
                                              index_format, vertex_data, vertex_stride, &bounds);
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
    const LONG cleared = InterlockedIncrement(&g_depth_clear_count);
    if (cleared <= 16)
      LogLine("depth-clear #%ld ok", cleared);
  }
  else
  {
    const LONG failed = InterlockedIncrement(&g_depth_clear_failures);
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
  const LONG frame = InterlockedIncrement(&g_frame_counter);
  if (frame <= g_initial_frame_summaries ||
      (g_frame_summary_interval > 0 && (frame % g_frame_summary_interval) == 0))
  {
    LogLine("frame=%ld dp=%ld dip=%ld dpup=%ld accepted=%ld transparent=%ld "
            "dipup=%ld dipupAccepted=%ld dipupTransparent=%ld dipupRejected=%ld "
            "depthClear=%ld depthFail=%ld uvDraws=%ld uvCoords=%ld "
            "subpixDraws=%ld subpixCoords=%ld depthBiasDraws=%ld depthBiasVerts=%ld "
            "thinTransparent=%ld "
            "stride=%ld alpha=%ld axis=%ld rhw=%ld other=%ld "
            "setTex=%ld tex0Changes=%ld setFVF=%ld setRS=%ld curTex0=%p curFVF=0x%lX "
            "ownedDepthCreate=%ld ownedDepthSet=%ld ownedDepthFail=%ld",
            frame, g_dp_calls, g_dip_calls, g_dpup_total, g_dpup_accepted,
            g_dpup_transparent_accepted, g_dipup_total, g_dipup_accepted,
            g_dipup_transparent_accepted, g_dipup_rejected,
            g_depth_clear_count, g_depth_clear_failures,
            g_model_uv_corrected_draws, g_model_uv_corrected_coords,
            g_model_subpixel_stabilized_draws, g_model_subpixel_stabilized_coords,
            g_model_depth_biased_draws, g_model_depth_biased_vertices,
            g_transparent_thin_stabilized_draws,
            g_dpup_stride_rejected, g_dpup_alpha_rejected, g_dpup_axis_rejected,
            g_dpup_rhw_rejected, g_dpup_other_rejected,
            g_set_texture_calls, g_set_texture0_changes, g_set_fvf_calls,
            g_set_render_state_calls, g_current_texture0, g_current_fvf,
            g_owned_depth_creates, g_owned_depth_sets, g_owned_depth_failures);
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
    DeleteFileA(g_log_path);
    DisableThreadLibraryCalls(instance);
    LogLine("dc1_zfix loaded");
    LogLine("geometry-stabilizer subpixel=%d halfPixel=%d/%.2f linearFilter=%d "
            "fineSnap=%d grid=%.1f eps=%.4f depthBias=%d opaque=%.7f "
            "transparent=%.7f thinTransparent=%.7f thinExpand=%.3f anchor=%d/%.1f",
            g_model_subpixel_stabilization, g_dc1_half_pixel_correction,
            g_dc1_half_pixel_offset, g_model_linear_filter, g_dc1_fine_vertex_snap,
            g_model_subpixel_grid, g_model_subpixel_epsilon, g_model_depth_bias_enabled,
            g_model_depth_bias, g_transparent_model_depth_bias,
            g_transparent_thin_depth_bias, g_transparent_thin_expand_pixels,
            g_dc1_anchor_stabilization, g_dc1_anchor_grid);
    PatchAllImports();
  }
  return TRUE;
}
