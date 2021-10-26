#include "src/EventLoop.h"
#include "src/TcpServer.h"
#include "src/InetAddress.h"

#include <string>
#include <functional>

#include <stdio.h>

using std::string;
using namespace tinyWeb;
using namespace tinyWeb::net;

class TestServer {
 public:
  TestServer(EventLoop* loop, const InetAddress& localAddr) \
    : loop_(loop), \
      server_(loop, localAddr, "TestServer") {
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
    server_.setWriteCompleteCallback(std::bind( \
                                     &TestServer::onWriteComplete, \
                                     this, \
                                     std::placeholders::_1));
    // 生成数据
    string line;
    for (int i = 33; i < 127; ++i)
    {
      line.push_back(char(i));
    }
    line += line;

    for (size_t i = 0; i < 127-33; ++i)
    {
      message_ += line.substr(i, 72) + '\n';
    }                                
  }

  void start() {
    server_.start();
  }

 private:
  void onConnection(const TcpConnectionPtr& conn)
  {
    if (conn->connected())
    {
      printf("onConnection(): new connection [%s] from %s\n",
             conn->name().c_str(),
             conn->peerAddress().toIpPort().c_str());

      conn->setTcpNoDelay(true);
      conn->send(message_);
    }
    else
    {
      printf("onConnection(): connection [%s] is down\n",
             conn->name().c_str());
    }
  }

  void onMessage(const TcpConnectionPtr& conn,
                 Buffer* buf,
                 Timestamp receiveTime)
  {
    string msg(buf->retrieveAllAsString());
  	printf("onMessage(): received %zd bytes from connection [%s] at %s\n",
           msg.size(),
           conn->name().c_str(),
           receiveTime.toLocalFormatString().c_str());

    conn->send(msg);
  }

  void onWriteComplete(const TcpConnectionPtr& conn)
  {
    conn->send(message_);
  }



  EventLoop* loop_;
  TcpServer server_;

  string message_;
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