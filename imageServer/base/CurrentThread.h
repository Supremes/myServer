#pragma once
#include <stdint.h>
#include "utility.h"

namespace CurrentThread
{
  // __thread int t_cachedTid = 0;
  // __thread char t_tidString[32];
  // __thread int t_tidStringLength = 6;
  // __thread const char* t_threadName = "default";
      // internal
    extern __thread int t_cachedTid;
    extern __thread char t_tidString[32];
    extern __thread int t_tidStringLength;
    extern __thread const char* t_threadName;

  void cacheTid();
  // {
  //   if (t_cachedTid == 0)
  //   {
  //       t_cachedTid = gettid();
  //       t_tidStringLength = snprintf(t_tidString, sizeof t_tidString, "%5d ", t_cachedTid);
  //   }
  // }

  inline int tid()
  {
    if (__builtin_expect(t_cachedTid == 0, 0))
    {
      cacheTid();
    }
    return t_cachedTid;
  }

  inline const char* tidString() // for logging
  {
    return t_tidString;
  }

  inline int tidStringLength() // for logging
  {
    return t_tidStringLength;
  }

  inline const char* name()
  {
    return t_threadName;
  }
}
