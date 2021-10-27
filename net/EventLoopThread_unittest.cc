#include "src/EventLoopThread.h"
#include "src/EventLoop.h"
#include "../base/src/Thread.h"
#include "../base/src/CountDownLatch.h"

#include <stdio.h>
#include <unistd.h>

using namespace tinyWeb;
using namespace tinyWeb::net;

void print(EventLoop* p = NULL)
{
  printf("print: pid = %d, tid = %d, loop = %p\n",
         getpid(), CurrentThread::tid(), p);
}

void quit(EventLoop* p)
{
  print(p);
  p->quit();
}

int main()
{
  print();

  {
  EventLoopThread thr1;  // never start
  }

  {
    // dtor calls quit()
    EventLoopThread thr2;
    EventLoop* loop = thr2.startInLoop();
    loop->runInLoop(std::bind(print, loop));
    usleep(500 * 1000);
  }

  {
    // quit() before dtor
    EventLoopThread thr3;
    EventLoop* loop = thr3.startInLoop();
    loop->runInLoop(std::bind(quit, loop));
    usleep(500 * 1000);
  }
}

