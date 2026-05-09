#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "zfix_hooks.h"
#include "zfix_log.h"

#ifndef ARRAYSIZE
#define ARRAYSIZE(a) (sizeof(a) / sizeof((a)[0]))
#endif

#define DDSCAPS_BACKBUFFER 0x00000004u
#define DDSCAPS_FRONTBUFFER 0x00000020u
#define DDSCAPS_PRIMARYSURFACE 0x00000200u
#define DDSCAPS_TEXTURE 0x00001000u
#define DDSCAPS_ZBUFFER 0x00020000u

#define STARTUP_DISCLAIMER_DURATION_MS 5000u
#define STARTUP_DISCLAIMER_FADE_IN_MS 650u
#define STARTUP_DISCLAIMER_FADE_OUT_MS 750u
#define STARTUP_DISCLAIMER_FRAME_SLEEP_MS 8u
#define STARTUP_DISCLAIMER_MAX_DRAWS 65536
#define STARTUP_DISCLAIMER_MAX_PRESENTS 1800
#define STARTUP_DISCLAIMER_SURFACE_SLOTS 3
#define STARTUP_DISCLAIMER_SKIP_KEY 'J'
#define STARTUP_DISCLAIMER_KEY_ACTION_SLOT 10
#define STARTUP_DISCLAIMER_JOY_ACTION_SLOT 4
#define STARTUP_DISCLAIMER_MAX_ACTION_KEYS 8
#define STARTUP_DISCLAIMER_MAX_ACTION_BUTTONS 16

#define XINPUT_GAMEPAD_X 0x4000u
#define JOY_RETURNBUTTONS 0x00000080u
#define JOYERR_NOERROR 0u
#define JOY_BUTTON1 0x00000001u

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

typedef struct XINPUT_GAMEPAD_COMPAT {
  WORD wButtons;
  BYTE bLeftTrigger;
  BYTE bRightTrigger;
  SHORT sThumbLX;
  SHORT sThumbLY;
  SHORT sThumbRX;
  SHORT sThumbRY;
} XINPUT_GAMEPAD_COMPAT;

typedef struct XINPUT_STATE_COMPAT {
  DWORD dwPacketNumber;
  XINPUT_GAMEPAD_COMPAT Gamepad;
} XINPUT_STATE_COMPAT;

typedef struct JOYINFOEX_COMPAT {
  DWORD dwSize;
  DWORD dwFlags;
  DWORD dwXpos;
  DWORD dwYpos;
  DWORD dwZpos;
  DWORD dwRpos;
  DWORD dwUpos;
  DWORD dwVpos;
  DWORD dwButtons;
  DWORD dwButtonNumber;
  DWORD dwPOV;
  DWORD dwReserved1;
  DWORD dwReserved2;
} JOYINFOEX_COMPAT;

typedef struct StartupDisclaimerImage {
  BYTE* bgra;
  BYTE* fade_bgra;
  int width;
  int height;
  BYTE fade_alpha;
  char path[MAX_PATH];
} StartupDisclaimerImage;

typedef struct StartupDisclaimerFrameBuffer {
  HDC dc;
  HBITMAP bitmap;
  HGDIOBJ old_bitmap;
  int width;
  int height;
} StartupDisclaimerFrameBuffer;

typedef HRESULT(WINAPI* DirectDrawCreateProc)(GUID* lpGUID, void** lplpDD, void* pUnkOuter);
typedef HRESULT(STDMETHODCALLTYPE* DirectDrawCreateSurfaceProc)(void* self, DDSURFACEDESC_COMPAT* desc,
                                                                void** surface, void* outer);
typedef HRESULT(STDMETHODCALLTYPE* DDSurfaceBltProc)(void* self, RECT* dst, void* src, RECT* src_rect,
                                                     DWORD flags, void* fx);
typedef HRESULT(STDMETHODCALLTYPE* DDSurfaceBltFastProc)(void* self, DWORD x, DWORD y, void* src,
                                                         RECT* src_rect, DWORD trans);
typedef HRESULT(STDMETHODCALLTYPE* DDSurfaceFlipProc)(void* self, void* target_override, DWORD flags);
typedef HRESULT(STDMETHODCALLTYPE* DDSurfaceGetSurfaceDescProc)(void* self, DDSURFACEDESC_COMPAT* desc);
typedef HRESULT(STDMETHODCALLTYPE* QueryInterfaceProc)(void* self, REFIID riid, void** ppvObj);
typedef uint8_t* (*WebPDecodeBGRAProc)(const uint8_t* data, size_t data_size, int* width, int* height);
typedef void (*WebPFreeProc)(void* ptr);
typedef DWORD(WINAPI* XInputGetStateProc)(DWORD index, XINPUT_STATE_COMPAT* state);
typedef UINT(WINAPI* JoyGetNumDevsProc)(void);
typedef UINT(WINAPI* JoyGetPosExProc)(UINT joy_id, JOYINFOEX_COMPAT* info);

static const GUID kIID_IDirectDraw2 =
  {0xB3A6F3E0, 0x2B43, 0x11CF, {0xA2, 0xDE, 0x00, 0xAA, 0x00, 0xB9, 0x33, 0x56}};

static HookEntry g_hooks[64];
static volatile LONG g_hook_count = 0;
static DirectDrawCreateProc g_real_direct_draw_create = NULL;
static char g_game_dir[MAX_PATH];
static char g_log_path[MAX_PATH];
static int g_log_enabled = 0;
static HWND g_game_window = NULL;

static StartupDisclaimerImage g_image;
static StartupDisclaimerFrameBuffer g_frame;
static HMODULE g_webp_module = NULL;
static WebPDecodeBGRAProc g_webp_decode_bgra = NULL;
static WebPFreeProc g_webp_free = NULL;
static HMODULE g_xinput_module = NULL;
static XInputGetStateProc g_xinput_get_state = NULL;
static HMODULE g_winmm_module = NULL;
static JoyGetNumDevsProc g_joy_get_num_devs = NULL;
static JoyGetPosExProc g_joy_get_pos_ex = NULL;

