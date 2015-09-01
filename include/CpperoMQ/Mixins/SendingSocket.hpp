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

#include <CpperoMQ/Sendable.hpp>

namespace CpperoMQ
{
namespace Mixins
{

template <typename S>
class SendingSocket : public S
{
public:
    SendingSocket() = delete;
    virtual ~SendingSocket() = default;
    SendingSocket(const SendingSocket& other) = delete;
    SendingSocket(SendingSocket&& other);
    SendingSocket& operator=(SendingSocket& other) = delete;
    SendingSocket& operator=(SendingSocket&& other);

    template <typename... SendableTypes>
    auto send(const Sendable& sendable, SendableTypes&&... sendables) const -> bool;

    auto getLingerPeriod() const      -> int;
    auto getMulticastHops() const     -> int;
    auto getSendBufferSize() const    -> int;
    auto getSendHighWaterMark() const -> int;
    auto getSendTimeout() const       -> int;

    auto setLingerPeriod(const int milliseconds) -> void;
    auto setMulticastHops(const int hops)        -> void;
    auto setSendBufferSize(const int size)       -> void;
    auto setSendHighWaterMark(const int hwm)     -> void;
    auto setSendTimeout(const int timeout)       -> void;

protected:
    SendingSocket(void* context, int type);

private:
    // Terminating function for variadic member template.
    auto send() const -> bool { return true; }
};

template <typename S>
inline
SendingSocket<S>::SendingSocket(SendingSocket<S>&& other)
    : S(std::move(other))
{
}

template <typename S>
inline
SendingSocket<S>& SendingSocket<S>::operator=(SendingSocket<S>&& other)
{
    S::operator=(std::move(other));
    return (*this);
}

template <typename S>
template <typename... SendableTypes>
inline
auto SendingSocket<S>::send( const Sendable& sendable
                           , SendableTypes&&... sendables ) const -> bool
{
    if (!sendable.send(*this, (sizeof...(sendables) > 0)))
    {
        return false;
    }

    return (send(sendables...));
}

template <typename S>
inline
auto SendingSocket<S>::getLingerPeriod() const -> int
{
    return (S::template getSocketOption<int>(ZMQ_LINGER));
}

template <typename S>
inline
auto SendingSocket<S>::getMulticastHops() const -> int
{
    return (S::template getSocketOption<int>(ZMQ_MULTICAST_HOPS));
}

template <typename S>
inline
auto SendingSocket<S>::getSendBufferSize() const -> int
{
    return (S::template getSocketOption<int>(ZMQ_SNDBUF));
}

template <typename S>
inline
auto SendingSocket<S>::getSendHighWaterMark() const -> int
{
    return (S::template getSocketOption<int>(ZMQ_SNDHWM));
}

template <typename S>
inline
auto SendingSocket<S>::getSendTimeout() const -> int
{
    return (S::template getSocketOption<int>(ZMQ_SNDTIMEO));
}

template <typename S>
inline
auto SendingSocket<S>::setLingerPeriod(const int milliseconds) -> void
{
    S::template setSocketOption(ZMQ_LINGER, milliseconds);
}

template <typename S>
inline
auto SendingSocket<S>::setMulticastHops(const int hops) -> void
{
    S::template setSocketOption(ZMQ_MULTICAST_HOPS, hops);
}

template <typename S>
inline
auto SendingSocket<S>::setSendBufferSize(const int size) -> void
{
    S::template setSocketOption(ZMQ_SNDBUF, size);
}

template <typename S>
inline
auto SendingSocket<S>::setSendHighWaterMark(const int hwm) -> void
{
    S::template setSocketOption(ZMQ_SNDHWM, hwm);
}

template <typename S>
inline
auto SendingSocket<S>::setSendTimeout(const int timeout) -> void
{
    S::template setSocketOption(ZMQ_SNDTIMEO, timeout);
}

template <typename S>
inline
SendingSocket<S>::SendingSocket(void* context, int type)
    : S(context, type)
{
}

}
}
