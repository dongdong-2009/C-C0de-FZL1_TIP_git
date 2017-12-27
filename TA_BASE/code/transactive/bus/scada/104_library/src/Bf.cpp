#include "Btype.h"

#include <stdlib.h>

#ifdef __OS_WIN32_
#include <windows.h>
#else
#include <unistd.h>
#endif

#ifdef __OS_WIN32_
const static HANDLE heap_handle = GetProcessHeap();
#endif

pointer_type bheap_alloc(b_i32 msize)
{
  if(msize <= 0)
    return 0;

#ifdef __OS_WIN32_
  if(!heap_handle)
    return 0;

  return HeapAlloc(heap_handle, 0, msize);
#else
  return malloc(msize);
#endif
}

void bheap_free(pointer_type mem)
{
  if(!mem)
    return;

#ifdef __OS_WIN32_
  if(!heap_handle)
    return;

  HeapFree(heap_handle, 0, mem);
#else
  free(mem);
#endif
}

pointer_type bheap_realloc(pointer_type old_mem, b_i32 new_msize)
{
  if(!old_mem)
    return bheap_alloc(new_msize);

  if(new_msize <= 0)
    return 0;

#ifdef __OS_WIN32_
  if(!heap_handle)
    return 0;

  return HeapReAlloc(heap_handle, 0, old_mem, new_msize);
#else
  return realloc(old_mem, new_msize);
#endif
}

b_i32 atoi32(char* str)
{
  return (b_i32)strtol(str, (char**)0, 10);
}

b_i64 atoi64(char* str)
{
  return (b_i64)strtol(str, (char**)0, 10);
}

void bsleep(b_i32 mills)
{
  if(mills <= 0)
    return;

#ifdef __OS_WIN32_
  Sleep(mills);
#else
  usleep(mills * 1000);
#endif
}
