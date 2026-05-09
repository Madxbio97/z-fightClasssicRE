#ifndef ZFIX_HOOKS_H
#define ZFIX_HOOKS_H

#include <windows.h>
#include <string.h>
#include <tlhelp32.h>

#define ZFIX_IMPORT_REPATCH_PASSES 20u
#define ZFIX_IMPORT_REPATCH_DELAY_MS 250u

typedef struct HookEntry {
  void** vtable;
  int slot;
  void* original;
} HookEntry;

typedef int (*ZfixModulePatchProc)(HMODULE module);
typedef int (*ZfixImportPatchPassProc)(void);
typedef void (*ZfixImportPatchLogProc)(DWORD pass, int patched);

static inline void* GetVTableSlot(void* obj, int slot)
{
  if (!obj)
    return NULL;

  void** vtable = *(void***)obj;
  return vtable ? vtable[slot] : NULL;
}

static inline int ZfixIsGuid(REFIID a, const GUID* b)
{
  return a && b && memcmp(a, b, sizeof(GUID)) == 0;
}

static inline void* ZfixGetOriginal(HookEntry* hooks, volatile LONG* hook_count,
                                    void** vtable, int slot)
{
  if (!hooks || !hook_count)
    return NULL;

  const LONG count = *hook_count;
  for (LONG i = 0; i < count; i++)
  {
    if (hooks[i].vtable == vtable && hooks[i].slot == slot)
      return hooks[i].original;
  }
  return NULL;
}

static inline void ZfixAcquirePatchLock(volatile LONG* lock)
{
  while (InterlockedCompareExchange(lock, 1, 0) != 0)
    Sleep(0);
}

static inline void ZfixReleasePatchLock(volatile LONG* lock)
{
  InterlockedExchange(lock, 0);
}

static inline int ZfixPatchVTableSlot(HookEntry* hooks, LONG hook_capacity,
                                      volatile LONG* hook_count, void* obj,
                                      int slot, void* hook)
{
  if (!hooks || !hook_count || hook_capacity <= 0 || !obj)
    return 0;

  static volatile LONG patch_lock = 0;
  int result = 0;
  ZfixAcquirePatchLock(&patch_lock);

  void** vtable = *(void***)obj;
  if (!vtable || vtable[slot] == hook)
  {
    result = vtable ? 1 : 0;
    goto done;
  }

  LONG count = *hook_count;
  if (count < 0)
    count = 0;
  if (count > hook_capacity)
    count = hook_capacity;
  for (LONG i = 0; i < count; i++)
  {
    if (hooks[i].vtable == vtable && hooks[i].slot == slot)
    {
#ifdef ZFIX_LEGACY_HOOK_PATCHING
      result = 1;
      goto done;
#else
      if (vtable[slot] == hook)
      {
        result = 1;
        goto done;
      }
      if (vtable[slot] != hooks[i].original)
      {
        /* Another hook owns the slot now. Leave it in place so chained hooks
           can keep working instead of fighting for the top of the vtable. */
        result = 0;
        goto done;
      }

      DWORD old_protect = 0;
      if (!VirtualProtect(&vtable[slot], sizeof(void*), PAGE_EXECUTE_READWRITE, &old_protect))
        goto done;

      vtable[slot] = hook;
      DWORD ignored = 0;
      VirtualProtect(&vtable[slot], sizeof(void*), old_protect, &ignored);
      FlushInstructionCache(GetCurrentProcess(), &vtable[slot], sizeof(void*));
      result = 1;
      goto done;
#endif
    }
  }

  if (*hook_count >= hook_capacity)
    goto done;

  DWORD old_protect = 0;
  if (!VirtualProtect(&vtable[slot], sizeof(void*), PAGE_EXECUTE_READWRITE, &old_protect))
    goto done;

  if (vtable[slot] == hook)
  {
    DWORD ignored = 0;
    VirtualProtect(&vtable[slot], sizeof(void*), old_protect, &ignored);
    result = 1;
    goto done;
  }

  const LONG idx = *hook_count;
  if (idx < 0 || idx >= hook_capacity)
  {
    DWORD ignored = 0;
    VirtualProtect(&vtable[slot], sizeof(void*), old_protect, &ignored);
    goto done;
  }

  hooks[idx].vtable = vtable;
  hooks[idx].slot = slot;
  hooks[idx].original = vtable[slot];
  InterlockedExchange(hook_count, idx + 1);
  vtable[slot] = hook;

  DWORD ignored = 0;
  VirtualProtect(&vtable[slot], sizeof(void*), old_protect, &ignored);
  FlushInstructionCache(GetCurrentProcess(), &vtable[slot], sizeof(void*));
  result = 1;

done:
  ZfixReleasePatchLock(&patch_lock);
  return result;
}

static inline void* ZfixRvaToPtr(BYTE* module, DWORD rva)
{
  return (module && rva) ? (void*)(module + rva) : NULL;
}

