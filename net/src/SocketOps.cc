#include  "SocketOps.h"

#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <stdio.h>
#include <errno.h>
#include <sys/uio.h>

#include "Endian.h"
#include "../../base/src/Logging.h"
#include "../../base/src/Types.h"

namespace tinyWeb {
namespace net {
namespace sockets {

typedef struct sockaddr SA;

void setNonBlockingAndCloseOnExec(int sockfd) {
  int flag = ::fcntl(sockfd, F_GETFL, 0);
  flag |= O_NONBLOCK | FD_CLOEXEC;
  ::fcntl(sockfd, F_SETFL, flag);
}

const struct sockaddr *sockaddr_cast( \
                        const struct sockaddr_in *addr) {
  return reinterpret_cast<const struct sockaddr *>(addr);
}

const struct sockaddr *sockaddr_cast( \
                        const struct sockaddr_in6 *addr) {
  return reinterpret_cast<const struct sockaddr *>(addr);
}

struct sockaddr *sockaddr_cast(struct sockaddr_in6 *addr) {
  return reinterpret_cast<struct sockaddr *>(addr);
}

const struct sockaddr_in *sockaddr_in_cast( \
                        const struct sockaddr *addr) {
  return reinterpret_cast<const struct sockaddr_in *>(addr);
}

const struct sockaddr_in6 *sockaddr_in6_cast( \
                        const struct sockaddr *addr) {
  return reinterpret_cast<const struct sockaddr_in6 *>(addr);
}

int createNonBlockingOrDie(sa_family_t family) {
  int sockfd = ::socket(AF_INET, \
        SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
  if (sockfd < 0) {
    LOG_FATAL << "sockets::createNonBlockingOrDie()";
  }

  return sockfd;
}

int connect(int sockfd, const struct sockaddr *addr) {
  return ::connect(sockfd, addr, \
          static_cast<socklen_t>(sizeof(struct sockaddr_in6)));
}

void bindOrDie(int sockfd, const struct sockaddr *addr) {
  int ret =  ::bind(sockfd, addr, \
          static_cast<socklen_t>(sizeof(struct sockaddr_in6)));
  if (ret < 0) {
    LOG_FATAL << "sockets::bindOrDie()";
  }
}

void listenOrDie(int sockfd) {
  int ret = ::listen(sockfd, SOMAXCONN);
  if (ret < 0) {
    LOG_FATAL << "sockets::listen()";
  }
}

int accept(int sockfd, struct sockaddr_in6 *addr) {
  socklen_t addrLen = static_cast<socklen_t>(sizeof *addr);
  int connfd = ::accept4(sockfd, sockaddr_cast(addr), &addrLen, \
                       SOCK_NONBLOCK | SOCK_CLOEXEC);
  
  if (connfd < 0) {
    int saveErrno = errno;
    LOG_SYSERROR << "sockets::accept()";
    switch (saveErrno) {
      case EAGAIN:
      case ECONNABORTED:
      case EINTR:
      case EPROTO:
      case EPERM:
      case EMFILE:
        // 非致命性错误
        errno = saveErrno;
        break;
      case EBADF:
      case EFAULT:
      case EINVAL:
      case ENFILE:
      case ENOBUFS:
      case ENOMEM:
      case ENOTSOCK:
      case EOPNOTSUPP:
        // 致命性错误
        LOG_FATAL << "unexpected err of ::accept" << saveErrno;
        break;
      default:
        LOG_FATAL << "unknown error" << saveErrno;
        break;
    }
  }

  return connfd;
}

ssize_t read(int sockfd, void *buf, size_t count) {
  return ::read(sockfd, buf, count);
}

ssize_t readv(int sockfd, const struct iovec *iov, int iovcnt) {
  return ::readv(sockfd, iov, iovcnt);
}

ssize_t write(int sockfd, const void *buf, size_t count) {
  return ::write(sockfd, buf, count);
}

void close(int sockfd) {
  if (::close(sockfd) < 0) {
    LOG_SYSERROR << "sockets::close()";
  }
}

void shutdownWrite(int sockfd) {
  if(::shutdown(sockfd, SHUT_WR) < 0) {
    LOG_SYSERROR << "sockets::shutdown()";
  }
}

void toIp(const struct sockaddr *addr, \
          char* buf, size_t size) {
  if (addr->sa_family == AF_INET) {
    assert(size >= INET_ADDRSTRLEN);
    const struct sockaddr_in *addr4 = \
        sockaddr_in_cast(addr);
    ::inet_ntop(AF_INET, &addr4->sin_addr, buf, size);
  } else if (addr->sa_family == AF_INET6) {
    assert(size >= INET6_ADDRSTRLEN);
    const struct sockaddr_in6 *addr6 = \
        sockaddr_in6_cast(addr);
    ::inet_ntop(AF_INET6, &addr6->sin6_addr, buf, size);
  } else {
    errno = EAFNOSUPPORT;
    LOG_ERROR << "sockets::toIp() unknow socket family";
  }
}

void toIpPort(const struct sockaddr *addr, \
              char *buf, size_t size) {
  if (addr->sa_family == AF_INET) {
    toIp(addr, buf, size);
    size_t end = ::strlen(buf);
    const struct sockaddr_in *addr4 = \
                                sockaddr_in_cast(addr);
    uint16_t port = addr4->sin_port;
    assert(size > end);
    snprintf(buf + end, size - end, ":%u", networkToHost16(port));
  } else if (addr->sa_family == AF_INET6) {
    buf[0] = '[';
    toIp(addr, buf + 1, size - 1);
    size_t end = strlen(buf);
    const struct sockaddr_in6 *addr6 = \
                                sockaddr_in6_cast(addr);
    uint16_t port = addr6->sin6_port;
    snprintf(buf + end, size - end, "]:%u", networkToHost16(port));
  } else {
    errno = EAFNOSUPPORT;
    LOG_ERROR << "sockets::toIpPort() unknow socket family: ";
  }
}

void fromIpPort(const char *ip, uint16_t port, \
                struct sockaddr_in *addr) {
  addr->sin_family = AF_INET;
  addr->sin_port = hostToNetwork16(port);
  if (::inet_pton(AF_INET, ip, &addr->sin_addr) < 0) {
    LOG_SYSERROR << "sockets::fromIpPort()";
  }
}

void fromIpPort(const char *ip, uint64_t port, \
                struct sockaddr_in6 *addr) {
  addr->sin6_family = AF_INET6;
  addr->sin6_port = hostToNetwork16(port);
  if (::inet_pton(AF_INET6, ip, &addr->sin6_addr) < 0) {
    LOG_SYSERROR << "sockets::fromIpPort()";
  }
}

int getSocketError(int sockfd) {
  int optval;
  socklen_t optlen = static_cast<socklen_t>(sizeof optval);

  if (::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, \
                   &optval, &optlen) < 0) {
    return errno;
  } else {
    return optval;
  }
}

struct sockaddr_in6 getLocalAddr(int sockfd) {
  struct sockaddr_in6 localAddr;
  ::memset(&localAddr, 0, sizeof localAddr);
  socklen_t addrLen = static_cast<socklen_t>(sizeof localAddr);
  if (::getsockname(sockfd, sockaddr_cast(&localAddr), &addrLen) < 0) {
    LOG_SYSERROR << "sockets::getLocalAddr()";
  }

