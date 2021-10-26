#pragma once

#include <memory>
#include <string>

#include "../../base/src/noncopyable.hpp"
#include "EventLoop.h"
#include "Channel.h"
#include "Socket.h"
#include "Callbacks.h"
#include "InetAddress.h"
#include "Buffer.h"
#include "../../base/src/Logging.h"

namespace tinyWeb {
namespace net {

class EventLoop;
class Socket;
class Channel;

// 封装 TCP 连接，管理TCP连接的整个过程，用户可以持有该对象，其生命周期由 TcpServer 管理
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
  // 绑定消息发送完毕的用户回调函数
  void setWriteCompleteCallback(WriteCompleteCallback writeCompleteCallback) {
    writeCompleteCallback_ = writeCompleteCallback;
  }
  // 仅由类 TcpServer 调用，客户不能调用该函数
  void setCloseCallback(CloseCallback closeCallback) {
    closeCallback_ = closeCallback;
  }
  
  InetAddress localAddress() const { return localAddress_; }
  InetAddress peerAddress() const { return peerAddress_; }
  
  // 启动对 TCP 连接的监听,仅能调用一次
  // 当 TcpServer 创建一个 TcpConnection 并完成相应的初始化后
  // 会调用该函数，该函数会启动在此连接上的事件监听并调用用户回调
  void connectEstablished();

  // TCP连接关闭时最后一个调用的函数，
  // 彻底移除对当前 TcpConnection 所管理的文件描述符的事件关注
  // 此函数一般由 TcpServer::removeConnection 移除该TcpConnection
  // 后调用，也可能直接调用
  void connectDestroyed();

  // 关闭连接的写端,线程安全的
  // 实现优雅的 TCP 关闭
  void shutdown() {
    // 这里不具有线程安全性
    //if (state_ == kConnected) {
    //  setState(kDisConnecting);
    //  ownerLoop_->runInLoop(std::bind( 
    //                        &TcpConnection::shutdownInLoop, this));
    //}
    if (__atomic_exchange_n(&state_, kDisConnecting, __ATOMIC_SEQ_CST)) {
      ownerLoop_->runInLoop(std::bind( \
                            &TcpConnection::shutdownInLoop, this));
    }
  }

  // 发送数据，线程安全的
  void send(const void* data, size_t len) {
    send(StringPiece(static_cast<const char*>(data), len));
  }
  void send(const StringPiece& str);
  void send(Buffer* buf);

  void setTcpNoDelay(bool on) {
    connSock_->setTcpNoDelay(on);
  }

  void setTcpKeepAlive(bool on) {
    connSock_->setKeepAlive(on);
  }

 private:
  // 当前 TCP 连接的状态
  enum StateE {
    kConnecting,
    kConnected,
    kDisConnecting,   // 用于主动断开连接调用shutdown()之后的状态
    kDisConnected
  };

  void setState(StateE state) { state_ = state; }
  std::string stateToString();
  // connChannel_ 上的回调函数
  void handleRead(Timestamp receiveTime);
  void handleWrite();
  void handleClose();
  void handleError();
  

  void shutdownInLoop() {
    ownerLoop_->assertInLoopThread();
    // 如果当前发送缓冲区无内容，则可直接 shutdownWrite()
    // 否则不能立即关闭，需延迟到数据发送完毕时关闭连接
    if (!connChannel_->isWriting()) {
      connSock_->shutdownWrite();
    }
  }

  void sendInLoop(const char* data, size_t len);
  void sendInLoop(const StringPiece& str) {
    sendInLoop(str.data(), str.size());
  }

  EventLoop* ownerLoop_;
  const std::string name_;  // 连接名称（服务器的名称 + 序列号）
  StateE state_;   // 连接的状态
  std::unique_ptr<Socket> connSock_;  // 持有连接描述符
  std::unique_ptr<Channel> connChannel_;  // 连接描述符对应的事件通道
  InetAddress localAddress_;
  InetAddress peerAddress_;
  Buffer inputBuffer_;  // TCP 连接的接收缓冲区
  Buffer outputBuffer_;  // TCP 连接的发送缓冲区

  // TCP连接需要处理的 3 个半事件
  ConnectionCallback connectionCallback_;
  MessageCallback messageCallback_;
  WriteCompleteCallback writeCompleteCallback_;
  // 绑定到 TcpServer::removeConnection()，
  // 当 TcpConnection 所管理的连接关闭时，会调用此回调，
  // 用于删除 TcpConnection
  CloseCallback closeCallback_;
};

}  // namespace net
}  // namespace tinyWeb