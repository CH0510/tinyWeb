#include "InetAddress.h"

#include <netdb.h>

#include "../../base/src/Logging.h"

namespace tinyWeb {
namespace net {

InetAddress::InetAddress(uint16_t port, bool loopbackOnly, bool ipv6) {
  if (ipv6) {
    ::memset(&addr6_, 0, sizeof addr6_);
    addr6_.sin6_family = AF_INET6;
    addr6_.sin6_port = sockets::hostToNetwork16(port);
    addr6_.sin6_addr = \
            loopbackOnly ? in6addr_loopback : in6addr_any;
  } else {
    ::memset(&addr_, 0, sizeof addr_);
    addr_.sin_family = AF_INET;
    addr_.sin_port = sockets::hostToNetwork16(port);
    in_addr_t ip = loopbackOnly ? INADDR_LOOPBACK : INADDR_ANY;
    addr_.sin_addr.s_addr = sockets::hostToNetwork32(ip);
  }
}

InetAddress::InetAddress(StringArg ip, uint16_t port, bool ipv6) {
  if (ipv6 || strchr(ip.c_str(), ':')) {
    ::memset(&addr6_, 0, sizeof addr6_);
    sockets::fromIpPort(ip.c_str(), port, &addr6_);
  } else {
    ::memset(&addr_, 0, sizeof addr_);
    sockets::fromIpPort(ip.c_str(), port, &addr_);
  }
}

std::string InetAddress::toIp() const {
  char buf[64] = {0};
  sockets::toIp(getSockAddr(), buf, sizeof buf);
  return buf;
}

std::string InetAddress::toIpPort() const {
  char buf[64] = {0};
  sockets::toIpPort(getSockAddr(), buf, sizeof buf);
  return buf;
}

bool InetAddress::resolve(StringArg hostname, InetAddress* out) {
  assert(out != NULL);
  struct addrinfo hints;
  struct addrinfo *result;
  ::bzero(&hints, sizeof hints);
  hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_CANONNAME;
  int ret = getaddrinfo(hostname.c_str(), NULL, &hints, &result);
  if (ret != 0) {
      LOG_SYSERROR << "InetAddress::resolve() failed: " \
                   << gai_strerror(ret);
      return false;
  }

  /* getaddrinfo() returns a list of address structures.*/
  if (result->ai_family == AF_INET) {
    out->addr_.sin_addr = reinterpret_cast<struct sockaddr_in*>( \
        result->ai_addr)->sin_addr;
  } else {
    out->addr6_.sin6_addr = reinterpret_cast<struct sockaddr_in6*>( \
        result->ai_addr)->sin6_addr;
  }

  freeaddrinfo(result);           /* No longer needed */
  return true;
}

}  // namespace net
}  // namespace tinyWeb