#pragma once

#include <memory>
#include <string>

#include "../../base/src/noncopyable.hpp"
#include "Callbacks.h"
#include "InetAddress.h"

namespace tinyWeb {
namespace net {

class EventLoop;
class Socket;
class Channel;

// 封装 TCP 连接（内部类），管理TCP连接的整个过程， 生命周期由 TcpServer 管理
// 其持有连接文件描述符，因此只要TcpConnection 未销毁，系统就不会分配同样的文件描述符
// 给新连接，防止了串话现象
class TcpConnection : public noncopyable, \
                      public std::enable_shared_from_this<TcpConnection> {
 public:
  TcpConnection(EventLoop* loop, \
                const std::string& name, \
                int connfd, \
                const InetAddress& localAddr, \
                const InetAddress& peerAddr);
  ~TcpConnection();

  bool connected() const {
    return state_ == kConnected;
  }

  std::string name() const { return name_; }

  // 绑定链接到来的用户回调函数
  void setConnectionCallback(ConnectionCallback connectionCallback) {
    connectionCallback_ = connectionCallback;
    assert(connectionCallback);
  }

  // 绑定消息到来的用户回调函数
  void setMessageCallback(MessageCallback messageCallback) {
    messageCallback_ = messageCallback;
  }

  // 由于 TcpConnection 由TcpServer管理，
  // 所以该函数由 TcpServer 调用
  void setCloseCallback(CloseCallback closeCallback) {
    closeCallback_ = closeCallback;
  }

  void setErrorCallback(ErrorCallback errorCallback) {
    errorCallback_ = errorCallback;
  }

  InetAddress localAddress() const { return localAddress_; }
  InetAddress peerAddress() const { return peerAddress_; }
  
  // 启动对 TCP 连接的监听,仅能调用一次
  // 当 TcpServer 创建一个 TcpConnection 并完成相应的初始化后
  // 会调用该函数，该函数会启动在此链接的事件监听并调用用户回调
  void connectEstablished();

  // TCP连接关闭时最后一个调用的函数，
  // 彻底移除对当前 TcpConnection 所管理的文件描述符的事件关注
  // 此函数一般由 TcpServer::removeConnection 移除当前TcpConnection
  // 后调用，也可能直接调用
  void connectDestroyed();

 private:
  // 当前 TCP 连接的状态
  enum StateE {
    kConnecting,
    kConnected,
    kDisConnected
  };

  void setState(StateE state) { state_ = state; }
  std::string stateToString();
  void handleRead();
  void handleWrite();
  void handleClose();
  void handleError();

  EventLoop* ownerLoop_;
  const std::string name_;  // 连接名称（服务器的名称 + 序列号）
  StateE state_;   // 连接的状态
  std::unique_ptr<Socket> connSock_;  // 持有连接描述符
  std::unique_ptr<Channel> connChannel_;  // 连接描述符对应的事件通道
  InetAddress localAddress_;
  InetAddress peerAddress_;
  ConnectionCallback connectionCallback_;
  MessageCallback messageCallback_;
  // 绑定到 TcpServer::removeConnection()，
  // 当 TcpConnection 所管理的连接关闭时，会调用此回调，
  // 用于删除 TcpConnection
  CloseCallback closeCallback_;
  ErrorCallback errorCallback_;
};

}  // namespace net
}  // namespace tinyWeb