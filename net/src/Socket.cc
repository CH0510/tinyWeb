#include "Socket.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#include "../../base/src/Types.h"

namespace tinyWeb {
namespace net {

bool Socket::getTcpInfo(struct tcp_info *tcpInfo) const {
  socklen_t len = sizeof(*tcpInfo);
  memset(tcpInfo, 0, len);
  return ::getsockopt(fd_, IPPROTO_TCP, \
                      TCP_INFO, tcpInfo, &len) == 0;
}


bool Socket::getTcpInfo(char *buf, size_t len) const {
  struct tcp_info tcpi;
  bool ok = getTcpInfo(&tcpi);
  if (ok) {
    snprintf(buf, len, "unrecovered=%u "
          "rto=%u ato=%u snd_mss=%u rcv_mss=%u "
          "lost=%u retrans=%u rtt=%u rttvar=%u "
          "sshthresh=%u cwnd=%u total_retrans=%u",
          tcpi.tcpi_retransmits,  // Number of unrecovered [RTO] timeouts
          tcpi.tcpi_rto,          // Retransmit timeout in usec
          tcpi.tcpi_ato,          // Predicted tick of soft clock in usec
          tcpi.tcpi_snd_mss,
          tcpi.tcpi_rcv_mss,
          tcpi.tcpi_lost,         // Lost packets
          tcpi.tcpi_retrans,      // Retransmitted packets out
          tcpi.tcpi_rtt,          // Smoothed round trip time in usec
          tcpi.tcpi_rttvar,       // Medium deviation
          tcpi.tcpi_snd_ssthresh,
          tcpi.tcpi_snd_cwnd,
          tcpi.tcpi_total_retrans);  // Total retransmits for entire connection
  }

  return ok;
}

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
                      &optval, static_cast<socklen_t>(optval)));
}

void Socket::setReuseAddr(bool on) {
  int optval  = on ? 1 : 0;
  MCHECK(::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, \
                      &optval, static_cast<socklen_t>(optval)));
}

void Socket::setReusePort(bool on) {
  int optval = on ? 1 : 0;
  MCHECK(::setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, \
                      &optval, static_cast<socklen_t>(optval)));
}

void Socket::setKeepAlive(bool on) {
  int optval = on ? 1 : 0;
  MCHECK(::setsockopt(fd_, SOL_SOCKET, SO_KEEPALIVE, \
                      &optval, static_cast<socklen_t>(optval)));
}

}  // namespace net
}  // namespace tinyWeb