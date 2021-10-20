#include "src/Acceptor.h"
#include "src/EventLoop.h"
#include "src/InetAddress.h"
#include "src/SocketOps.h"

#include <stdio.h>

using namespace tinyWeb;
using namespace tinyWeb::net;

void newConnection(int sockfd, const InetAddress& peerAddr) {
  printf("newConnection(): accepted a new connection from %s\n", \
          peerAddr.toIpPort().c_str());
  int ret = ::write(sockfd, "How are you?\n", 13);
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