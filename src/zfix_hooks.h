#ifndef ZFIX_HOOKS_H
#define ZFIX_HOOKS_H

#include <windows.h>

typedef struct HookEntry {
  void** vtable;
  int slot;
  void* original;
} HookEntry;

static inline void* GetVTableSlot(void* obj, int slot)
{
  if (!obj)
    return NULL;

  void** vtable = *(void***)obj;
  return vtable ? vtable[slot] : NULL;
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

static inline int ZfixPatchVTableSlot(HookEntry* hooks, LONG hook_capacity,
                                      volatile LONG* hook_count, void* obj,
                                      int slot, void* hook)
{
  if (!hooks || !hook_count || hook_capacity <= 0 || !obj)
    return 0;

  void** vtable = *(void***)obj;
  if (!vtable || vtable[slot] == hook)
    return 1;

  LONG count = *hook_count;
  if (count < 0)
    count = 0;
  if (count > hook_capacity)
    count = hook_capacity;
  for (LONG i = 0; i < count; i++)
  {
    if (hooks[i].vtable == vtable && hooks[i].slot == slot)
      return 1;
  }

  if (*hook_count >= hook_capacity)
    return 0;

  DWORD old_protect = 0;
  if (!VirtualProtect(&vtable[slot], sizeof(void*), PAGE_EXECUTE_READWRITE, &old_protect))
    return 0;

  if (vtable[slot] == hook)
  {
    DWORD ignored = 0;
    VirtualProtect(&vtable[slot], sizeof(void*), old_protect, &ignored);
    return 1;
  }

  const LONG idx = InterlockedIncrement(hook_count) - 1;
  if (idx < 0 || idx >= hook_capacity)
  {
    DWORD ignored = 0;
    VirtualProtect(&vtable[slot], sizeof(void*), old_protect, &ignored);
    InterlockedDecrement(hook_count);
    return 0;
  }

  hooks[idx].vtable = vtable;
  hooks[idx].slot = slot;
  hooks[idx].original = vtable[slot];
  vtable[slot] = hook;

  DWORD ignored = 0;
  VirtualProtect(&vtable[slot], sizeof(void*), old_protect, &ignored);
  FlushInstructionCache(GetCurrentProcess(), &vtable[slot], sizeof(void*));
  return 1;
}

#endif
