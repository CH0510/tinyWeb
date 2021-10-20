#pragma once

#include <functional>

#include "../../base/src/noncopyable.hpp"
#include "Socket.h"
#include "Channel.h"

namespace tinyWeb {
namespace net {

class EventLoop;
class InetAddress;

// 监听套接字的封装类
// 注意此类只会由所属的IO线程调用，因此不必线程同步
class Acceptor : public noncopyable {
 public:
  typedef std::function<void(int, InetAddress)> NewConnectionCallback;

  Acceptor(EventLoop* loop, \
           const InetAddress& listenAddr, \
           bool reusePort = false);
  ~Acceptor();

  bool listenning() const { return listening_; }
  void setNewConnectionCallback(NewConnectionCallback newConnectionCallback) {
    newConnectionCallback_ = newConnectionCallback;
  }

  // 进行监听
  void listen();

  // 设置接受缓冲区大小
  // 须在listen之前调用
  void setReceiveBufferSize(int size) {
    listenSock_.setReceiveBufferSize(size);
  }

 private:
  // 接受新的连接，并调用绑定的回调函数
  void handleRead();

  EventLoop* ownerLoop_;
  Socket listenSock_;
  Channel listenChannel_;
  // 新连接到来调用的回调函数
  NewConnectionCallback newConnectionCallback_;
  bool listening_;
  // 用于处理EMFILE错误的空闲文件描述符
  int idleFd_;
};

}  // namespace net
}  // namespace tinyWeb