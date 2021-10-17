#include <stdio.h>
#include <pthread.h>

#include "src/EventLoop.h"
#include "src/EventLoopThread.h"
#include "src/TimerId.h"

using namespace tinyWeb;
using namespace net;

void runInThread() {
  printf("runInThread(): pid = %d, tid = %d\n", \
    getpid(), CurrentThread::tid());
}

int main() {
  printf("main(): pid = %d, tid = %d\n", \
    getpid(), CurrentThread::tid());
  
  EventLoopThread loopThread;
  EventLoop* loop = loopThread.startInLoop();

  loop->runInLoop(runInThread);
  sleep(1);

  loop->runAfter(runInThread, 2);
  sleep(3);
  loop->quit();
  printf("exit main()\n");
  pthread_exit(NULL);
}