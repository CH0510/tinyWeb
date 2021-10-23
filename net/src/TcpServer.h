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

namespace tinyWeb {
namespace net {

class EventLoop;
class TcpConnection;

// 服务器类，供客户直接使用
// 客户可封装自己的服务器类，并包含一个TcpServer实例
// 使用前需设置响应的回调函数，对于建立的每一个链接都将调用该回调函数
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

  // 启动服务器，线程安全，且可以多次调用
  // 函数会启动内部的监听套接字
  void start();

 private:
  typedef std::map<std::string, TcpConnectionPtr> ConnectionMap;

  // 新连接到来的回调函数，其会根据新连接的文件描述符创建一个
  // TcpConnection用于管理新的链接，并调用用户绑定的新连接回调函数
  void newConnection(int fd, const InetAddress& peerAddr);

  EventLoop* ownerLoop_;
  const std::string name_;  // 服务器的名称
  std::unique_ptr<Acceptor> acceptor_;  // 服务器接受新连接的监听套接字
  //bool started_;
  AtomicInt32 started_;
  uint64_t nextConn_;  // 客户端的序列号
  ConnectionCallback connectionCallback_;
  MessageCallback messageCallback_;
  // 维护一个已建立连接的队列，
  // 键为连接名，值为相应的TcpConnection对象
  ConnectionMap connections_;
};

}  // namespace net
}  // namespace tinyWeb