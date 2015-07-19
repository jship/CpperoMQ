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

    DealerSocket createDealerSocket();
    PublishSocket createPublishSocket();
    PullSocket createPullSocket();
    PushSocket createPushSocket();
    ReplySocket createReplySocket();
    RequestSocket createRequestSocket();
    RouterSocket createRouterSocket();
    SubscribeSocket createSubscribeSocket();

    int getIoThreadCount() const;
    int getMaxSocketCount() const;
    int getMaxConfigurableSocketCount() const;

    bool isIPv6Enabled() const;
    bool isBlocky() const;

    void setIPv6Enabled(bool enabled);
    void setBlocky(bool blocky);

private:
    int getContextSetting(const int settingName) const;
    void setContextSetting(int settingName, int settingValue);

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
DealerSocket Context::createDealerSocket()
{
    DealerSocket socket(mContext);
    return socket;
}

inline
PublishSocket Context::createPublishSocket()
{
    PublishSocket socket(mContext);
    return socket;
}

inline
PullSocket Context::createPullSocket()
{
    PullSocket socket(mContext);
    return socket;
}

inline
PushSocket Context::createPushSocket()
{
    PushSocket socket(mContext);
    return socket;
}

inline
ReplySocket Context::createReplySocket()
{
    ReplySocket socket(mContext);
    return socket;
}

inline
RequestSocket Context::createRequestSocket()
{
    RequestSocket socket(mContext);
    return socket;
}

inline
RouterSocket Context::createRouterSocket()
{
    RouterSocket socket(mContext);
    return socket;
}

inline
SubscribeSocket Context::createSubscribeSocket()
{
    SubscribeSocket socket(mContext);
    return socket;
}

inline
int Context::getIoThreadCount() const
{
    return (getContextSetting(ZMQ_IO_THREADS));
}

inline
int Context::getMaxSocketCount() const
{
    return (getContextSetting(ZMQ_MAX_SOCKETS));
}

inline
int Context::getMaxConfigurableSocketCount() const
{
    return (getContextSetting(ZMQ_SOCKET_LIMIT));
}

inline
bool Context::isIPv6Enabled() const
{
    return (1 == getContextSetting(ZMQ_IPV6));
}

inline
bool Context::isBlocky() const
{
    return (1 == getContextSetting(ZMQ_BLOCKY));
}

inline
void Context::setIPv6Enabled(bool enabled)
{
    setContextSetting(ZMQ_IPV6, enabled ? 1 : 0);
}

inline
void Context::setBlocky(bool blocky)
{
    setContextSetting(ZMQ_BLOCKY, blocky ? 1 : 0);
}

inline
int Context::getContextSetting(const int settingName) const
{
    int result = zmq_ctx_get(mContext, settingName);

    if (result < 0)
    {
        throw Error();
    }

    return result;
}

inline
void Context::setContextSetting(int settingName, int settingValue)
{
    if (0 != zmq_ctx_set(mContext, settingName, settingValue))
    {
        throw Error();
    }
}

}
