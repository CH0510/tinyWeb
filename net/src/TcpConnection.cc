#include "TcpConnection.h"

#include <unistd.h>
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
  // connSock_ 上发生时间时，会调用相应回调
  connChannel_->setReadCallback(std::bind( \
                     &TcpConnection::handleRead, this, std::placeholders::_1));
  connChannel_->setWriteCallback(std::bind(&TcpConnection::handleWrite, this));
  connChannel_->setCloseCallback(std::bind(&TcpConnection::handleClose, this));
  connChannel_->setErrorCallback(std::bind(&TcpConnection::handleError, this));
  LOG_DEBUG << "TcpConnection::ctor[" <<  name_ << "] at " << this
          << " fd=" << connfd;
  connSock_->setKeepAlive(true);
}

TcpConnection::~TcpConnection() {
  LOG_DEBUG << "TcpConnection::dtor[" << name_ << "] at " \
            << this << "fd = " << connSock_->fd() \
            << " state = " << stateToString();
}

void TcpConnection::connectEstablished() {
  ownerLoop_->assertInLoopThread();
  assert(state_ == kConnecting);
  setState(kConnected);
  connChannel_->enableReading();
  // 由于采用的是 epoll 的水平触发模式，此时开启
  // 会导致 busy loop
  // connChannel_->enableWriting();
  connectionCallback_(shared_from_this());
}

void TcpConnection::connectDestroyed() {
  ownerLoop_->assertInLoopThread();
  // 可能绕过 handleClose 直接调用connectDestroyed
  if (state_ == kConnected || state_ == kDisConnecting) {
    setState(kDisConnected);
    connChannel_->disableAll();
    // 是否应该调用 TcpServer 的 removeConnection?
    // closeCallback(shared_from_this());
  }

  connectionCallback_(shared_from_this());
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

void TcpConnection::handleRead(Timestamp receiveTime) {
  ownerLoop_->assertInLoopThread();
  int saveErrno = 0;
  ssize_t len = inputBuffer_.readFd(connSock_->fd(), &saveErrno);
  if (len > 0) {
    messageCallback_(shared_from_this() , &inputBuffer_, receiveTime);
  } else if (len == 0) {
    handleClose();
  } else {
    handleError();
  }
}

void TcpConnection::handleWrite() {
  ownerLoop_->assertInLoopThread();
  if (connChannel_->isWriting()) {
    ssize_t nwriten = sockets::write(connSock_->fd(), \
                               outputBuffer_.peek(), \
                               outputBuffer_.readableBytes());
    if (nwriten >= 0) {
      outputBuffer_.retrieve(nwriten);
      if (outputBuffer_.readableBytes() == 0) {
        //  发送缓冲区发送完毕后需关闭写事件防止 busy loop
        connChannel_->disableWriting();
        if (writeCompleteCallback_) {
          // ！！！注意此处需在事件循环结束后才执行
          // 防止writeCompleteCallback调用 send 形成魂环调用
          // 导致服务器失去响应
          ownerLoop_->queueInLoop(std::bind( \
                                writeCompleteCallback_, \
                                shared_from_this()));
        }
        // 发送缓冲区发送完毕，且之前调用过 shutdown() 则直接关闭套接字写端
        if (state_ == kDisConnecting) {
          shutdownInLoop();
        }
      }
    } else {
      LOG_SYSERROR << "TcpConnection::handleWrite()";
    }
  } else {
    // logic error
    LOG_WARN << "TcpConnection::handleWrite() fd = " \
             << connSock_->fd() << " don't enable writing";
  }
}

void TcpConnection::handleClose() {
  ownerLoop_->assertInLoopThread();
  LOG_DEBUG << "TcpConnection::handleClose fd = " << connSock_->fd() \
            << " state = " << stateToString();
  assert(state_ == kConnected || state_ == kDisConnecting);
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

void TcpConnection::send(const StringPiece& str) {
  if (state_ == kConnected) {
    if (ownerLoop_->isInLoopThread()) {
      // 在本 IO 线程下就直接调用，防止拷贝带来的性能损失
      sendInLoop(str.data(), str.size());
    } else {
      // 由于存在函数重载，此处必须使用函数指针
      void (TcpConnection::*cb)(const StringPiece&) = \
                                      &TcpConnection::sendInLoop;
      // 此处应转为独立的string，因为离开该函数后，str可能会消失
      ownerLoop_->runInLoop(std::bind( \
                            cb, \
                            this, \
                            str.as_string()));
    }
  }
}

void TcpConnection::send(Buffer* buf) {
  if (state_ == kConnected) {
    // 在本 IO 线程中就直接调用，防止拷贝带来的性能损失
    if (ownerLoop_->isInLoopThread()) {
      sendInLoop(buf->peek(), buf->readableBytes());
      buf->retrieveAll();
    } else {
      void (TcpConnection::*cb)(const StringPiece& str) = \
                                        &TcpConnection::sendInLoop;
      // 此处应将buf里的内容转为单独的string,因为离开本函数后buf的内容将是未知的
      ownerLoop_->runInLoop(std::bind( \
                            cb, \
                            this, \
                            buf->retrieveAllAsString()));
    }
  } 
}

void TcpConnection::sendInLoop(const char* data, size_t len) {
  ownerLoop_->assertInLoopThread();
  ssize_t nwriten = 0;
  size_t remaining = len;
  bool faultError = false;

  // 防止在已断开TCP链接后客户仍调用send进行发送数据
  if (state_ == kDisConnected) {
    LOG_WARN << "disconneced, give up write";
    return;
  }

  if (!connChannel_->isWriting()) {
    // 如果发送缓冲区没有数据，则尝试直接发送
    nwriten = sockets::write(connSock_->fd(), \
                                  data, \
                                  len);
    if (nwriten >= 0) {
      remaining -= nwriten;
      if (remaining == 0 && writeCompleteCallback_) {
        // ！！！注意此处需在事件循环结束后才执行
        // 防止writeCompleteCallback调用 send 形成魂环调用
        // 导致服务器失去响应
        ownerLoop_->queueInLoop(std::bind( \
                              writeCompleteCallback_, \
                              shared_from_this()));
      }
    } else {
      nwriten = 0;  // 防止后面 data + nwriten 时数组越界
      if (errno != EWOULDBLOCK) {
        LOG_SYSERROR << "TcpConnection::sendInLoop";
        if (errno == EPIPE || errno == ECONNRESET) {
          faultError = true;
        }
      }
    }
  }

  assert(0 <= remaining && remaining <= len);
  // 1. 不存在错误并且数据未写完
  // 2. 原本发送缓冲区仍存在未发送数据
  // 将数据追加到发送缓冲区并开启写事件监听
  if (!faultError && remaining > 0) {
    outputBuffer_.append(data + nwriten, remaining);
    if (!connChannel_->isWriting()) {
      connChannel_->enableWriting();
    }
  }
}

}  // namespace net
}  // namespace tinyWeb