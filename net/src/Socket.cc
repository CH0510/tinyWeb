#include "Socket.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include "../../base/src/Types.h"

namespace tinyWeb {
namespace net {

void Socket::bindAddress(const InetAddress& localAddr) {
  sockets::bindOrDie(fd_, localAddr.getSockAddr());
}

void Socket::listen() {
  sockets::listenOrDie(fd_);
}

int Socket::accept(InetAddress* peerAddr) {
  struct sockaddr_in6 addr;
  ::memset(&addr, 0 , sizeof(addr));
  int connfd = sockets::accept(fd_, &addr);
  if (connfd >= 0) {
    peerAddr->setSockAddrInet6(addr);
  }

  return connfd;
}

void Socket::shutdownWrite() {
  sockets::shutdownWrite(fd_);
}

void Socket::setTcpNoDelay(bool on) {
  int optval = on ? 1 : 0;
  MCHECK(::setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY, \
          &optval, static_cast<socklen_t>(sizeof(optval))));
}

void Socket::setReuseAddr(bool on) {
  int optval  = on ? 1 : 0;
  MCHECK(::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, \
          &optval, static_cast<socklen_t>(sizeof(optval))));
}

void Socket::setReusePort(bool on) {
  int optval = on ? 1 : 0;
  MCHECK(::setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, \
          &optval, static_cast<socklen_t>(sizeof(optval))));
}

void Socket::setKeepAlive(bool on) {
  int optval = on ? 1 : 0;
  MCHECK(::setsockopt(fd_, SOL_SOCKET, SO_KEEPALIVE, \
          &optval, static_cast<socklen_t>(sizeof(optval))));
}

void Socket::setLinger(bool on, int seconds) {
  struct linger lg;
  lg.l_onoff = on;
  if (on) lg.l_linger = seconds;
  MCHECK(::setsockopt(fd_, SOL_SOCKET, SO_LINGER, \
                &lg, static_cast<socklen_t>(sizeof(lg))));
}

void Socket::setReceiveBufferSize(int size) {
  MCHECK(::setsockopt(fd_, SOL_SOCKET, SO_RCVBUF, \
              &size, static_cast<socklen_t>(sizeof(size))));
}

void Socket::setSendBufferSize(int size) {
  MCHECK(::setsockopt(fd_, SOL_SOCKET, SO_SNDBUF, \
              &size, static_cast<socklen_t>(sizeof(size))));
}

void Socket::setReceiveLowat(int size) {
  MCHECK(::setsockopt(fd_, SOL_SOCKET, SO_RCVLOWAT, \
              &size, static_cast<socklen_t>(sizeof(size))));
}

void Socket::setSendLowat(int size) {
  MCHECK(::setsockopt(fd_, SOL_SOCKET, SO_SNDLOWAT, \
              &size, static_cast<socklen_t>(sizeof(size))));
}

void Socket::setReceiveTimeout(struct timeval timeout) {
  MCHECK(::setsockopt(fd_, SOL_SOCKET, SO_RCVTIMEO, \
              &timeout, static_cast<socklen_t>(sizeof timeout)));
}

void Socket::setSendTimeout(struct timeval timeout) {
  MCHECK(::setsockopt(fd_, SOL_SOCKET, SO_SNDTIMEO, \
              &timeout, static_cast<socklen_t>(sizeof timeout)));
}

}  // namespace net
}  // namespace tinyWeb