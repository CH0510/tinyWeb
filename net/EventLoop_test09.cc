#include <stdio.h>

#include <functional>

#include "src/TcpServer.h"
#include "src/EventLoop.h"
#include "src/InetAddress.h"

using namespace tinyWeb;
using namespace tinyWeb::net;

class TestServer {
 public:
  TestServer(EventLoop* loop, \
             const InetAddress& listenAddr) \
    : loop_(loop), \
      server_(loop, listenAddr, "TestServer") {
  server_.setConnectionCallback(std::bind( \
                                &TestServer::onConnection, \
                                this, \
                                std::placeholders::_1));
  server_.setMessageCallback(std::bind( \
                             &TestServer::onMessage, \
                             this, \
                             std::placeholders::_1, \
                             std::placeholders::_2, \
                             std::placeholders::_3));
  }

  void start() {
    server_.start();
  }

 private:
  void onConnection(const TcpConnectionPtr& conn) {
    if (conn->connected()) {
      printf("onConnection(): new connection [%s] from %s\n", \
              conn->name().c_str(), \
              conn->peerAddress().toIpPort().c_str());
    } else {
      printf("onConnection(): connection [%s] is down\n",
              conn->name().c_str());
    }
  }

  void onMessage(const TcpConnectionPtr& conn, \
                Buffer* buf, \
                Timestamp receiveTime) {
    std::string message(buf->retrieveAllAsString());
    printf("onMessage(): received %zd bytes from connection [%s] at %s\n", \
           message.size(), \
           conn->name().c_str(), \
           receiveTime.toLocalFormatString().c_str());
  }

  EventLoop* loop_;
  TcpServer server_;
};

int main()
{
  printf("main(): pid = %d\n", getpid());

  InetAddress listenAddr(8888);
  EventLoop loop;

  TestServer server(&loop, listenAddr);
  server.start();

  loop.loop();
}