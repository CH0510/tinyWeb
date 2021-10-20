#include "Acceptor.h"

#include <unistd.h>
#include <fcntl.h>
#include <assert.h>

#include "SocketOps.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "../../base/src/Logging.h"

namespace tinyWeb {
namespace net {

Acceptor::Acceptor(EventLoop* loop, \
                   const InetAddress& listenAddr, \
                   bool reusePort) \
  : ownerLoop_(loop), \
    listenSock_(sockets::createNonBlockingOrDie( \
                          listenAddr.family())), \
    listenChannel_(loop, listenSock_.fd()), \
    listening_(false), \
    idleFd_(::open("/dev/null", O_RDONLY | O_CLOEXEC)) {
  assert(idleFd_ >= 0);
  listenSock_.setReuseAddr(true);
  listenSock_.setReusePort(reusePort);
  listenSock_.bindAddress(listenAddr);
  listenChannel_.setReadCallback(std::bind(\
                        &Acceptor::handleRead, this));
}

Acceptor::~Acceptor() {
  listenChannel_.disableAll();
  listenChannel_.remove();
  ::close(idleFd_);
}

void Acceptor::listen() {
  ownerLoop_->assertInLoopThread();
  listening_ = true;
  listenSock_.listen();
  listenChannel_.enableReading();
}

void Acceptor::handleRead() {
  ownerLoop_->assertInLoopThread();
  InetAddress peerAddr;
  int connfd = listenSock_.accept(&peerAddr);
  if (connfd > 0) {
    if (newConnectionCallback_) {
      newConnectionCallback_(connfd, peerAddr);
    } else {
      sockets::close(connfd);
    }
  } else {
    LOG_SYSERROR << "Acceptor::handleRead()";
    // 对于文件描述符已达到进程限制的情况
    // 采用空闲文件描述符的方法解决，即：
    // 总是打开一个空闲的文件描述符，在出现EMFILE时关闭
    // 这样就能成功建立连接，并立即关闭重新打开空闲文件描述符
    if (errno == EMFILE) {
      ::close(idleFd_);
      idleFd_ = ::accept(listenSock_.fd(), NULL, NULL);
      ::close(idleFd_);
      idleFd_ = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
    }
  }
}

}  // namespace net
}  // namespace tinyWeb