#pragma once

#include <arpa/inet.h>
#include <stdint.h>
#include <assert.h>

#include <string>

#include "../../base/src/StringPiece.hpp"
#include "SocketOps.h"
#include "Endian.h"

namespace tinyWeb {
namespace net {

// 该类是对套接字地址的封装
class InetAddress {
 public:
  InetAddress(uint16_t port = 0, bool loopbackOnly = false, bool ipv6 = false);
  InetAddress(StringArg ip, uint16_t port, bool ipv6 = false);
  InetAddress(const struct sockaddr_in addr) { addr_ = addr; }
  InetAddress(const struct sockaddr_in6 addr) { addr6_ = addr; }
  // 获取套接字地址的协议族
  sa_family_t family() const { return addr6_.sin6_family; }
  uint16_t port() const { 
    return sockets::networkToHost16(addr6_.sin6_port); 
  }
  
  std::string toIp() const;
  std::string toIpPort() const;
  
  const struct sockaddr* getSockAddr() const {
    return sockets::sockaddr_cast(&addr6_);
  }

  // 供外部设置套接字地址值
  // 这里仅提供Ipv6的形式即可，因为Ipv6与Ipv4的前半部分是一样的
  void setSockAddrInet6(const struct sockaddr_in6& addr) {
    addr6_ = addr;
  }
  // 返回网络字节序的ipv4地址
  uint32_t ipv4NetEndian() const { 
    assert(family() == AF_INET);
    return addr_.sin_addr.s_addr; 
  }

  uint16_t portEndian() const { return addr6_.sin6_port; }
  
  void setScopeId(uint32_t scopeId) {
    if (family() == AF_INET6) {
      addr6_.sin6_scope_id = scopeId;
    }
  }

  static bool resolve(StringArg hostname, InetAddress* out);

 private:
  union {
    struct sockaddr_in addr_;
    struct sockaddr_in6 addr6_;
  };
};

}  // namespace net
}  // namespace tinyWeb