#include "src/EventLoopThreadPool.h"
#include "src/EventLoop.h"
#include "../base/src/Thread.h"
#include "src/TimerId.h"

#include <stdio.h>
#include <unistd.h>
#include <functional>

using namespace tinyWeb;
using namespace tinyWeb::net;

void print(EventLoop* p = NULL)
{
  printf("main(): pid = %d, tid = %d, loop = %p\n",
         getpid(), CurrentThread::tid(), p);
}

void init(EventLoop* p)
{
  printf("init(): pid = %d, tid = %d, loop = %p\n",
         getpid(), CurrentThread::tid(), p);
}

int main()
{
  print();

  EventLoop loop;
  TimerId tmId = loop.runAfter(std::bind(&EventLoop::quit, &loop), 11);
  (void) tmId;

  {
    printf("Single thread %p:\n", &loop);
    EventLoopThreadPool model(&loop, "single");
    model.setThreadNum(0);
    model.start();
    assert(model.getNextLoop() == &loop);
    assert(model.getNextLoop() == &loop);
    assert(model.getNextLoop() == &loop);
  }

  {
    printf("Another thread:\n");
    EventLoopThreadPool model(&loop, "another");
    model.setThreadNum(1);
    model.start();
    EventLoop* nextLoop = model.getNextLoop();
    TimerId tmId = nextLoop->runAfter(std::bind(print, nextLoop), 2);
    (void) tmId;
    assert(nextLoop != &loop);
    assert(nextLoop == model.getNextLoop());
    assert(nextLoop == model.getNextLoop());
    ::sleep(3);
  }

  {
    printf("Three threads:\n");
    EventLoopThreadPool model(&loop, "three");
    model.setThreadNum(3);
    model.start();
    EventLoop* nextLoop = model.getNextLoop();
    nextLoop->runInLoop(std::bind(print, nextLoop));
    (void) tmId;
    assert(nextLoop != &loop);
    assert(nextLoop != model.getNextLoop());
    assert(nextLoop != model.getNextLoop());
    assert(nextLoop == model.getNextLoop());
  }

  loop.loop();
}

