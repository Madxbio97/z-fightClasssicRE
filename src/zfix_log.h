#ifndef ZFIX_LOG_H
#define ZFIX_LOG_H

#include <windows.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static inline const char* ZfixBaseNameFromPath(const char* path)
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

static inline void ZfixBuildLogPath(HINSTANCE instance, char* game_dir,
                                    size_t game_dir_size, char* log_path,
                                    size_t log_path_size, const char* log_name)
{
  if (game_dir && game_dir_size)
    game_dir[0] = '\0';
  if (log_path && log_path_size)
    log_path[0] = '\0';
  if (!log_path || !log_path_size || !log_name)
    return;

  DWORD len = GetModuleFileNameA(instance, log_path, (DWORD)log_path_size);
  if (!len || len >= log_path_size)
    return;

  for (char* p = log_path + strlen(log_path); p > log_path; --p)
  {
    if (p[-1] == '\\' || p[-1] == '/')
    {
      *p = '\0';
      break;
    }
  }

  if (game_dir && game_dir_size)
  {
    strncpy(game_dir, log_path, game_dir_size - 1);
    game_dir[game_dir_size - 1] = '\0';
  }

  strncat(log_path, log_name, log_path_size - strlen(log_path) - 1);
}

static inline void ZfixLogLineV(int enabled, const char* log_path,
                                const char* fmt, va_list args)
{
  if (!enabled || !log_path || !log_path[0] || !fmt)
    return;

  char line[1400];
  SYSTEMTIME st;
  GetLocalTime(&st);
  int prefix = snprintf(line, sizeof(line), "[%02u:%02u:%02u.%03u] ",
                        st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
  if (prefix < 0 || prefix >= (int)sizeof(line))
    return;

  int body = vsnprintf(line + prefix, sizeof(line) - (size_t)prefix, fmt, args);
  if (body < 0)
    return;

  size_t len = strlen(line);
  if (len + 2 < sizeof(line))
  {
    line[len++] = '\r';
    line[len++] = '\n';
    line[len] = '\0';
  }

  HANDLE file = CreateFileA(log_path, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
                            NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if (file == INVALID_HANDLE_VALUE)
    return;

  DWORD written = 0;
  WriteFile(file, line, (DWORD)len, &written, NULL);
  CloseHandle(file);
}

#endif