static DWORD g_start_tick = 0;
static WORD g_action_keys[STARTUP_DISCLAIMER_MAX_ACTION_KEYS];
static DWORD g_action_buttons[STARTUP_DISCLAIMER_MAX_ACTION_BUTTONS];
static volatile LONG g_load_state = 0;
static volatile LONG g_config_state = 0;
static volatile LONG g_action_key_count = 0;
static volatile LONG g_action_button_count = 0;
static volatile LONG g_surface_seen = 0;
static volatile LONG g_surface_patched = 0;
static volatile LONG g_draws = 0;
static volatile LONG g_presents = 0;
static volatile LONG g_hold_done = 0;
static volatile LONG g_retired = 0;
static volatile LONG g_draw_logged = 0;
static volatile LONG g_failure_logged = 0;
static volatile LONG g_window_logged = 0;
static volatile LONG g_window_width = 0;
static volatile LONG g_window_height = 0;
static volatile LONG g_layout_changes = 0;

static HRESULT STDMETHODCALLTYPE Hook_DD_CreateSurface(void* self, DDSURFACEDESC_COMPAT* desc,
                                                       void** surface, void* outer);
static HRESULT STDMETHODCALLTYPE Hook_DDSurface_Blt(void* self, RECT* dst, void* src, RECT* src_rect,
                                                    DWORD flags, void* fx);
static HRESULT STDMETHODCALLTYPE Hook_DDSurface_BltFast(void* self, DWORD x, DWORD y, void* src,
                                                        RECT* src_rect, DWORD trans);
static HRESULT STDMETHODCALLTYPE Hook_DDSurface_Flip(void* self, void* target_override, DWORD flags);
static HRESULT STDMETHODCALLTYPE Hook_QueryInterface(void* self, REFIID riid, void** ppvObj);
static void ReleaseFrameBuffer(void);
static void LogLine(const char* fmt, ...);
static int DrawDisclaimer(void* surface, int present_event);

static void* GetOriginal(void** vtable, int slot)
{
  return ZfixGetOriginal(g_hooks, &g_hook_count, vtable, slot);
}

static int PatchVTableSlot(void* obj, int slot, void* hook)
{
  return ZfixPatchVTableSlot(g_hooks, (LONG)ARRAYSIZE(g_hooks),
                             &g_hook_count, obj, slot, hook);
}

static int ValidDisplayExtent(DWORD width, DWORD height)
{
  return width > 0 && height > 0 && width <= 8192 && height <= 8192;
}

static void MarkLayoutChange(void)
{
  ReleaseFrameBuffer();
  InterlockedIncrement(&g_layout_changes);
}

static void BuildLogPath(HINSTANCE instance)
{
  ZfixBuildLogPath(instance, g_game_dir, sizeof(g_game_dir), g_log_path,
                   sizeof(g_log_path), "re1_startup_disclaimer.log");
}

static void LogLine(const char* fmt, ...)
{
  if (!g_log_enabled)
    return;

  va_list args;
  va_start(args, fmt);
  ZfixLogLineV(1, g_log_path, fmt, args);
  va_end(args);
}

static void BuildGamePath(const char* relative_path, char* out, size_t out_size)
{
  if (!out || out_size == 0)
    return;

  out[0] = '\0';
  if (!relative_path || !relative_path[0])
    return;

  if (g_game_dir[0])
    snprintf(out, out_size, "%s\\%s", g_game_dir, relative_path);
  else
    snprintf(out, out_size, "%s", relative_path);
  out[out_size - 1] = '\0';
}

static int ReadWholeFile(const char* path, BYTE** out_data, DWORD* out_size)
{
  if (!path || !out_data || !out_size)
    return 0;

  *out_data = NULL;
  *out_size = 0;

  HANDLE file = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
                            FILE_ATTRIBUTE_NORMAL, NULL);
  if (file == INVALID_HANDLE_VALUE)
    return 0;

  DWORD high_size = 0;
  SetLastError(NO_ERROR);
  const DWORD size = GetFileSize(file, &high_size);
  if ((size == INVALID_FILE_SIZE && GetLastError() != NO_ERROR) ||
      high_size != 0 || size == 0 || size > (64u * 1024u * 1024u))
  {
    CloseHandle(file);
    return 0;
  }

  BYTE* data = (BYTE*)HeapAlloc(GetProcessHeap(), 0, size);
  if (!data)
  {
    CloseHandle(file);
    return 0;
  }

  DWORD read = 0;
  const BOOL ok = ReadFile(file, data, size, &read, NULL);
  CloseHandle(file);
  if (!ok || read != size)
  {
    HeapFree(GetProcessHeap(), 0, data);
    return 0;
  }

  *out_data = data;
  *out_size = size;
  return 1;
}

static int FindDisclaimerPath(char* out, size_t out_size)
{
  static const char* kCandidates[] = {
    "hires\\misc\\startup_disclaimer.webp",
    "hires\\misc\\re1_disclaimer.webp",
    "hires\\misc\\disclaimer.webp",
    "bio1dc\\hires\\misc\\startup_disclaimer.webp",
    "bio1dc\\hires\\misc\\re1_disclaimer.webp",
    "bio1dc\\hires\\misc\\disclaimer.webp",
  };

  if (!out || out_size == 0)
    return 0;

  for (DWORD i = 0; i < ARRAYSIZE(kCandidates); i++)
  {
    char path[MAX_PATH];
    BuildGamePath(kCandidates[i], path, sizeof(path));
    const DWORD attrs = GetFileAttributesA(path);
    if (attrs != INVALID_FILE_ATTRIBUTES && !(attrs & FILE_ATTRIBUTE_DIRECTORY))
    {
      snprintf(out, out_size, "%s", path);
      out[out_size - 1] = '\0';
      return 1;
    }
  }

  out[0] = '\0';
  return 0;
}

