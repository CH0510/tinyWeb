#include "src/EventLoop.h"

#include <stdio.h>

#include "../base/src/CurrentThread.h"
#include "../base/src/Thread.h"

using namespace tinyWeb;

void threadFunc() {
  printf("threadFunc(): pid = %d, tid = %d\n", \
        getpid(), CurrentThread::tid());

  net::EventLoop loop;
  loop.loop();
}

int main(void) {
  printf("main(): pid = %d, tid = %d\n", \
        getpid(), CurrentThread::tid());
  
  net::EventLoop loop;
  
  Thread t(threadFunc);
  t.start();

  loop.loop();
  t.join();

  return 0;
}