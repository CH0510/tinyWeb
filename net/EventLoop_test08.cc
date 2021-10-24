#include "src/TcpServer.h"
#include "src/TcpConnection.h"
#include "src/InetAddress.h"

#include <stdio.h>

using namespace tinyWeb;
using namespace net;

void onConnection(const TcpConnectionPtr& conn) {
  if (conn->connected()) {
    printf("onConnection() : new connection [%s] from %s\n", \
            conn->name().c_str(), \
            conn->peerAddress().toIpPort().c_str());
  } else {
    printf("onConnection() : connection [%s] is down\n", \
            conn->name().c_str());
  }
}

void onMessage(const TcpConnectionPtr& conn, \
               Buffer* buf, \
               Timestamp receiveTime) {
  std::string message(buf->retrieveAllAsString());
  printf("onMessage() : reveived %zd[%s] bytes from connection [%s] at %s\n", \
          message.size() , \
          message.c_str(), \
          conn->name().c_str(), \
          receiveTime.toLocalFormatString().c_str());
}

int main() {
  printf("main() : pid = %d\n", getpid());

  InetAddress listenAddr(8888);
  EventLoop loop;
  
  TcpServer server(&loop, listenAddr, "TestServer");
  server.setConnectionCallback(onConnection);
  server.setMessageCallback(onMessage);
  server.start();

  loop.loop();
}