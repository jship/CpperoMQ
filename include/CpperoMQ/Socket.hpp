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

#include <CpperoMQ/Common.hpp>

#include <algorithm>
#include <cstring>

namespace CpperoMQ
{

class Socket
{
    friend class IncomingMessage;
    friend class OutgoingMessage;

public:
    Socket() = delete;
    virtual ~Socket();
    Socket(const Socket& other) = delete;
    Socket& operator=(Socket& other) = delete;

    auto bind(const char* address) -> void;
    auto unbind(const char* address) -> void;

    auto connect(const char* address) -> void;
    auto disconnect(const char* address) -> void;
    
    auto getBacklog() const                                 -> int;
    auto getHandshakeInterval() const                       -> int;
    auto getImmediate() const                               -> bool;
    auto getIoThreadAffinity() const                        -> uint64_t;
    auto getIPv6() const                                    -> bool;
    auto getLastEndpoint(size_t length, char* buffer) const -> void;
    auto getMaxReconnectInterval() const                    -> int;
    auto getMulticastRate() const                           -> int;
    auto getMulticastRecoveryInterval() const               -> int;
    auto getReconnectInterval() const                       -> int;

    auto setBacklog(const int backlog)                        -> void;
    auto setHandshakeInterval(const int milliseconds)         -> void;
    auto setImmediate(const bool immediate)                   -> void;
    auto setIoThreadAffinity(const uint64_t affinity)         -> void;
    auto setIPv6(const bool ipv6)                             -> void;
    auto setMaxReconnectInterval(const int milliseconds)      -> void;
    auto setMulticastRate(const int kbps)                     -> void;
    auto setMulticastRecoveryInterval(const int milliseconds) -> void;
    auto setReconnectInterval(const int milliseconds)         -> void;

    explicit operator void*();
    
protected:
    Socket(void* context, int type);
    Socket(Socket&& other);
    Socket& operator=(Socket&& other);

    template <typename T>
    auto getSocketOption(const int option) const -> T;
    auto getSocketOption( const int option
                        , void* value
                        , size_t* valueLength ) const -> void;

    template <typename T>
    auto setSocketOption(const int option, const T value) -> void;
    auto setSocketOption( const int option
                        , const void* value
                        , const size_t valueLength ) -> void;

private:
    void* mSocket;
};

inline
Socket::~Socket()
{
    if (mSocket != nullptr)
    {
        int result = zmq_close(mSocket);
        CPPEROMQ_ASSERT(result == 0);
        mSocket = 0 ;
    }
}

inline
auto Socket::bind(const char* address) -> void
{
    if (0 != zmq_bind(mSocket, address))
    {
        throw Error();
    }
}

inline
auto Socket::unbind(const char* address) -> void
{
    if (0 != zmq_unbind(mSocket, address))
    {
        throw Error();
    }
}

inline
auto Socket::connect(const char* address) -> void
{
    if (0 != zmq_connect(mSocket, address))
    {
        throw Error();
    }
}

inline
auto Socket::disconnect(const char* address) -> void
{
    if (0 != zmq_disconnect(mSocket, address))
    {
        throw Error();
    }
}

inline
Socket::Socket(void* context, int type)
    : mSocket(nullptr)
{
    CPPEROMQ_ASSERT(context != nullptr);

    mSocket = zmq_socket(context, type);
    if (mSocket == NULL)
    {
        throw Error();
    }
}

inline
Socket::Socket(Socket&& other)
    : mSocket(other.mSocket)
{
    other.mSocket = nullptr;
}

inline
Socket& Socket::operator=(Socket&& other)
{
    using std::swap;
    swap(mSocket, other.mSocket);
    return (*this);
}

inline
auto Socket::getBacklog() const -> int
{
    return (getSocketOption<int>(ZMQ_BACKLOG));
}

inline
auto Socket::getHandshakeInterval() const -> int
{
    return (getSocketOption<int>(ZMQ_HANDSHAKE_IVL));
}

inline
auto Socket::getImmediate() const -> bool
{
    return (getSocketOption<bool>(ZMQ_IMMEDIATE));
}

inline
auto Socket::getIoThreadAffinity() const -> uint64_t
{
    return (getSocketOption<uint64_t>(ZMQ_AFFINITY));
}

inline
auto Socket::getIPv6() const -> bool
{
    return (getSocketOption<bool>(ZMQ_IPV6));
}

inline
auto Socket::getLastEndpoint(size_t length, char* buffer) const -> void
{
    CPPEROMQ_ASSERT(buffer != nullptr);
    memset(buffer, 0, length);
    return (getSocketOption(ZMQ_LAST_ENDPOINT, buffer, &length));
}

inline
auto Socket::getMaxReconnectInterval() const -> int
{
    return (getSocketOption<int>(ZMQ_RECONNECT_IVL_MAX));
}

inline
auto Socket::getMulticastRate() const -> int
{
    return (getSocketOption<int>(ZMQ_RATE));
}

inline
auto Socket::getMulticastRecoveryInterval() const -> int
{
    return (getSocketOption<int>(ZMQ_RECOVERY_IVL));
}

inline
auto Socket::getReconnectInterval() const -> int
{
    return (getSocketOption<int>(ZMQ_RECONNECT_IVL));
}

inline
auto Socket::setBacklog(const int backlog) -> void
{
    setSocketOption(ZMQ_BACKLOG, backlog);
}

inline 
auto Socket::setHandshakeInterval(const int milliseconds) -> void
{
    setSocketOption(ZMQ_HANDSHAKE_IVL, milliseconds);
}

inline
auto Socket::setImmediate(const bool immediate) -> void
{
    setSocketOption(ZMQ_IMMEDIATE, (immediate) ? 1 : 0);
}

inline
auto Socket::setIoThreadAffinity(const uint64_t affinity) -> void
{
    setSocketOption(ZMQ_AFFINITY, affinity);
}

inline
auto Socket::setIPv6(const bool ipv6) -> void
{
    setSocketOption(ZMQ_IPV6, (ipv6) ? 1 : 0);
}

inline
auto Socket::setMulticastRate(const int kbps) -> void
{
    setSocketOption(ZMQ_RATE, kbps);
}

inline
auto Socket::setMulticastRecoveryInterval(const int milliseconds) -> void
{
    setSocketOption(ZMQ_RECOVERY_IVL, milliseconds);
}

inline
auto Socket::setMaxReconnectInterval(const int milliseconds) -> void
{
    setSocketOption(ZMQ_RECONNECT_IVL_MAX, milliseconds);
}

inline
auto Socket::setReconnectInterval(const int milliseconds) -> void
{
    setSocketOption(ZMQ_RECONNECT_IVL, milliseconds);
}

inline
Socket::operator void*()
{
    return mSocket;
}

template <typename T>
inline
auto Socket::getSocketOption(const int option) const -> T
{
    T value;
    size_t valueLength = sizeof(T);
    getSocketOption(option, &value, &valueLength);
    return value;
}

inline
auto Socket::getSocketOption( const int option
                            , void* value
                            , size_t* valueLength ) const -> void
{
    if (0 != zmq_getsockopt(mSocket, option, value, valueLength))
    {
        throw Error();
    }
}

template <typename T>
inline
auto Socket::setSocketOption(const int option, const T value) -> void
{
    setSocketOption(option, &value, sizeof(value));
}

inline
auto Socket::setSocketOption( const int option
                            , const void* value
                            , const size_t valueLength ) -> void
{
    if (0 != zmq_setsockopt(mSocket, option, value, valueLength))
    {
        throw Error();
    }
}

}
