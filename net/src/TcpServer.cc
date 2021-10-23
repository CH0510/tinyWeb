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
  // 创建一个 TcpConnection 来管理新的连接
  TcpConnectionPtr conn(new TcpConnection( \
          ownerLoop_, connName, connFd, sockets::getLocalAddr(connFd), peerAddr));
  conn->setConnectionCallback(connectionCallback_);
  conn->setMessageCallback(messageCallback_);
  conn->setCloseCallback(std::bind( \
                         &TcpServer::removeConnection, \
                         this, \
                         std::placeholders::_1));
  connections_[connName] = conn;
  // 对 TcpConnection 完成所有的初始化之后，
  // 调用 TcpConnection::connectEstablished() 
  // 启动链接描述符上的监听事件并调用用户的回调
  conn->connectEstablished();
}

void TcpServer::removeConnection(const TcpConnectionPtr &conn) {
  ownerLoop_->assertInLoopThread();
  assert(connections_.find(conn->name()) != connections_.end());
  LOG_INFO << "TcpServer::removeConnection() [" << name_ \
           <<  "] - connection " << conn->name();
  // erase 后引用计数变为1，只剩下函数参数传递进来的的 conn
  size_t n = connections_.erase(conn->name());
  assert(n == 1); (void) n;
  // 在此处bind是值语义，所以会拷贝一份，使得引用计数变为2
  ownerLoop_->queueInLoop(std::bind( \
                          &TcpConnection::connectDestroyed, conn));
}

}  // namespace net
}  // namespace tinyWeb