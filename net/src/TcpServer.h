#pragma once

#include <stdint.h>

#include <string>
#include <memory>
#include <map>
#include <functional>

#include "../../base/src/noncopyable.hpp"
#include "Acceptor.h"
#include "Callbacks.h"
#include "../../base/src/Atomic.hpp"
#include "TcpConnection.h"
#include "EventLoopThreadPool.h"

namespace tinyWeb {
namespace net {

class EventLoop;
class TcpConnection;

// 服务器类，供客户端使用,其生命周期由客户端管理,
// 客户可直接使用该类，或在自己服务器类中包含一个TcpServer实例,
// 使用前需设置响应的回调函数，对于建立的每一个连接都将调用这些回调函数
class TcpServer : public noncopyable {
 public:
  TcpServer(EventLoop* loop, \
            const InetAddress& listenAddr, \
            const std::string& name);
  ~TcpServer() = default;

  bool started() const { return started_.get(); }
  
  void setConnectionCallback(ConnectionCallback connectionCallback) {
    connectionCallback_ = connectionCallback;
  }

  void setMessageCallback(MessageCallback messageCallback) {
    messageCallback_ = messageCallback;
  }

  void setWriteCompleteCallback(WriteCompleteCallback writeCompleteCallback) {
    writeCompleteCallback_ = writeCompleteCallback;
  }

  // 设置 IO Thread 的数量，须在调用 start() 前调用
  void setThreadNum(size_t numThreads) {
    assert(started_.get() == 0);
    ioThreadPool_->setThreadNum(numThreads);
  }

  // 启动服务器，线程安全，且可以多次调用
  // 函数会启动内部的监听套接字
  void start();

 private:
  typedef std::map<std::string, TcpConnectionPtr> ConnectionMap;

  // 新连接到来的回调函数(Acceptor的回调函数)，其会根据新连接的文件描述符创建一个
  // TcpConnection用于管理新的链接，并调用用户绑定的新连接回调函数
  void newConnection(int fd, const InetAddress& peerAddr);

  // 当一个 TcpConnection 所管理的连接关闭时，会调用handleClose()
  // 进而调用该函数将链接从TcpServer中移除，
  // 完成这些操作后，其会调用 TcpConnection::connectionDestroy()做收尾工作
  void removeConnection(const TcpConnectionPtr &conn);
  void removeConnectionInLoop(const TcpConnectionPtr &conn);

  EventLoop* baseLoop_;
  const std::string name_;  // 服务器的名称
  std::unique_ptr<Acceptor> acceptor_;  // 服务器接收新连接的监听套接字
  std::unique_ptr<EventLoopThreadPool> ioThreadPool_;
  //bool started_;  // 无法满足原子性的要求
  AtomicInt32 started_;
  uint64_t nextConn_;  // 客户端的序列号
  // 一个TCP应处理的三个半事件，客户端应绑定自己的回调函数
  ConnectionCallback connectionCallback_;
  MessageCallback messageCallback_;
  WriteCompleteCallback writeCompleteCallback_;
  // 维护一个已建立连接的队列，
  // 键为连接名，值为相应的TcpConnection对象
  ConnectionMap connections_;
};

}  // namespace net
}  // namespace tinyWeb