static int LoadWebP(void)
{
  if (g_webp_module && g_webp_decode_bgra && g_webp_free)
    return 1;

  char dll_path[MAX_PATH];
  BuildGamePath("libwebp.dll", dll_path, sizeof(dll_path));
  HMODULE module = LoadLibraryA(dll_path);
  if (!module)
    module = LoadLibraryA("libwebp.dll");
  if (!module)
    return 0;

  WebPDecodeBGRAProc decode_bgra =
    (WebPDecodeBGRAProc)GetProcAddress(module, "WebPDecodeBGRA");
  WebPFreeProc free_proc = (WebPFreeProc)GetProcAddress(module, "WebPFree");
  if (!decode_bgra || !free_proc)
  {
    FreeLibrary(module);
    return 0;
  }

  g_webp_module = module;
  g_webp_decode_bgra = decode_bgra;
  g_webp_free = free_proc;
  return 1;
}

static void ReleaseFrameBuffer(void)
{
  if (g_frame.dc)
  {
    if (g_frame.old_bitmap)
      SelectObject(g_frame.dc, g_frame.old_bitmap);
    if (g_frame.bitmap)
      DeleteObject(g_frame.bitmap);
    DeleteDC(g_frame.dc);
  }

  memset(&g_frame, 0, sizeof(g_frame));
}

static int EnsureFrameBuffer(HDC ref_dc, int width, int height)
{
  if (!ref_dc || width <= 0 || height <= 0 || width > 8192 || height > 8192)
    return 0;

  if (g_frame.dc && g_frame.bitmap && g_frame.width == width && g_frame.height == height)
    return 1;

  ReleaseFrameBuffer();

  BITMAPINFO bmi;
  memset(&bmi, 0, sizeof(bmi));
  bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bmi.bmiHeader.biWidth = width;
  bmi.bmiHeader.biHeight = -height;
  bmi.bmiHeader.biPlanes = 1;
  bmi.bmiHeader.biBitCount = 32;
  bmi.bmiHeader.biCompression = BI_RGB;

  HDC mem_dc = CreateCompatibleDC(ref_dc);
  if (!mem_dc)
    return 0;

  void* bits = NULL;
  HBITMAP bitmap = CreateDIBSection(ref_dc, &bmi, DIB_RGB_COLORS, &bits, NULL, 0);
  if (!bitmap || !bits)
  {
    if (bitmap)
      DeleteObject(bitmap);
    DeleteDC(mem_dc);
    return 0;
  }

  HGDIOBJ old_bitmap = SelectObject(mem_dc, bitmap);
  if (!old_bitmap || old_bitmap == HGDI_ERROR)
  {
    DeleteObject(bitmap);
    DeleteDC(mem_dc);
    return 0;
  }

  g_frame.dc = mem_dc;
  g_frame.bitmap = bitmap;
  g_frame.old_bitmap = old_bitmap;
  g_frame.width = width;
  g_frame.height = height;
  return 1;
}

static int ValidDisclaimerImageSize(int width, int height)
{
  return width > 0 && height > 0 && width <= 8192 && height <= 8192;
}

static int EnsureImage(void)
{
  const LONG state = g_load_state;
  if (state == 1)
    return 1;
  if (state != 0)
    return 0;
  if (InterlockedCompareExchange(&g_load_state, 2, 0) != 0)
    return 0;

  int ok = 0;
  BYTE* file_data = NULL;
  DWORD file_size = 0;
  int width = 0;
  int height = 0;

  if (!FindDisclaimerPath(g_image.path, sizeof(g_image.path)))
  {
    LogLine("startup disclaimer skipped: no hires\\misc\\startup_disclaimer.webp");
    goto done;
  }

  if (!LoadWebP())
  {
    LogLine("startup disclaimer skipped: libwebp.dll unavailable");
    goto done;
  }

  if (!ReadWholeFile(g_image.path, &file_data, &file_size))
  {
    LogLine("startup disclaimer skipped: failed to read %s", g_image.path);
    goto done;
  }

  BYTE* bgra = g_webp_decode_bgra(file_data, (size_t)file_size, &width, &height);
  if (!bgra || !ValidDisclaimerImageSize(width, height))
  {
    if (bgra)
      g_webp_free(bgra);
    LogLine("startup disclaimer skipped: failed to decode %s wh=%dx%d",
            g_image.path, width, height);
    goto done;
  }

  g_image.bgra = bgra;
  g_image.width = width;
  g_image.height = height;
  LogLine("startup disclaimer loaded path=%s wh=%dx%d duration=%ums",
          g_image.path, width, height, STARTUP_DISCLAIMER_DURATION_MS);
  ok = 1;

done:
  if (file_data)
    HeapFree(GetProcessHeap(), 0, file_data);
  InterlockedExchange(&g_load_state, ok ? 1 : -1);
  return ok;
}

static void ReleaseImage(void)
{
  if (g_image.bgra && g_webp_free)
    g_webp_free(g_image.bgra);
  if (g_image.fade_bgra)
    HeapFree(GetProcessHeap(), 0, g_image.fade_bgra);
  memset(&g_image, 0, sizeof(g_image));
  ReleaseFrameBuffer();

  g_webp_decode_bgra = NULL;
  g_webp_free = NULL;
  if (g_webp_module)
  {
    FreeLibrary(g_webp_module);
    g_webp_module = NULL;
  }
}

static int HexNibble(char c)
{
  if (c >= '0' && c <= '9')
    return c - '0';
  if (c >= 'a' && c <= 'f')
    return 10 + c - 'a';
  if (c >= 'A' && c <= 'F')
    return 10 + c - 'A';
  return -1;
}

static int ParseHexBytes(const char* text, BYTE* out, int out_capacity)
{
  if (!text || !out || out_capacity <= 0)
    return 0;

  int count = 0;
  const char* p = text;
  while (*p && count < out_capacity)
  {
    while (*p == ' ' || *p == '\t' || *p == ',' || *p == ';')
      p++;
    const int hi = HexNibble(p[0]);
    const int lo = HexNibble(p[1]);
    if (hi < 0 || lo < 0)
      break;
    out[count++] = (BYTE)((hi << 4) | lo);
    p += 2;
  }

  return count;
}

