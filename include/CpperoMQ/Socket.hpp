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
    Socket(Socket&& other);
    Socket& operator=(Socket& other) = delete;
    Socket& operator=(Socket&& other);

    void bind(const char* address);
    void unbind(const char* address);

    void connect(const char* address);
    void disconnect(const char* address);
    
    auto getBacklog() const                   -> int;
    auto getHandshakeInterval() const         -> int;
    auto getImmediate() const                 -> bool;
    auto getIoThreadAffinity() const          -> uint64_t;
    auto getIPv6() const                      -> bool;
    auto getMaxReconnectInterval() const      -> int;
    auto getMulticastRate() const             -> int;
    auto getMulticastRecoveryInterval() const -> int;
    auto getReconnectInterval() const         -> int;

    void setBacklog(const int backlog);
    void setConflate(const bool conflate);
    void setHandshakeInterval(const int milliseconds);
    void setImmediate(const bool immediate);
    void setIoThreadAffinity(const uint64_t affinity);
    void setIPv6(const bool ipv6);
    void setMaxReconnectInterval(const int milliseconds);
    void setMulticastRate(const int kbps);
    void setMulticastRecoveryInterval(const int milliseconds);
    void setReconnectInterval(const int milliseconds);
    
protected:
    Socket(void* context, int type);

    template <typename T>
    auto getSocketOption(const int option) const -> T;
    void getSocketOption( const int option
                        , void* value
                        , size_t* valueLength ) const;

    template <typename T>
    void setSocketOption(const int option, const T value);
    void setSocketOption( const int option
                        , const void* value
                        , const size_t valueLength );

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
void Socket::bind(const char* address)
{
    if (0 != zmq_bind(mSocket, address))
    {
        throw Error();
    }
}

inline
void Socket::unbind(const char* address)
{
    if (0 != zmq_unbind(mSocket, address))
    {
        throw Error();
    }
}

inline
void Socket::connect(const char* address)
{
    if (0 != zmq_connect(mSocket, address))
    {
        throw Error();
    }
}

inline
void Socket::disconnect(const char* address)
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
void Socket::setBacklog(const int backlog)
{
    setSocketOption(ZMQ_BACKLOG, backlog);
}

inline
void Socket::setConflate(const bool conflate)
{
    setSocketOption(ZMQ_CONFLATE, (conflate) ? 1 : 0);
}

inline 
void Socket::setHandshakeInterval(const int milliseconds)
{
    setSocketOption(ZMQ_HANDSHAKE_IVL, milliseconds);
}

inline
void Socket::setImmediate(const bool immediate)
{
    setSocketOption(ZMQ_IMMEDIATE, (immediate) ? 1 : 0);
}

inline
void Socket::setIoThreadAffinity(const uint64_t affinity)
{
    setSocketOption(ZMQ_AFFINITY, affinity);
}

inline
void Socket::setIPv6(const bool ipv6)
{
    setSocketOption(ZMQ_IPV6, (ipv6) ? 1 : 0);
}

inline
void Socket::setMulticastRate(const int kbps)
{
    setSocketOption(ZMQ_RATE, kbps);
}

inline
void Socket::setMulticastRecoveryInterval(const int milliseconds)
{
    setSocketOption(ZMQ_RECOVERY_IVL, milliseconds);
}

inline
void Socket::setMaxReconnectInterval(const int milliseconds)
{
    setSocketOption(ZMQ_RECONNECT_IVL_MAX, milliseconds);
}

inline
void Socket::setReconnectInterval(const int milliseconds)
{
    setSocketOption(ZMQ_RECONNECT_IVL, milliseconds);
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
void Socket::getSocketOption( const int option
                            , void* value
                            , size_t* valueLength ) const
{
    if (0 != zmq_getsockopt(mSocket, option, value, valueLength))
    {
        throw Error();
    }
}

template <typename T>
inline
void Socket::setSocketOption(const int option, const T value)
{
    setSocketOption(option, &value, sizeof(value));
}

inline
void Socket::setSocketOption( const int option
                            , const void* value
                            , const size_t valueLength )
{
    if (0 != zmq_setsockopt(mSocket, option, value, valueLength))
    {
        throw Error();
    }
}

}
