#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/timerfd.h>

#include <functional>

#include "src/EventLoop.h"
#include "src/Channel.h"
#include "../base/src/Timestamp.h"
#include "../base/src/Thread.h"

tinyWeb::net::EventLoop* g_loop;
int timerfd;

void timeout(tinyWeb::Timestamp receiveTimer) {
  printf("Timeout!\n");
  uint64_t howmany;
  ssize_t n = ::read(timerfd, &howmany, sizeof(howmany));
  (void)n;
  g_loop->quit();
}

void threadFunc() {
  printf("wake up EventLoop!\n");
  g_loop->wakeup();
}

int main(int argc, char** argv) {
  tinyWeb::net::EventLoop loop;
  g_loop = &loop;

  timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
  tinyWeb::net::Channel channel(&loop, timerfd);
  channel.setReadCallback(timeout);
  channel.enableReading();

  struct itimerspec howlong;
  bzero(&howlong, sizeof howlong);
  howlong.it_value.tv_sec = 1;
  ::timerfd_settime(timerfd, 0, &howlong, NULL);
  tinyWeb::Thread t(threadFunc);
  t.start();
  loop.loop();
  channel.disableAll();
  channel.remove();
  ::close(timerfd);

  return 0;
}