static LONG ClampListCount(LONG count, LONG max_count)
{
  if (count < 0)
    return 0;
  if (count > max_count)
    return max_count;
  return count;
}

static void AddActionKey(WORD key)
{
  if (!key)
    return;

  LONG count = ClampListCount(g_action_key_count, STARTUP_DISCLAIMER_MAX_ACTION_KEYS);
  for (LONG i = 0; i < count; i++)
  {
    if (g_action_keys[i] == key)
      return;
  }
  if (count >= STARTUP_DISCLAIMER_MAX_ACTION_KEYS)
    return;

  const LONG idx = InterlockedIncrement(&g_action_key_count) - 1;
  if (idx >= 0 && idx < STARTUP_DISCLAIMER_MAX_ACTION_KEYS)
    g_action_keys[idx] = key;
}

static void AddActionButton(DWORD mask)
{
  if (!mask)
    return;

  LONG count = ClampListCount(g_action_button_count, STARTUP_DISCLAIMER_MAX_ACTION_BUTTONS);
  for (LONG i = 0; i < count; i++)
  {
    if (g_action_buttons[i] == mask)
      return;
  }
  if (count >= STARTUP_DISCLAIMER_MAX_ACTION_BUTTONS)
    return;

  const LONG idx = InterlockedIncrement(&g_action_button_count) - 1;
  if (idx >= 0 && idx < STARTUP_DISCLAIMER_MAX_ACTION_BUTTONS)
    g_action_buttons[idx] = mask;
}

static void AddDefaultControls(void)
{
  AddActionKey(STARTUP_DISCLAIMER_SKIP_KEY);
  AddActionButton(JOY_BUTTON1);
  AddActionButton(0x0004u);
}

static int ConfigNameEquals(const char* line, const char* eq, const char* expected)
{
  const char* end = eq;
  while (end > line && (end[-1] == ' ' || end[-1] == '\t'))
    end--;

  const size_t expected_len = strlen(expected);
  return (size_t)(end - line) == expected_len && memcmp(line, expected, expected_len) == 0;
}

static void ParseConfigLine(char* line)
{
  if (!line)
    return;

  while (*line == ' ' || *line == '\t')
    line++;

  char* eq = strchr(line, '=');
  if (!eq)
    return;
  char* value = eq + 1;
  while (*value == ' ' || *value == '\t')
    value++;

  BYTE bytes[128];
  const int byte_count = ParseHexBytes(value, bytes, (int)sizeof(bytes));
  if (byte_count <= 0)
    return;

  if (ConfigNameEquals(line, eq, "Key_Def"))
  {
    const int offset = STARTUP_DISCLAIMER_KEY_ACTION_SLOT * 2;
    if (byte_count >= offset + 2)
    {
      const WORD key = (WORD)(bytes[offset] | ((WORD)bytes[offset + 1] << 8));
      AddActionKey(key);
    }
    return;
  }

  if (ConfigNameEquals(line, eq, "Joy_Def"))
  {
    const int offset = STARTUP_DISCLAIMER_JOY_ACTION_SLOT * 2;
    if (byte_count >= offset + 2)
    {
      const WORD mask = (WORD)(bytes[offset] | ((WORD)bytes[offset + 1] << 8));
      AddActionButton(mask);
    }
  }
}

static void EnsureControls(void)
{
  if (g_config_state == 1)
    return;
  if (InterlockedCompareExchange(&g_config_state, 2, 0) != 0)
    return;

  AddDefaultControls();

  BYTE* raw = NULL;
  DWORD raw_size = 0;
  char path[MAX_PATH];
  BuildGamePath("config.ini", path, sizeof(path));
  if (ReadWholeFile(path, &raw, &raw_size))
  {
    char* text = (char*)HeapAlloc(GetProcessHeap(), 0, (SIZE_T)raw_size + 1u);
    if (text)
    {
      memcpy(text, raw, raw_size);
      text[raw_size] = '\0';

      char* line = text;
      while (line && *line)
      {
        char* next = strpbrk(line, "\r\n");
        if (next)
        {
          *next++ = '\0';
          while (*next == '\r' || *next == '\n')
            next++;
        }

        ParseConfigLine(line);
        line = next;
      }

      HeapFree(GetProcessHeap(), 0, text);
    }
    HeapFree(GetProcessHeap(), 0, raw);
  }

  InterlockedExchange(&g_config_state, 1);
  LogLine("startup disclaimer controls keys=%ld buttons=%ld",
          ClampListCount(g_action_key_count, STARTUP_DISCLAIMER_MAX_ACTION_KEYS),
          ClampListCount(g_action_button_count, STARTUP_DISCLAIMER_MAX_ACTION_BUTTONS));
}

static int EnsureXInput(void)
{
  if (g_xinput_module && g_xinput_get_state)
    return 1;

  static const char* kXInputDlls[] = {
    "xinput1_4.dll",
    "xinput1_3.dll",
    "xinput9_1_0.dll",
  };

  for (DWORD i = 0; i < ARRAYSIZE(kXInputDlls); i++)
  {
    HMODULE module = LoadLibraryA(kXInputDlls[i]);
    if (!module)
      continue;

    XInputGetStateProc get_state =
      (XInputGetStateProc)GetProcAddress(module, "XInputGetState");
    if (!get_state)
    {
      FreeLibrary(module);
      continue;
    }

    g_xinput_module = module;
    g_xinput_get_state = get_state;
    return 1;
  }

  return 0;
}

static int EnsureJoy(void)
{
  if (g_winmm_module && g_joy_get_num_devs && g_joy_get_pos_ex)
    return 1;

  HMODULE module = LoadLibraryA("winmm.dll");
  if (!module)
    return 0;

  JoyGetNumDevsProc get_num_devs =
    (JoyGetNumDevsProc)GetProcAddress(module, "joyGetNumDevs");
  JoyGetPosExProc get_pos_ex =
    (JoyGetPosExProc)GetProcAddress(module, "joyGetPosEx");
  if (!get_num_devs || !get_pos_ex)
  {
    FreeLibrary(module);
    return 0;
  }

  g_winmm_module = module;
  g_joy_get_num_devs = get_num_devs;
  g_joy_get_pos_ex = get_pos_ex;
  return 1;
}

