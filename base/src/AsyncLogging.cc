#include "AsyncLogging.h"

#include "stdio.h"
#include "string.h"

#include "LogFile.h"
#include "Timestamp.h"

namespace tinyWeb {

AsyncLogging::AsyncLogging(const std::string basename, \
            size_t rollingSize, \
            size_t flushInterval) \
            : basename_(basename), \
            rollingSize_(rollingSize), \
            flushInterval_(flushInterval), \
            backThread_(std::bind(&AsyncLogging::threadFunc, this), \
                        "Logging"), \
            latch_(1), \
            mutex_(), \
            cond_(mutex_), \
            currentBuffer_(new Buffer), \
            nextBuffer_(new Buffer), \
            buffers_(), \
            running_(false) {
  currentBuffer_->bzero();
  nextBuffer_->bzero();
  buffers_.reserve(16);
}

AsyncLogging::~AsyncLogging() {
  if (running_) {
    stop();
  }
}

void AsyncLogging::append(const char* logline, size_t len) {
  MutexLockGuard lock(mutex_);
  // 当前缓冲区大小足够的情况
  if (currentBuffer_->avail() > len) {
    currentBuffer_->append(logline, len);
  } else {  // 当前缓冲区大小不足
    buffers_.push_back(std::move(currentBuffer_));  // 移入已写满的缓冲区
    if (nextBuffer_) {
      currentBuffer_ = std::move(nextBuffer_);
    } else {  // 日志信息输出过快，导致前段两个缓冲区都满（很少发生）
      currentBuffer_.reset(new Buffer);
    }
    // 写入缓冲区并通知后台线程写日志文件
    currentBuffer_->append(logline, len);
    cond_.notify();
  }
}

void AsyncLogging::threadFunc() {
  assert(running_);
  BufferPtr newBuffer1(new Buffer);
  BufferPtr newBuffer2(new Buffer);
  newBuffer1->bzero();
  newBuffer2->bzero();
  LogFile output(basename_, rollingSize_, false);   // 用于后台线程写日志文件
  BufferVector bufferToWrite;
  bufferToWrite.reserve(16);
  latch_.countDown();

  while (running_) {
    assert(newBuffer1 && newBuffer1->length() == 0);
    assert(newBuffer2 && newBuffer2->length() == 0);
    assert(bufferToWrite.empty());

    {
      MutexLockGuard lock(mutex_);
      if (buffers_.empty()) {   // 注意这是非常规的用法
        cond_.waitForSecond(flushInterval_);
      }
      // if (running_) break;   //此处不判断，这样可以使得写完当前剩余的日志
      // 使用 swap on write 技术缩小临界区的范围
      buffers_.push_back(std::move(currentBuffer_));  // 将当前缓冲区也取走
      bufferToWrite.swap(buffers_);
      currentBuffer_ = std::move(newBuffer1);
      if (!nextBuffer_) {
        nextBuffer_ = std::move(newBuffer2);
      }
    }

    assert(bufferToWrite.empty() == false);   // 至少有currentBuffer_

    // 防止前段死循环拼命的写日志，如果已写满的缓冲区过大，则只保留前2个
    if (bufferToWrite.size() > 25) {  // 每个缓冲区4M
      // 打印丢弃日的提醒
      char buf[256] = {0};
      snprintf(buf, sizeof buf, "Dropped message at %s, %zd larger buffers\n", \
               Timestamp::now().toUTCFormatString().c_str(), \
               bufferToWrite.size() - 2);
      fputs(buf, stderr);
      output.append(buf, strlen(buf));
      bufferToWrite.erase(bufferToWrite.begin() + 2, bufferToWrite.end());
    }

    //  剩余的操作只有后台线程会访问到 bufferToWrite，所以不需要保护
    for (BufferPtr& buffer : bufferToWrite) {
      output.append(buffer->data(), buffer->length());
    }

    if (bufferToWrite.size() > 2) {
      bufferToWrite.resize(2);
    }

    if (!newBuffer1) {
      assert(bufferToWrite.empty() == false);
      newBuffer1 = std::move(bufferToWrite.back());
      bufferToWrite.pop_back();
      newBuffer1->reset();  // 注意调用的是FixedBuffer的reset,而不是智能指针的
    }

    if (!newBuffer2) {
      assert(bufferToWrite.empty() == false);
      newBuffer2 = std::move(bufferToWrite.back());
      bufferToWrite.pop_back();
      newBuffer2->reset();  // 注意调用的是FixedBuffer的reset,而不是智能指针的
    }

    bufferToWrite.clear();
    output.flush();
  }

  output.flush();
}

}   // namespace tinyWeb
