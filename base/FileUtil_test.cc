#include "src/FileUtil.h"
#include "src/Thread.h"
#include "src/CurrentThread.h"
#include "src/CountDownLatch.h"
#include <cstdio>
#include <inttypes.h>
#include <string>

using namespace tinyWeb;

AppendFile af("test.txt");
const int g_times = 64 * 1024 * 100;

void threadFunc() {
  printf("tid : %d, thread start\n", CurrentThread::tid());
  for (int i = 0; i < g_times; ++i) {
    char buf[64] = {0};
    snprintf(buf, sizeof(buf), "tid : %d , hello%d\n", CurrentThread::tid(), i);
    af.append(buf, strlen(buf) + 1);
  }
}

int main(int argc, char* argv[]) {
  Thread t1(threadFunc, "thread1");
  Thread t2(threadFunc, "thread2");
  t1.start();
  t2.start();

  t1.join();
  t2.join();

  return 0;
}