static int SkipPressed(void)
{
  EnsureControls();

  LONG key_count = ClampListCount(g_action_key_count, STARTUP_DISCLAIMER_MAX_ACTION_KEYS);
  for (LONG i = 0; i < key_count; i++)
  {
    const WORD key = g_action_keys[i];
    if (key && (GetAsyncKeyState((int)key) & 0x8000))
      return 1;
  }

  if (EnsureXInput())
  {
    for (DWORD i = 0; i < 4; i++)
    {
      XINPUT_STATE_COMPAT state;
      memset(&state, 0, sizeof(state));
      if (g_xinput_get_state(i, &state) == ERROR_SUCCESS &&
          (state.Gamepad.wButtons & XINPUT_GAMEPAD_X))
      {
        return 1;
      }
    }
  }

  if (EnsureJoy())
  {
    const UINT count = g_joy_get_num_devs();
    LONG button_count = ClampListCount(g_action_button_count, STARTUP_DISCLAIMER_MAX_ACTION_BUTTONS);
    for (UINT i = 0; i < count && i < 16u; i++)
    {
      JOYINFOEX_COMPAT info;
      memset(&info, 0, sizeof(info));
      info.dwSize = sizeof(info);
      info.dwFlags = JOY_RETURNBUTTONS;
      if (g_joy_get_pos_ex(i, &info) != JOYERR_NOERROR)
        continue;

      for (LONG j = 0; j < button_count; j++)
      {
        const DWORD mask = g_action_buttons[j];
        if (mask && (info.dwButtons & mask))
          return 1;
      }
    }
  }

  return 0;
}

static void ReleaseInput(void)
{
  g_xinput_get_state = NULL;
  if (g_xinput_module)
  {
    FreeLibrary(g_xinput_module);
    g_xinput_module = NULL;
  }

  g_joy_get_num_devs = NULL;
  g_joy_get_pos_ex = NULL;
  if (g_winmm_module)
  {
    FreeLibrary(g_winmm_module);
    g_winmm_module = NULL;
  }
}

static int IsDisclaimerSurfaceDesc(const DDSURFACEDESC_COMPAT* desc)
{
  if (!desc)
    return 0;

  const DWORD caps = desc->ddsCaps.dwCaps;
  if (caps & (DDSCAPS_TEXTURE | DDSCAPS_ZBUFFER))
    return 0;

  return (caps & (DDSCAPS_PRIMARYSURFACE | DDSCAPS_FRONTBUFFER | DDSCAPS_BACKBUFFER)) != 0;
}

static int RectWidth(const RECT* rect)
{
  return rect ? (int)(rect->right - rect->left) : 0;
}

static int RectHeight(const RECT* rect)
{
  return rect ? (int)(rect->bottom - rect->top) : 0;
}

static int DrawDisclaimerGdiToDC(HDC dc, const BYTE* pixels, int image_w,
                                 int image_h, int surface_w, int surface_h,
                                 int* out_dst_w, int* out_dst_h,
                                 int* out_dst_x, int* out_dst_y)
{
  if (!dc || !pixels || image_w <= 0 || image_h <= 0 ||
      !ValidDisplayExtent((DWORD)surface_w, (DWORD)surface_h))
    return 0;

  int dst_w = surface_w;
  int dst_h = (int)(((int64_t)surface_w * image_h) / image_w);
  if (dst_h > surface_h)
  {
    dst_h = surface_h;
    dst_w = (int)(((int64_t)surface_h * image_w) / image_h);
  }
  if (dst_w <= 0 || dst_h <= 0 || !EnsureFrameBuffer(dc, surface_w, surface_h))
    return 0;

  const int dst_x = (surface_w - dst_w) / 2;
  const int dst_y = (surface_h - dst_h) / 2;

  BITMAPINFO bmi;
  memset(&bmi, 0, sizeof(bmi));
  bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bmi.bmiHeader.biWidth = image_w;
  bmi.bmiHeader.biHeight = -image_h;
  bmi.bmiHeader.biPlanes = 1;
  bmi.bmiHeader.biBitCount = 32;
  bmi.bmiHeader.biCompression = BI_RGB;

  SetStretchBltMode(g_frame.dc, COLORONCOLOR);
  SetBrushOrgEx(g_frame.dc, 0, 0, NULL);
  PatBlt(g_frame.dc, 0, 0, surface_w, surface_h, BLACKNESS);
  const int stretch = StretchDIBits(g_frame.dc, dst_x, dst_y, dst_w, dst_h,
                                    0, 0, image_w, image_h,
                                    pixels, &bmi, DIB_RGB_COLORS, SRCCOPY);
  if (stretch == 0 || stretch == (int)GDI_ERROR)
  {
    if (InterlockedIncrement(&g_failure_logged) <= 4)
      LogLine("startup disclaimer draw failed: StretchDIBits result=%d", stretch);
    return 0;
  }

  if (!BitBlt(dc, 0, 0, surface_w, surface_h, g_frame.dc, 0, 0, SRCCOPY))
  {
    if (InterlockedIncrement(&g_failure_logged) <= 4)
      LogLine("startup disclaimer draw failed: BitBlt gle=%lu", GetLastError());
    return 0;
  }

  if (out_dst_w)
    *out_dst_w = dst_w;
  if (out_dst_h)
    *out_dst_h = dst_h;
  if (out_dst_x)
    *out_dst_x = dst_x;
  if (out_dst_y)
    *out_dst_y = dst_y;
  return 1;
}

typedef struct WindowSearch {
  DWORD pid;
  HWND best;
  int best_width;
  int best_height;
  int best_area;
} WindowSearch;

