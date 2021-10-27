#pragma once

#include <memory>
#include <functional>

#include "Buffer.h"
#include "../../base/src/Timestamp.h"

namespace tinyWeb {
namespace net {

class TcpConnection;

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef std::function<void(const TcpConnectionPtr&)> ConnectionCallback;
typedef std::function<void(const TcpConnectionPtr&)> CloseCallback;
typedef std::function<void(const TcpConnectionPtr&)> WriteCompleteCallback;
typedef std::function<void(const TcpConnectionPtr&, size_t)> HighWaterMarkCallback;
typedef std::function<void(const TcpConnectionPtr&, \
                            Buffer* buf, \
                            Timestamp receiveTime)> MessageCallback;

}  // namespace net
}  // namespace tinyWeb