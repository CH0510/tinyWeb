#pragma once

#include <unistd.h>

#include <string>

#include "InetAddress.h"
#include "../../base/src/noncopyable.hpp"

namespace tinyWeb {
namespace net {

class Socket {
 public:
  Socket(int fd) : fd_(fd) {
  }

  ~Socket() {
    ::close(fd_);
  }

  int fd() const { return fd_; }

  void bindAddress(const InetAddress& localAddr);
  void listen();
  int accept(InetAddress* peerAddr);

  void shutdownWrite();

  void setTcpNoDelay(bool on);
  // 须在使用bindAddress前调用
  void setReuseAddr(bool on);
  // 须在使用bindAddress前调用
  void setReusePort(bool on);

  void setKeepAlive(bool on);
  // 设置close的行为
  void setLinger(bool on, int seconds);
  // 设置接受缓冲区大小
  // 须在connect或listen之前调用
  void setReceiveBufferSize(int size);
  // 设置发送缓冲区大小
  void setSendBufferSize(int size);
  // 设置接受低水位大小
  void setReceiveLowat(int size);
  // 设置发送低水位标志
  void setSendLowat(int size);

 private:
  const int fd_;
};

}  // namespace net
}  // namespace tinyWeb