static int ResolveWindowClient(HWND hwnd, int* out_width, int* out_height)
{
  if (!hwnd || !IsWindow(hwnd) || !IsWindowVisible(hwnd) ||
      GetAncestor(hwnd, GA_ROOT) != hwnd)
    return 0;

  RECT client;
  memset(&client, 0, sizeof(client));
  if (!GetClientRect(hwnd, &client))
    return 0;

  const int width = RectWidth(&client);
  const int height = RectHeight(&client);
  if (!ValidDisplayExtent((DWORD)width, (DWORD)height) ||
      width < 64 || height < 64)
    return 0;

  if (out_width)
    *out_width = width;
  if (out_height)
    *out_height = height;
  return 1;
}

static BOOL CALLBACK EnumProcessWindowsProc(HWND hwnd, LPARAM param)
{
  int width = 0;
  int height = 0;
  WindowSearch* search = (WindowSearch*)param;
  if (!search || !ResolveWindowClient(hwnd, &width, &height))
    return TRUE;

  DWORD pid = 0;
  GetWindowThreadProcessId(hwnd, &pid);
  if (pid != search->pid)
    return TRUE;

  const int area = width * height;
  if (area > search->best_area)
  {
    search->best = hwnd;
    search->best_width = width;
    search->best_height = height;
    search->best_area = area;
  }

  return TRUE;
}

static void RememberWindow(HWND hwnd, int width, int height, const char* reason)
{
  if (!hwnd || !ValidDisplayExtent((DWORD)width, (DWORD)height))
    return;

  const HWND old_hwnd = g_game_window;
  const LONG old_w = g_window_width;
  const LONG old_h = g_window_height;
  if (old_hwnd == hwnd && old_w == (LONG)width && old_h == (LONG)height)
    return;

  InterlockedExchange(&g_window_width, (LONG)width);
  InterlockedExchange(&g_window_height, (LONG)height);
  g_game_window = hwnd;

  MarkLayoutChange();
  const LONG changes = g_layout_changes;
  char title[128];
  title[0] = '\0';
  GetWindowTextA(hwnd, title, (int)sizeof(title));
  LogLine("startup disclaimer window #%ld layout=%ld via=%s hwnd=%p client=%dx%d old=%p/%ldx%ld title=%s",
          InterlockedIncrement(&g_window_logged), changes,
          reason ? reason : "unknown", hwnd, width, height,
          old_hwnd, old_w, old_h, title);
}

static HWND ResolveGameWindow(int* out_width, int* out_height)
{
  const DWORD pid = GetCurrentProcessId();
  HWND foreground = GetForegroundWindow();
  DWORD window_pid = 0;
  if (foreground)
    GetWindowThreadProcessId(foreground, &window_pid);
  if (window_pid == pid)
  {
    int width = 0;
    int height = 0;
    if (ResolveWindowClient(foreground, &width, &height))
    {
      RememberWindow(foreground, width, height, "foreground");
      if (out_width)
        *out_width = width;
      if (out_height)
        *out_height = height;
      return foreground;
    }
  }

  HWND hwnd = g_game_window;
  int width = 0;
  int height = 0;
  if (ResolveWindowClient(hwnd, &width, &height))
  {
    RememberWindow(hwnd, width, height, "cached");
    if (out_width)
      *out_width = width;
    if (out_height)
      *out_height = height;
    return hwnd;
  }

  WindowSearch search;
  memset(&search, 0, sizeof(search));
  search.pid = pid;
  EnumWindows(EnumProcessWindowsProc, (LPARAM)&search);
  if (search.best)
  {
    RememberWindow(search.best, search.best_width, search.best_height, "enum");
    if (out_width)
      *out_width = search.best_width;
    if (out_height)
      *out_height = search.best_height;
    return search.best;
  }

  return NULL;
}

static int DrawDisclaimerWindow(const BYTE* pixels, int image_w, int image_h,
                                int* out_surface_w, int* out_surface_h,
                                int* out_dst_w, int* out_dst_h,
                                int* out_dst_x, int* out_dst_y,
                                int* out_bpp)
{
  int surface_w = 0;
  int surface_h = 0;
  HWND hwnd = ResolveGameWindow(&surface_w, &surface_h);
  if (!hwnd)
    return 0;

  HDC dc = GetDC(hwnd);
  if (!dc)
    return 0;

  const int bpp = GetDeviceCaps(dc, BITSPIXEL) * GetDeviceCaps(dc, PLANES);
  const int ok = DrawDisclaimerGdiToDC(dc, pixels, image_w, image_h,
                                       surface_w, surface_h, out_dst_w,
                                       out_dst_h, out_dst_x, out_dst_y);
  ReleaseDC(hwnd, dc);
  if (!ok)
    return 0;

  if (out_surface_w)
    *out_surface_w = surface_w;
  if (out_surface_h)
    *out_surface_h = surface_h;
  if (out_bpp)
    *out_bpp = bpp;
  return 1;
}

static void RetireDisclaimer(const char* reason, DWORD elapsed_ms)
{
  if (InterlockedCompareExchange(&g_retired, 1, 0) == 0)
  {
    LogLine("startup disclaimer retired reason=%s elapsed=%lums draws=%ld presents=%ld",
            reason ? reason : "unknown", elapsed_ms, g_draws, g_presents);
  }
}

static BYTE SmoothFadeAlpha(DWORD elapsed_ms, DWORD duration_ms)
{
  if (duration_ms == 0 || elapsed_ms >= duration_ms)
    return 255u;

  uint64_t x = (((uint64_t)elapsed_ms * 1024u) + (duration_ms / 2u)) / duration_ms;
  if (x > 1024u)
    x = 1024u;

  const uint64_t smooth = (x * x * (3072u - (2u * x))) / (1024u * 1024u);
  return (BYTE)((smooth * 255u + 512u) / 1024u);
}

