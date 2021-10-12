#pragma once

#include <assert.h>

#include <functional>
#include <string>

#include "../../base/src/noncopyable.hpp"
#include "EventLoop.h"
#include "../../base/src/Timestamp.h"


namespace tinyWeb {
namespace net {

class Channel : public noncopyable {
 public:
  typedef std::function<void ()> EventCallback;
  typedef std::function<void (Timestamp)> ReadCallback;

  Channel(EventLoop* loop, int fd);
  ~Channel();

  int fd() const { return fd_; }
  
  EventLoop* ownerLoop() const { return loop_; }
  
  int events() const { return event_; }
  
  int revent() const { return revent_; }

  int index() const { return index_; }
  
  void setRevent(int event) { revent_ = event; }
  
  void setIndex(int index) { index_ = index; }
  
  void setErrorCallback(EventCallback errorCallback) {
    errorCallback_ = std::move(errorCallback);
  }

  void setWriteCallback(EventCallback writeCallback) {
    writeCallback_ = std::move(writeCallback);
  }

  void setCloseCallback(EventCallback closeCallback) {
    closeCallback_ = std::move(closeCallback);
  }

  void setReadCallback(ReadCallback readCallback) {
    readCallback_ = std::move(readCallback);
  }

  void enableReading() {
    event_ |= kReadEvent;
    update();
  }

  void disableReading() {
    event_ &= ~kReadEvent;
  }

  bool isReading() const {
    return event_ & kReadEvent;
  }

  void enableWriting() {
    event_ |= kWriteEvent;
    update();
  }

  void disableWriting() {
    event_ &= ~kWriteEvent;
    update();
  }
  
  bool isWriting() const {
    return event_ & kWriteEvent;
  }

  void disableAll() {
    event_ = 0;
    update();
  }

  bool isNoneEvent() const {
    return event_ == 0;
  }

  void remove() {
    assert(isNoneEvent());
    loop_->removeChannel(this);
    addToLoop_ = false;
  }

  void handleEvent(Timestamp receiveTime);
  std::string eventToString() const;
  std::string reventToString() const;

 private:
  static std::string eventToString(int fd, int event);

  void update() {
    loop_->updateChannel(this);
    addToLoop_ = true;
  }

  static const int kNoneEvent;
  static const int kReadEvent;
  static const int kWriteEvent;

  const int fd_;
  EventLoop* loop_;
  int event_;
  int revent_;
  int index_;
  bool addToLoop_;

  EventCallback errorCallback_;
  EventCallback writeCallback_;
  EventCallback closeCallback_;
  ReadCallback readCallback_;
  bool eventHandling_;
};

}  // namespace net
}  // namespace tinyWeb