static inline int ZfixAddressInModule(HMODULE module, const void* address)
{
  if (!module || !address)
    return 0;

  BYTE* base = (BYTE*)module;
  IMAGE_DOS_HEADER* dos = (IMAGE_DOS_HEADER*)base;
  if (dos->e_magic != IMAGE_DOS_SIGNATURE || dos->e_lfanew <= 0)
    return 0;

  IMAGE_NT_HEADERS* nt = (IMAGE_NT_HEADERS*)(base + dos->e_lfanew);
  if (nt->Signature != IMAGE_NT_SIGNATURE)
    return 0;

  const BYTE* ptr = (const BYTE*)address;
  return ptr >= base && ptr < (base + nt->OptionalHeader.SizeOfImage);
}

static inline int ZfixPatchModuleImport(HMODULE module, const char* dll_name,
                                        const char* proc_name, void* hook,
                                        void** original)
{
  if (!module || !dll_name || !proc_name || !hook)
    return 0;

  BYTE* base = (BYTE*)module;
  IMAGE_DOS_HEADER* dos = (IMAGE_DOS_HEADER*)base;
  if (dos->e_magic != IMAGE_DOS_SIGNATURE)
    return 0;
  if (dos->e_lfanew <= 0)
    return 0;

  IMAGE_NT_HEADERS* nt = (IMAGE_NT_HEADERS*)(base + dos->e_lfanew);
  if (nt->Signature != IMAGE_NT_SIGNATURE)
    return 0;

  IMAGE_DATA_DIRECTORY dir =
    nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
  if (!dir.VirtualAddress)
    return 0;

#ifdef IMAGE_ORDINAL_FLAG32
  const ULONG_PTR ordinal_flag = IMAGE_ORDINAL_FLAG32;
#else
  const ULONG_PTR ordinal_flag = IMAGE_ORDINAL_FLAG;
#endif

  int patched = 0;
  IMAGE_IMPORT_DESCRIPTOR* desc =
    (IMAGE_IMPORT_DESCRIPTOR*)ZfixRvaToPtr(base, dir.VirtualAddress);
#ifndef ZFIX_LEGACY_HOOK_PATCHING
  HMODULE imported_module = GetModuleHandleA(dll_name);
#endif
  for (; desc && desc->Name; desc++)
  {
    const char* imported_dll = (const char*)ZfixRvaToPtr(base, desc->Name);
    if (!imported_dll || _stricmp(imported_dll, dll_name) != 0)
      continue;

    IMAGE_THUNK_DATA* orig_thunk =
      (IMAGE_THUNK_DATA*)ZfixRvaToPtr(base, desc->OriginalFirstThunk);
    IMAGE_THUNK_DATA* thunk =
      (IMAGE_THUNK_DATA*)ZfixRvaToPtr(base, desc->FirstThunk);
    if (!orig_thunk)
      orig_thunk = thunk;

    for (; orig_thunk && thunk && orig_thunk->u1.AddressOfData; orig_thunk++, thunk++)
    {
      if (orig_thunk->u1.Ordinal & ordinal_flag)
        continue;

      IMAGE_IMPORT_BY_NAME* by_name =
        (IMAGE_IMPORT_BY_NAME*)ZfixRvaToPtr(base, (DWORD)orig_thunk->u1.AddressOfData);
      if (!by_name || strcmp((const char*)by_name->Name, proc_name) != 0)
        continue;

      void** target = (void**)&thunk->u1.Function;
      if (*target == hook)
        continue;
#ifndef ZFIX_LEGACY_HOOK_PATCHING
      if (original && *original && *target != *original &&
          !ZfixAddressInModule(imported_module, *target))
      {
        /* The import already points at a third-party hook. Do not overwrite it
           after our original target has been established. */
        continue;
      }
#endif
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

static inline int ZfixPatchLoadedModules(ZfixModulePatchProc patch_module)
{
  if (!patch_module)
    return 0;

  int patched = 0;
  HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
  if (snap == INVALID_HANDLE_VALUE)
    return patch_module(GetModuleHandleA(NULL));

  MODULEENTRY32 me;
  memset(&me, 0, sizeof(me));
  me.dwSize = sizeof(me);
  if (Module32First(snap, &me))
  {
    do
    {
      patched += patch_module(me.hModule);
    } while (Module32Next(snap, &me));
  }
  CloseHandle(snap);
  return patched;
}

static inline void ZfixRunDelayedImportPatches(ZfixImportPatchPassProc patch_all,
                                               DWORD passes, DWORD delay_ms,
                                               ZfixImportPatchLogProc log_patch)
{
  if (!patch_all)
    return;

  for (DWORD i = 0; i < passes; i++)
  {
    Sleep(delay_ms);
    const int patched = patch_all();
    if (patched && log_patch)
      log_patch(i + 1, patched);
  }
}

static inline int ZfixStartDetachedThread(LPTHREAD_START_ROUTINE proc, void* param)
{
  if (!proc)
    return 0;

  HANDLE worker = CreateThread(NULL, 0, proc, param, 0, NULL);
  if (!worker)
    return 0;

  CloseHandle(worker);
  return 1;
}

#endif