static BYTE FadeAlpha(DWORD elapsed_ms)
{
  if (elapsed_ms < STARTUP_DISCLAIMER_FADE_IN_MS)
    return SmoothFadeAlpha(elapsed_ms, STARTUP_DISCLAIMER_FADE_IN_MS);

  if (elapsed_ms + STARTUP_DISCLAIMER_FADE_OUT_MS >= STARTUP_DISCLAIMER_DURATION_MS)
  {
    const DWORD remaining = STARTUP_DISCLAIMER_DURATION_MS > elapsed_ms
                              ? STARTUP_DISCLAIMER_DURATION_MS - elapsed_ms
                              : 0u;
    return SmoothFadeAlpha(remaining, STARTUP_DISCLAIMER_FADE_OUT_MS);
  }

  return 255u;
}

static const BYTE* PixelsForAlpha(BYTE alpha)
{
  if (alpha >= 255u || !g_image.bgra)
    return g_image.bgra;

  const SIZE_T pixel_count = (SIZE_T)g_image.width * (SIZE_T)g_image.height;
  const SIZE_T byte_count = pixel_count * 4u;
  if (!g_image.fade_bgra)
  {
    g_image.fade_bgra = (BYTE*)HeapAlloc(GetProcessHeap(), 0, byte_count);
    if (!g_image.fade_bgra)
      return g_image.bgra;
    g_image.fade_alpha = (BYTE)(alpha ^ 0xFFu);
  }

  if (g_image.fade_alpha == alpha)
    return g_image.fade_bgra;

  const BYTE* src = g_image.bgra;
  BYTE* dst = g_image.fade_bgra;
  for (SIZE_T i = 0; i < pixel_count; i++)
  {
    dst[0] = (BYTE)(((DWORD)src[0] * alpha) / 255u);
    dst[1] = (BYTE)(((DWORD)src[1] * alpha) / 255u);
    dst[2] = (BYTE)(((DWORD)src[2] * alpha) / 255u);
    dst[3] = 0xFFu;
    src += 4;
    dst += 4;
  }

  g_image.fade_alpha = alpha;
  return g_image.fade_bgra;
}

static void HoldAfterPresent(void* surface, DWORD elapsed_ms)
{
  (void)surface;
  (void)elapsed_ms;
  if (InterlockedCompareExchange(&g_hold_done, 1, 0) != 0)
    return;

  if (!EnsureImage())
    return;

  LogLine("startup disclaimer hold begin");
  for (;;)
  {
    if (g_retired)
      break;

    MSG msg;
    while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE))
    {
      TranslateMessage(&msg);
      DispatchMessageA(&msg);
    }

    const DWORD now = GetTickCount();
    if (!g_start_tick)
      g_start_tick = now ? now : 1u;

    const DWORD elapsed = (DWORD)(now - g_start_tick);
    if (elapsed >= STARTUP_DISCLAIMER_DURATION_MS)
      break;
    if (SkipPressed())
    {
      RetireDisclaimer("skip", elapsed);
      return;
    }

    const BYTE alpha = FadeAlpha(elapsed);
    const BYTE* pixels = PixelsForAlpha(alpha);
    int surface_w = 0;
    int surface_h = 0;
    int dst_w = 0;
    int dst_h = 0;
    int dst_x = 0;
    int dst_y = 0;
    int bpp = 0;
    if (DrawDisclaimerWindow(pixels, g_image.width, g_image.height,
                             &surface_w, &surface_h, &dst_w, &dst_h,
                             &dst_x, &dst_y, &bpp))
    {
      const LONG draws = InterlockedIncrement(&g_draws);
      const LONG presents = InterlockedIncrement(&g_presents);
      if (InterlockedIncrement(&g_draw_logged) <= 8)
      {
        LogLine("startup disclaimer draw #%ld present=%ld alpha=%u via=Hold path=window "
                "surface=%dx%d dst=%dx%d+%d+%d bpp=%d",
                draws, presents, (unsigned)alpha, surface_w, surface_h,
                dst_w, dst_h, dst_x, dst_y, bpp);
      }
    }

    DWORD left = STARTUP_DISCLAIMER_DURATION_MS - elapsed;
    if (left > STARTUP_DISCLAIMER_FRAME_SLEEP_MS)
      left = STARTUP_DISCLAIMER_FRAME_SLEEP_MS;
    Sleep(left ? left : 1u);
  }

  RetireDisclaimer("hold-complete", STARTUP_DISCLAIMER_DURATION_MS);
}

static int DrawDisclaimer(void* surface, int present_event)
{
  if (!surface || g_retired)
    return 0;

  DDSurfaceGetSurfaceDescProc get_desc =
    (DDSurfaceGetSurfaceDescProc)GetVTableSlot(surface, 22);
  if (!get_desc)
    return 0;

  DDSURFACEDESC_COMPAT desc;
  memset(&desc, 0, sizeof(desc));
  desc.dwSize = sizeof(desc);
  if (FAILED(get_desc(surface, &desc)) || !IsDisclaimerSurfaceDesc(&desc))
    return 0;

  const int effective_present =
    present_event || ((desc.ddsCaps.dwCaps & (DDSCAPS_PRIMARYSURFACE | DDSCAPS_FRONTBUFFER)) != 0);
  if (!effective_present)
    return 0;
  ResolveGameWindow(NULL, NULL);

  const DWORD now = GetTickCount();
  if (!g_start_tick)
    g_start_tick = now ? now : 1u;

  const DWORD elapsed = (DWORD)(now - g_start_tick);
  if (elapsed > STARTUP_DISCLAIMER_DURATION_MS)
  {
    RetireDisclaimer("timer", elapsed);
    return 0;
  }
  if (SkipPressed())
  {
    RetireDisclaimer("skip", elapsed);
    return 0;
  }
  if (g_draws >= STARTUP_DISCLAIMER_MAX_DRAWS ||
      g_presents >= STARTUP_DISCLAIMER_MAX_PRESENTS)
  {
    RetireDisclaimer("frame-limit", elapsed);
    return 0;
  }

  if (!EnsureImage())
    return 0;

  if (InterlockedCompareExchange(&g_hold_done, 0, 0) == 0)
  {
    HoldAfterPresent(surface, elapsed);
    return 1;
  }

  return 0;
}

