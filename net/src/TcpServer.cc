#include "TcpServer.h"

#include "../../base/src/Logging.h"
#include "TcpConnection.h"
#include "SocketOps.h"

namespace tinyWeb {
namespace net {

TcpServer::TcpServer(EventLoop* loop, \
          const InetAddress& listenAddr, \
          const std::string& name) \
  : ownerLoop_(loop), name_(name), \
    acceptor_(new Acceptor(loop, listenAddr)), \
    nextConn_(0) {
  acceptor_->setNewConnectionCallback( \
      std::bind(&TcpServer::newConnection, this, \
                std::placeholders::_1, std::placeholders::_2));
}

// 使用 runInLoop 保证该函数的线程安全性
void TcpServer::start() {
  /*
  if (started_ == false) {
    started_ = true;
    ownerLoop_->runInLoop(std::bind(&Acceptor::listen, acceptor_.get()));
  }
  */
 // 上述对于started_的操作不是原子性的，存在 race condition
 if (started_.getAndSet(1) == false) {
   ownerLoop_->runInLoop(std::bind(&Acceptor::listen, acceptor_.get()));
 }
}

void TcpServer::newConnection(int connFd, const InetAddress& peerAddr) {
  ownerLoop_->assertInLoopThread();
  char buf[32] = {0};
  snprintf(buf, sizeof buf, ":%ld", ++nextConn_);
  std::string connName = name_ + buf;

  LOG_INFO << "TcpServer::newConnection: [" << name_ \
           << "] - new connection [" << connName \
           << "] from " << peerAddr.toIpPort();
  
  TcpConnectionPtr conn(new TcpConnection( \
          ownerLoop_, connName, connFd, sockets::getLocalAddr(connFd), peerAddr));
  conn->setConnectionCallback(connectionCallback_);
  conn->setMessageCallback(messageCallback_);
  connections_[connName] = conn;
  conn->connectEstablished();
}

}  // namespace net
}  // namespace tinyWeb