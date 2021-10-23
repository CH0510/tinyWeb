#include "TcpConnection.h"

#include <unistd.h>

#include "Socket.h"
#include "Channel.h"

namespace tinyWeb {
namespace net {

TcpConnection::TcpConnection(EventLoop* loop, \
            const std::string& name, \
            int connfd, \
            const InetAddress& localAddr, \
            const InetAddress& peerAddr) \
  : ownerLoop_(loop), \
    name_(name), \
    state_(StateE::kConnecting), \
    connSock_(new Socket(connfd)), \
    connChannel_(new Channel(loop, connfd)), \
    localAddress_(localAddr), \
    peerAddress_(peerAddr) {
  connChannel_->setReadCallback(std::bind(&TcpConnection::handleRead, this));
}

TcpConnection::~TcpConnection() {
  ownerLoop_->assertInLoopThread();
  connChannel_->disableAll();
  connChannel_->remove();
}

void TcpConnection::connectEstablished() {
  ownerLoop_->assertInLoopThread();
  assert(state_ == kConnecting);
  state_ = kConnected;
  connChannel_->enableReading();

  connectionCallback_(shared_from_this());
}

void TcpConnection::handleRead() {
  ownerLoop_->assertInLoopThread();
  char buf[65536] = {0};
  ssize_t len = ::read(connSock_->fd(), buf, sizeof buf);
  messageCallback_(shared_from_this() , buf, len);
}

}  // namespace net
}  // namespace tinyWeb