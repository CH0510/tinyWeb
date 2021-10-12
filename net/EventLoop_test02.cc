#include "src/EventLoop.h"

#include <stdio.h>

#include "../base/src/Thread.h"

using namespace tinyWeb;

net::EventLoop* g_loop;

void threadFunc() {
  g_loop->loop();   // 非本线程调用loop，会crash掉
}

int main(void) {
  net::EventLoop loop;
  g_loop = &loop;
  Thread t(threadFunc);
  t.start();
  t.join();

  return 0;
}