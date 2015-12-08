// The MIT License (MIT)
//
// Copyright (c) 2015 Jason Shipman
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.

#pragma once

#include <CpperoMQ/DealerSocket.hpp>
#include <CpperoMQ/ExtendedPublishSocket.hpp>
#include <CpperoMQ/ExtendedSubscribeSocket.hpp>
#include <CpperoMQ/PublishSocket.hpp>
#include <CpperoMQ/PullSocket.hpp>
#include <CpperoMQ/PushSocket.hpp>
#include <CpperoMQ/ReplySocket.hpp>
#include <CpperoMQ/RequestSocket.hpp>
#include <CpperoMQ/RouterSocket.hpp>
#include <CpperoMQ/SubscribeSocket.hpp>

namespace CpperoMQ
{

class Context
{
public:
    Context(const int ioThreadCount = 1, const int maxSocketCount = 1023);
    ~Context();
    Context(const Context & rhs) = delete;
    Context(Context&& other);
    Context & operator=(const Context & rhs) = delete;
    Context& operator=(Context&& other);

    auto createDealerSocket()            -> DealerSocket;
    auto createExtendedPublishSocket()   -> ExtendedPublishSocket;
    auto createExtendedSubscribeSocket() -> ExtendedSubscribeSocket;
    auto createPublishSocket()           -> PublishSocket;
    auto createPullSocket()              -> PullSocket;
    auto createPushSocket()              -> PushSocket;
    auto createReplySocket()             -> ReplySocket;
    auto createRequestSocket()           -> RequestSocket;
    auto createRouterSocket()            -> RouterSocket;
    auto createSubscribeSocket()         -> SubscribeSocket;

    auto getIoThreadCount() const              -> int;
    auto getMaxSocketCount() const             -> int;
    auto getMaxConfigurableSocketCount() const -> int;

    auto isIPv6Enabled() const -> bool;

#if ZMQ_VERSION >= ZMQ_MAKE_VERSION(4, 2, 0)
    auto isBlocky() const -> bool;
#endif

    auto setIPv6Enabled(bool enabled) -> void;

#if ZMQ_VERSION >= ZMQ_MAKE_VERSION(4, 2, 0)
    auto setBlocky(bool blocky) -> void;
#endif

private:
    auto getContextSetting(const int settingName) const -> int;
    auto setContextSetting(int settingName, int settingValue) -> void;

    void* mContext;
};

inline
Context::Context(const int ioThreadCount, const int maxSocketCount)
    : mContext(zmq_ctx_new())
{
    if (nullptr == mContext)
    {
        throw Error();
    }

    setContextSetting(ZMQ_IO_THREADS, ioThreadCount);
    setContextSetting(ZMQ_MAX_SOCKETS, maxSocketCount);
}

inline
Context::~Context()
{
    if (mContext)
    {
        int result = zmq_ctx_term(mContext);
        CPPEROMQ_ASSERT(0 == result);
        mContext = nullptr;
    }
}

inline
Context::Context(Context&& other)
    : mContext(other.mContext)
{
    other.mContext = nullptr;
}

inline
Context& Context::operator=(Context&& other)
{
    using std::swap;
    swap(mContext, other.mContext);
    return (*this);
}

inline
auto Context::createDealerSocket() -> DealerSocket
{
    DealerSocket socket(mContext);
    return socket;
}

inline
auto Context::createExtendedPublishSocket() -> ExtendedPublishSocket
{
    ExtendedPublishSocket socket(mContext);
    return socket;
}

inline
auto Context::createExtendedSubscribeSocket() -> ExtendedSubscribeSocket
{
    ExtendedSubscribeSocket socket(mContext);
    return socket;
}

inline
auto Context::createPublishSocket() -> PublishSocket
{
    PublishSocket socket(mContext);
    return socket;
}

inline
auto Context::createPullSocket() -> PullSocket
{
    PullSocket socket(mContext);
    return socket;
}

inline
auto Context::createPushSocket() -> PushSocket
{
    PushSocket socket(mContext);
    return socket;
}

inline
auto Context::createReplySocket() -> ReplySocket
{
    ReplySocket socket(mContext);
    return socket;
}

inline
auto Context::createRequestSocket() -> RequestSocket
{
    RequestSocket socket(mContext);
    return socket;
}

inline
auto Context::createRouterSocket() -> RouterSocket
{
    RouterSocket socket(mContext);
    return socket;
}

inline
auto Context::createSubscribeSocket() -> SubscribeSocket
{
    SubscribeSocket socket(mContext);
    return socket;
}

inline
auto Context::getIoThreadCount() const -> int
{
    return (getContextSetting(ZMQ_IO_THREADS));
}

inline
auto Context::getMaxSocketCount() const -> int
{
    return (getContextSetting(ZMQ_MAX_SOCKETS));
}

inline
auto Context::getMaxConfigurableSocketCount() const -> int
{
    return (getContextSetting(ZMQ_SOCKET_LIMIT));
}

inline
auto Context::isIPv6Enabled() const -> bool
{
    return (1 == getContextSetting(ZMQ_IPV6));
}

#if ZMQ_VERSION >= ZMQ_MAKE_VERSION(4, 2, 0)
inline
auto Context::isBlocky() const -> bool
{
    return (1 == getContextSetting(ZMQ_BLOCKY));
}
#endif

inline
auto Context::setIPv6Enabled(bool enabled) -> void
{
    setContextSetting(ZMQ_IPV6, enabled ? 1 : 0);
}

#if ZMQ_VERSION >= ZMQ_MAKE_VERSION(4, 2, 0)
inline
auto Context::setBlocky(bool blocky) -> void
{
    setContextSetting(ZMQ_BLOCKY, blocky ? 1 : 0);
}
#endif

inline
auto Context::getContextSetting(const int settingName) const -> int
{
    int result = zmq_ctx_get(mContext, settingName);

    if (result < 0)
    {
        throw Error();
    }

    return result;
}

inline
auto Context::setContextSetting(int settingName, int settingValue) -> void
{
    if (0 != zmq_ctx_set(mContext, settingName, settingValue))
    {
        throw Error();
    }
}

}
