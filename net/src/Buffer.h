#pragma once

#include <sys/types.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>

#include <vector>
#include <string>

#include "../../base/src/StringPiece.hpp"
#include "Endian.h"

namespace tinyWeb {
namespace net {

class Buffer {
 public:
  static const size_t kCheapPrepend;
  static const size_t kInitialSize;
  static const char kCRLF[];

  Buffer(size_t initialSize = kInitialSize);
  
  size_t prependableBytes() const { return readIndex_; }
  size_t readableBytes() const { return writeIndex_ - readIndex_; }
  size_t writeableBytes() const { return buffer_.size() - writeIndex_; }

  void swap(Buffer& rhs) {
    buffer_.swap(rhs.buffer_);
    std::swap(writeIndex_, rhs.writeIndex_);
    std::swap(readIndex_, rhs.readIndex_);
  }
  // 可读位置的开始
  const char *peek() const { return begin() + readIndex_; }
  // 可写位置的开始
  const char *beginWrite() const { return begin() + writeIndex_; }
  char* beginWrite() { return begin() + writeIndex_; }
  
  // 查找 start 开始的字符串的第一个 \r\n 的位置
  const char *findCRLF(const char *start) const {
    assert(start >= peek());
    assert(start <= beginWrite());
    return static_cast<const char*>( \
              ::memmem(start, beginWrite() - start, kCRLF, 2));
  }
  
  // 查找可读区域的第一个 '\n' 的位置
  const char *findEOL() const {
    return static_cast<const char*> ( \
      ::memchr(peek(), '\n', readableBytes()));
  }
  const char * findEOL(const char* start) const {
    assert(start >= peek());
    assert(start <= beginWrite());
    return static_cast<const char*> ( \
      ::memchr(start, '\n', beginWrite() - start));
  }

  // 下列函数用于读取数据后调整 readIndex_ 指针
  void retrieve(size_t len) {
    assert(len <= readableBytes());
    if (len < readableBytes()) {
      readIndex_ += len;
    } else {
      retrieveAll();
    }

  }

  void retrieveAll() {
    readIndex_ = kCheapPrepend;
    writeIndex_ = kCheapPrepend;
  }

  void retrieUntil(const char* end) {
    assert(end >= peek());
    assert(end <= beginWrite());
    retrieve(end - peek());
  }

  std::string retrieveAsString(size_t len) {
    assert(len <= readableBytes());
    std::string result(peek(), len);
    retrieve(len);
    return result;
  }

  std::string retrieveAllAsString() {
    return retrieveAsString(readableBytes());
  }

  StringPiece toStringPiece() const {
    return StringPiece(peek(), readableBytes());
  }

  void hasWriten(size_t len) {
    assert(len <= writeableBytes());
    writeIndex_ += len;
  }

  void append(const char* data, size_t len) {
    ensureWriteableBytes(len);
    ::memmove(beginWrite(), data, len);
    hasWriten(len);
  }

  void append(const void* data, size_t len) {
    append(static_cast<const char*>(data), len);
  }

  void append(const StringPiece& str) {
    append(str.begin(), str.size());
  }

  void ensureWriteableBytes(size_t len) {
    if (len > writeableBytes()) {
      makeSpace(len);
    }
    assert(writeableBytes() >= len);
  }

  void unwrite(size_t len) {
    assert(len <= readableBytes());
    writeIndex_ -= len;
  }

  void appendInt64(int64_t x) {
    int64_t be64 = sockets::hostToNetwork64(x);
    append(&be64, sizeof be64);
  }

  void appendInt32(int32_t x) {
    int32_t be32 = sockets::hostToNetwork32(x);
    append(&be32, sizeof be32);
  }

  void appendInt16(int16_t x) {
    int16_t be16 = sockets::hostToNetwork16(x);
    append(&be16, sizeof be16);
  }

  void appendInt8(int8_t x) {
    append(&x, sizeof x);
  }

  void retrieveInt64() {
    retrieve(sizeof(int64_t));
  }

