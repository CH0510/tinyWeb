#include "TcpServer.h"

#include "../../base/src/Logging.h"
#include "SocketOps.h"

namespace tinyWeb {
namespace net {
namespace detail {
  void defaultConnectionCallback(const TcpConnectionPtr& conn) {
    if (conn->connected())
    {
      printf("onConnection(): new connection [%s] from %s\n",
             conn->name().c_str(),
             conn->peerAddress().toIpPort().c_str());
    }
    else
    {
      printf("onConnection(): connection [%s] is down\n",
             conn->name().c_str());
    }
  }

  void defaultMessageCallback(const TcpConnectionPtr& conn, \
                              Buffer* msg, \
                              Timestamp receiveTime) {
    // 默认是 discard
    msg->retrieveAll();
  }
}  // namespace detail


TcpServer::TcpServer(EventLoop* loop, \
          const InetAddress& listenAddr, \
          const std::string& name) \
  : baseLoop_(loop), name_(name), \
    acceptor_(new Acceptor(loop, listenAddr)), \
    ioThreadPool_(new EventLoopThreadPool(baseLoop_, name)), \
    nextConn_(0) {
  acceptor_->setNewConnectionCallback( \
      std::bind(&TcpServer::newConnection, this, \
                std::placeholders::_1, std::placeholders::_2));
  connectionCallback_ = detail::defaultConnectionCallback;
  messageCallback_ = detail::defaultMessageCallback;
}

// 使用 runInLoop 保证该函数的线程安全性
void TcpServer::start() {
  /*
  if (started_ == false) {
    started_ = true;
    baseLoop_->runInLoop(std::bind(&Acceptor::listen, acceptor_.get()));
  }
  */
 // 上述对于started_的操作不是原子性的，存在 race condition
 if (started_.getAndSet(1) == 0) {
   ioThreadPool_->start();
   baseLoop_->runInLoop(std::bind(&Acceptor::listen, acceptor_.get()));
 }
}

void TcpServer::newConnection(int connFd, const InetAddress& peerAddr) {
  baseLoop_->assertInLoopThread();
  char buf[32] = {0};
  snprintf(buf, sizeof buf, ":%ld", ++nextConn_);
  std::string connName = name_ + buf;

  LOG_INFO << "TcpServer::newConnection: [" << name_ \
           << "] - new connection [" << connName \
           << "] from " << peerAddr.toIpPort();
  // 创建一个 TcpConnection 来管理新的连接，并交由一个 IO 线程来处理
  EventLoop* ioLoop = ioThreadPool_->getNextLoop();
  TcpConnectionPtr conn(new TcpConnection( \
          ioLoop, connName, connFd, sockets::getLocalAddr(connFd), peerAddr));
  conn->setConnectionCallback(connectionCallback_);
  conn->setMessageCallback(messageCallback_);
  conn->setWriteCompleteCallback(writeCompleteCallback_);
  conn->setCloseCallback(std::bind( \
                         &TcpServer::removeConnection, \
                         this, \
                         std::placeholders::_1));
  connections_[connName] = conn;
  // 对 TcpConnection 完成所有的初始化之后，
  // 调用 TcpConnection::connectEstablished() 
  // 启动链接描述符上的监听事件并调用用户的回调
  ioLoop->runInLoop(std::bind(&TcpConnection::connectEstablished, conn));
}

void TcpServer::removeConnection(const TcpConnectionPtr &conn) {
  // conn 一般会在自己的 ioLoop 线程中调用 removeConnection
  // 而 TcpServer 的 connection 是没有加锁的，因此要转移到
  // TcpServer 的 baseLoop_ 线程执行
  baseLoop_->runInLoop(std::bind( \
                        &TcpServer::removeConnectionInLoop, \
                        this, \
                        conn));
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr &conn) {
  baseLoop_->assertInLoopThread();
  assert(connections_.find(conn->name()) != connections_.end());
  LOG_INFO << "TcpServer::removeConnection() [" << name_ \
           <<  "] - connection " << conn->name();
  // erase 后引用计数变为1，只剩下函数参数传递进来的的 conn
  size_t n = connections_.erase(conn->name());
  assert(n == 1); (void) n;

  EventLoop* ioLoop = conn->getLoop();
  // TcpConnection::connectDestroy() 应在 conn 所属的 ioLoop 线程中执行
  // 在此处bind是值语义，所以会拷贝一份，使得引用计数变为2
  ioLoop->queueInLoop(std::bind( \
                          &TcpConnection::connectDestroyed, conn));
}


}  // namespace net
}  // namespace tinyWeb