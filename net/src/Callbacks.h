#pragma once

#include <memory>
#include <functional>

namespace tinyWeb {
namespace net {

class TcpConnection;

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef std::function<void(const TcpConnectionPtr&)> ConnectionCallback;
typedef std::function<void(const TcpConnectionPtr&)> CloseCallback;
typedef std::function<void(const TcpConnectionPtr&)> ErrorCallback;
typedef std::function<void(const TcpConnectionPtr&, \
                            const char* buf, \
                            ssize_t len)> MessageCallback;

}  // namespace net
}  // namespace tinyWeb