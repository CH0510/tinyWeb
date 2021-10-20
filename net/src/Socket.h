#pragma once

#include <unistd.h>

#include <string>

#include "InetAddress.h"

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

  bool getTcpInfo(struct tcp_info *tcpInfo) const;
  bool getTcpInfo(char *buf, size_t len) const;

  void bindAddress(const InetAddress& localAddr);
  void listen();
  int accept(InetAddress* peerAddr);

  void shutdownWrite();

  void setTcpNoDelay(bool on);

  void setReuseAddr(bool on);

  void setReusePort(bool on);

  void setKeepAlive(bool on);

 private:
  const int fd_;
};

}  // namespace net
}  // namespace tinyWeb