  int64_t peekInt64() const {
    assert(readableBytes() >= sizeof(int64_t));
    int64_t be64 = 0;
    ::memcpy(&be64, peek(), sizeof(int64_t));
    return sockets::networkToHost64(be64);
  }

  int64_t readInt64() {
    int64_t result = peekInt64();
    retrieveInt64();
    return result;
  }

  void retrieveInt32() {
    retrieve(sizeof(int32_t));
  }

  int32_t peekInt32() const {
    assert(readableBytes() >= sizeof(int32_t));
    int32_t be32 = 0;
    ::memcpy(&be32, peek(), sizeof(int32_t));
    return sockets::networkToHost32(be32);
  }

  int32_t readInt32() {
    int32_t result = peekInt32();
    retrieveInt32();
    return result;
  }

  void retrieveInt16() {
    retrieve(sizeof(int16_t));
  }

  int16_t peekInt16() const {
    assert(readableBytes() >= sizeof(int16_t));
    int16_t be16 = 0;
    ::memcpy(&be16, peek(), sizeof(int16_t));
    return sockets::networkToHost16(be16);
  }

  int16_t readInt16() {
    int16_t result = peekInt16();
    retrieveInt16();
    return result;
  }

  void retrieveInt8() {
    retrieve(sizeof(int8_t));
  }

  int8_t peekInt8() const {
    assert(readableBytes() >= sizeof(int8_t));
    int8_t x = *peek();
    return x;
  }

  int8_t readInt8() {
    int8_t result = peekInt8();
    retrieveInt8();
    return result;
  }

  // 在 prepend 的地方添加字段
  void prepend(const void* __restrict__ data, size_t len ) {
    assert(len <= prependableBytes());
    readIndex_ -= len;
    const char* src = static_cast<const char*>(data);
    ::memcpy(begin() + readIndex_, src, len);
  }

  void prependInt64(int64_t x) {
    int64_t be64 = sockets::hostToNetwork64(x);
    prepend(&be64, sizeof be64);
  }

  void prependInt32(int32_t x) {
    int32_t be32 = sockets::hostToNetwork32(x);
    prepend(&be32, sizeof be32);
  }

  void prependInt16(int16_t x) {
    int16_t be16 = sockets::hostToNetwork16(x);
    prepend(&be16, sizeof be16);
  }

  void prependInt8(int8_t x) {
    prepend(&x, sizeof(x));
  }

  // 仅预留 reserve 的可写空间
  // 如果 prepend 的空间大于 kCheapPrepend 会触发内存挪腾
  void shrink(size_t reserve) {
    if (prependableBytes() > kCheapPrepend) {
      size_t readable = readableBytes();
      ::memmove(begin() + kCheapPrepend, peek(), readable);
      readIndex_ = kCheapPrepend;
      writeIndex_ = kCheapPrepend + readable;
      assert(readableBytes() == readable);
    }
    int size = writeIndex_ + reserve;
    buffer_.resize(size);
    buffer_.shrink_to_fit();
  }

  size_t internalCapacity() const {
    return buffer_.capacity();
  }

  // 从文件描述符读取数据到缓冲区
  ssize_t readFd(int fd, int* saveErrno);

 private:
  const char* begin() const { return buffer_.data(); }
  char* begin() { return buffer_.data(); }

  void makeSpace(size_t len) {
    // 注意在prepend前面已填入字段后就不能向后搬移，否则会扰乱数据
    if (prependableBytes() > kCheapPrepend) {
      size_t readable = readableBytes();
      ::memmove(begin() + kCheapPrepend, peek(), readableBytes());
      readIndex_ = kCheapPrepend;
      writeIndex_ = kCheapPrepend + readable;
      assert(readableBytes() == readable);
    }

    // 即使进行了数据挪腾后还是不够用，需要调整buffer_大小
    if (writeableBytes() < len) {
      buffer_.resize(len + writeIndex_);
    }
  }

  std::vector<char> buffer_;
  size_t readIndex_;
  size_t writeIndex_;
};

}  // namespace net
}  // namespace tinyWeb
