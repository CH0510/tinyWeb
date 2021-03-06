#include "CurrentThread.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <type_traits>

namespace tinyWeb {
namespace CurrentThread {

  __thread int t_cachedTid = 0;
  __thread char t_tidString[32];
  __thread int t_tidStringLength = 6;
  __thread const char* t_threadName = "unknow";
  static_assert(std::is_same<int, pid_t>::value, "pid_t should be int");

  void cacheTid() {
    if (t_cachedTid == 0) {
      t_cachedTid = ::gettid();
      memset(t_tidString, 0, sizeof(t_tidString));
      t_tidStringLength = snprintf(t_tidString, sizeof(t_tidString), \
                                   "%5d ", t_cachedTid);
    }
  }

}  // namespace CurrentThread
}  // namespace tinyWeb
