#include "TcpConnection.h"

#include <unistd.h>

#include "Socket.h"
#include "Channel.h"
#include "../../base/src/Logging.h"

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
  connChannel_->setWriteCallback(std::bind(&TcpConnection::handleWrite, this));
  connChannel_->setCloseCallback(std::bind(&TcpConnection::handleClose, this));
  connChannel_->setErrorCallback(std::bind(&TcpConnection::handleError, this));
}

TcpConnection::~TcpConnection() {
  LOG_DEBUG << "TcpConnection::dtor[" << name_ << "] at " \
            << this << "fd = " << connSock_->fd() \
            << " state = " << stateToString();
}

void TcpConnection::connectEstablished() {
  ownerLoop_->assertInLoopThread();
  assert(state_ == kConnecting);
  state_ = kConnected;
  connChannel_->enableReading();
  // 由于采用的是 epoll 的水平触发模式，此时开启
  // 会导致 busy loop
  // connChannel_->enableWriting();

  connectionCallback_(shared_from_this());
}

void TcpConnection::connectDestroyed() {
  ownerLoop_->assertInLoopThread();
  // 可能绕过 handleClose 直接调用connectDestroyed
  if (state_ == kConnected) {
    setState(kDisConnected);
    connChannel_->disableAll();
    // 是否应该调用 TcpServer 的 removeConnection?
    // closeCallback(shared_from_this());
  }

  if (connectionCallback_) {
    connectionCallback_(shared_from_this());
  }

  connChannel_->remove();
}

std::string TcpConnection::stateToString() {
  switch (state_) {
    case kDisConnected:
      return "kDisConnected";
    case kConnecting:
      return "kConnecting";
    case kConnected:
      return "kConnected";
    default:
      return "unknown state";
  }
}

void TcpConnection::handleRead() {
  ownerLoop_->assertInLoopThread();
  char buf[65536] = {0};
  ssize_t len = ::read(connSock_->fd(), buf, sizeof buf);
  if (len > 0) {
    messageCallback_(shared_from_this() , buf, len);
  } else if (len == 0) {
    handleClose();
  } else {
    handleError();
  }
}

// TODO
void TcpConnection::handleWrite() {
  
}

void TcpConnection::handleClose() {
  ownerLoop_->assertInLoopThread();
  LOG_DEBUG << "TcpConnection::handleClose fd = " << connSock_->fd() \
            << " state = " << stateToString();
  assert(state_ == kConnected);
  setState(kDisConnected);
  connChannel_->disableAll();
  // 将调用 TcpServer 的 removeConnection,
  // 将此 TcpConnection 管理的连接从 TcpServer 的
  // 列表中删除。最后会调用 TcpCOnnection::connectDestroy做收尾工作
  closeCallback_(shared_from_this());
}

void TcpConnection::handleError() {
  ownerLoop_->assertInLoopThread();
  int err = sockets::getSocketError(connSock_->fd());
  char buf[128] = {0};
  LOG_ERROR << "TcpConnection::handleError [" << name_ \
            << "] - SO_ERROR = " << err << " " \
            << ::strerror_r(err, buf, sizeof buf);
}

}  // namespace net
}  // namespace tinyWeb