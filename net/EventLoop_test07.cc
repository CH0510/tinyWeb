#include "src/Acceptor.h"
#include "src/EventLoop.h"
#include "src/InetAddress.h"
#include "src/SocketOps.h"
#include "../base/src/Timestamp.h"

#include <stdio.h>

using namespace tinyWeb;
using namespace tinyWeb::net;

void newConnection(int sockfd, const InetAddress& peerAddr) {
  printf("newConnection(): accepted a new connection from %s\n", \
          peerAddr.toIpPort().c_str());
  char buf[512] = {0};
  int n = snprintf(buf, sizeof buf, "%s\n", \
                    Timestamp::now().toLocalFormatString().c_str());
  int ret = ::write(sockfd, buf, n);
  (void) ret;
  sockets::close(sockfd);
}

int main() {
  printf("main(): pid = %d\n", getpid());

  InetAddress listenAddr(8888);
  EventLoop loop;
  Acceptor acceptor(&loop, listenAddr);
  acceptor.setNewConnectionCallback(newConnection);
  acceptor.listen();

  loop.loop();
  return 0;
}