  return localAddr;
}

struct sockaddr_in6 getPeerAddr(int sockfd) {
  struct sockaddr_in6 peerAddr;
  ::memset(&peerAddr, 0, sizeof peerAddr);
  socklen_t addrLen = sizeof(peerAddr);
  if (::getpeername(sockfd, sockaddr_cast(&peerAddr), &addrLen) < 0) {
    LOG_ERROR << "sockets::getPeerAddr()";
  }

  return peerAddr;
}

bool isSelfConnect(int sockfd) {
  struct sockaddr_in6 localAddr = getLocalAddr(sockfd);
  struct sockaddr_in6 peerAddr = getPeerAddr(sockfd);
  if (localAddr.sin6_family == AF_INET) {
    const struct sockaddr_in *laddr4 = \
          reinterpret_cast<const struct sockaddr_in *>(&localAddr);
    const struct sockaddr_in *paddr4 = \
          reinterpret_cast<const struct sockaddr_in *>(&peerAddr);
    return laddr4->sin_port == paddr4->sin_port && \
           laddr4->sin_addr.s_addr == paddr4->sin_addr.s_addr;
  } else if (localAddr.sin6_family == AF_INET6) {
    return localAddr.sin6_port == peerAddr.sin6_port && \
           memcmp(&localAddr.sin6_addr, &peerAddr.sin6_addr, \
                    sizeof localAddr.sin6_addr) == 0;
  } else {
    return false;
  }
}

}  // namespace sockets
}  // namespace net
}  // namespace tinyWeb