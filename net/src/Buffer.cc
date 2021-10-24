#include "Buffer.h"

#include <assert.h>
#include <sys/uio.h>  // writev readv

#include <utility>

#include "SocketOps.h"

namespace tinyWeb {
namespace net {

const size_t Buffer::kCheapPrepend = 8;
const size_t Buffer::kInitialSize = 1024;
const char Buffer::kCRLF[] = "\r\n";

Buffer::Buffer(size_t initialSize) \
  : buffer_(initialSize + kCheapPrepend), \
    readIndex_(kCheapPrepend), \
    writeIndex_(kCheapPrepend) {
  assert(readableBytes() == 0);
  assert(writeableBytes() == initialSize);
  assert(prependableBytes() == kCheapPrepend);
}

ssize_t Buffer::readFd(int fd, int *saveErrno) {
  char extrabuf[65536];
  struct iovec vec[2];
  const size_t writeable = writeableBytes();
  vec[0].iov_base = beginWrite();
  vec[0].iov_len = writeableBytes();
  vec[1].iov_base = extrabuf;
  vec[1].iov_len = sizeof extrabuf;
  // 如果 buffer_ 的可读区域足够64K大，则不需要extrabuf
  const int iovcnt = (writeable < sizeof extrabuf) ? 2 : 1;
  const ssize_t n = sockets::readv(fd, vec, iovcnt);
  if (n < 0) {
    *saveErrno = errno;
  } else if (static_cast<size_t>(n) <= writeable) {
    hasWriten(n);
  } else {
    writeIndex_ = buffer_.size();
    append(extrabuf, n - writeable);
  }

  return n;
}


}  // namespace net
}  // namespace tinyWeb