static void PatchDisclaimerSurface(void* surface, const DDSURFACEDESC_COMPAT* desc)
{
  if (!surface || g_retired || !IsDisclaimerSurfaceDesc(desc))
    return;

  const LONG seen = InterlockedIncrement(&g_surface_seen);
  int patched = 0;
  patched += PatchVTableSlot(surface, 5, (void*)Hook_DDSurface_Blt);
  patched += PatchVTableSlot(surface, 7, (void*)Hook_DDSurface_BltFast);
  patched += PatchVTableSlot(surface, 11, (void*)Hook_DDSurface_Flip);

  if (patched == STARTUP_DISCLAIMER_SURFACE_SLOTS)
  {
    const LONG logged = InterlockedIncrement(&g_surface_patched);
    if (logged <= 4)
    {
      LogLine("startup disclaimer surface #%ld seen=%ld surface=%p caps=0x%08lX wh=%lux%lu",
              logged, seen, surface, desc->ddsCaps.dwCaps, desc->dwWidth, desc->dwHeight);
    }
  }
}

static HRESULT STDMETHODCALLTYPE Hook_DDSurface_Blt(void* self, RECT* dst, void* src, RECT* src_rect,
                                                    DWORD flags, void* fx)
{
  if (!self)
    return E_FAIL;

  DDSurfaceBltProc orig = (DDSurfaceBltProc)GetOriginal(*(void***)self, 5);
  if (!orig)
    return E_FAIL;

  HRESULT hr = orig(self, dst, src, src_rect, flags, fx);
  if (SUCCEEDED(hr) && !g_retired)
    DrawDisclaimer(self, 0);
  return hr;
}

static HRESULT STDMETHODCALLTYPE Hook_DDSurface_BltFast(void* self, DWORD x, DWORD y, void* src,
                                                        RECT* src_rect, DWORD trans)
{
  if (!self)
    return E_FAIL;

  DDSurfaceBltFastProc orig = (DDSurfaceBltFastProc)GetOriginal(*(void***)self, 7);
  if (!orig)
    return E_FAIL;

  HRESULT hr = orig(self, x, y, src, src_rect, trans);
  if (SUCCEEDED(hr) && !g_retired)
    DrawDisclaimer(self, 0);
  return hr;
}

static HRESULT STDMETHODCALLTYPE Hook_DDSurface_Flip(void* self, void* target_override, DWORD flags)
{
  if (!self)
    return E_FAIL;

  DDSurfaceFlipProc orig = (DDSurfaceFlipProc)GetOriginal(*(void***)self, 11);
  if (!orig)
    return E_FAIL;

  HRESULT hr = orig(self, target_override, flags);
  if (SUCCEEDED(hr) && !g_retired)
    DrawDisclaimer(self, 1);
  return hr;
}

static void TraceCreatedSurface(void* surface, const DDSURFACEDESC_COMPAT* desc, HRESULT hr)
{
  if (SUCCEEDED(hr) && surface && !g_retired)
  {
    PatchVTableSlot(surface, 0, (void*)Hook_QueryInterface);
    PatchDisclaimerSurface(surface, desc);
  }
}

static HRESULT STDMETHODCALLTYPE Hook_DD_CreateSurface(void* self, DDSURFACEDESC_COMPAT* desc,
                                                       void** surface, void* outer)
{
  DirectDrawCreateSurfaceProc orig =
    (DirectDrawCreateSurfaceProc)GetOriginal(*(void***)self, 6);
  if (!orig)
    return E_FAIL;

  HRESULT hr = orig(self, desc, surface, outer);
  TraceCreatedSurface(surface ? *surface : NULL, desc, hr);
  return hr;
}

static HRESULT STDMETHODCALLTYPE Hook_QueryInterface(void* self, REFIID riid, void** ppvObj)
{
  QueryInterfaceProc orig = (QueryInterfaceProc)GetOriginal(*(void***)self, 0);
  if (!orig)
    return E_NOINTERFACE;

  HRESULT hr = orig(self, riid, ppvObj);
  if (SUCCEEDED(hr) && ppvObj && *ppvObj && !g_retired)
  {
    PatchVTableSlot(*ppvObj, 0, (void*)Hook_QueryInterface);
    if (ZfixIsGuid(riid, &kIID_IDirectDraw2))
      PatchVTableSlot(*ppvObj, 6, (void*)Hook_DD_CreateSurface);
  }

  return hr;
}

static HRESULT WINAPI Hook_DirectDrawCreate(GUID* lpGUID, void** lplpDD, void* pUnkOuter)
{
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
  if (reason == DLL_PROCESS_ATTACH)
  {
    (void)reserved;
    BuildLogPath(instance);
    g_log_enabled = ZfixLogEnabledByMarker(g_log_path);
    if (g_log_enabled)
      DeleteFileA(g_log_path);
    DisableThreadLibraryCalls(instance);
    LogLine("re1_startup_disclaimer loaded log=%d duration=%ums fade=%u/%ums "
            "skip=action-fire(config)+J/XInputX/JoyButton1",
            g_log_enabled, STARTUP_DISCLAIMER_DURATION_MS, STARTUP_DISCLAIMER_FADE_IN_MS,
            STARTUP_DISCLAIMER_FADE_OUT_MS);
    LogLine("patch DirectDrawCreate imports=%d", PatchAllImports());
    ZfixStartDetachedThread(PatchImportsWorker, NULL);
  }
  else if (reason == DLL_PROCESS_DETACH)
  {
    LogLine("startup disclaimer summary loadState=%ld surfaces=%ld patched=%ld "
            "draws=%ld presents=%ld hold=%ld retired=%ld window=%ldx%ld layout=%ld",
            g_load_state, g_surface_seen, g_surface_patched,
            g_draws, g_presents, g_hold_done, g_retired,
            g_window_width, g_window_height, g_layout_changes);
    if (!reserved)
    {
      ReleaseImage();
      ReleaseInput();
    }
  }
  return TRUE;
}
