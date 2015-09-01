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

#include <CpperoMQ/Receivable.hpp>

namespace CpperoMQ
{
namespace Mixins
{

template <typename S>
class ReceivingSocket : public S
{
public:
    ReceivingSocket() = delete;
    virtual ~ReceivingSocket() = default;
    ReceivingSocket(const ReceivingSocket& other) = delete;
    ReceivingSocket(ReceivingSocket&& other);
    ReceivingSocket& operator=(ReceivingSocket& other) = delete;
    ReceivingSocket& operator=(ReceivingSocket&& other);

    template <typename... ReceivableTypes>
    auto receive( Receivable& receivable
                , ReceivableTypes&... receivables ) -> bool;

    auto getMaxInboundMessageSize() const -> int;
    auto getReceiveBufferSize() const     -> int;
    auto getReceiveHighWaterMark() const  -> int;
    auto getReceiveTimeout() const        -> int;

    auto setMaxInboundMessageSize(const int size) -> void;
    auto setReceiveBufferSize(const int size)     -> void;
    auto setReceiveHighWaterMark(const int hwm)   -> void;
    auto setReceiveTimeout(const int timeout)     -> void;

protected:
    ReceivingSocket(void* context, int type);

private:
    // Terminating function for variadic member template.
    auto receive() -> bool { return true; }
};

template <typename S>
inline
ReceivingSocket<S>::ReceivingSocket(ReceivingSocket<S>&& other)
    : S(std::move(other))
{
}

template <typename S>
inline
ReceivingSocket<S>& ReceivingSocket<S>::operator=(ReceivingSocket<S>&& other)
{
    S::operator=(std::move(other));
    return (*this);
}

template <typename S>
template <typename... ReceivableTypes>
inline
auto ReceivingSocket<S>::receive( Receivable& receivable
                                , ReceivableTypes&... receivables ) -> bool
{
    bool moreToReceive = false;
    if (!receivable.receive(*this, moreToReceive))
    {
        return false;
    }

    const size_t receivablesCount = sizeof...(receivables);

    if (!moreToReceive && (receivablesCount > 0))
    {
        return false;
    }

    if (moreToReceive && (receivablesCount == 0))
    {
        return false;
    }

    return (receive(receivables...));
}

template <typename S>
inline
auto ReceivingSocket<S>::getMaxInboundMessageSize() const -> int
{
    return (S::template getSocketOption<int>(ZMQ_MAXMSGSIZE));
}

template <typename S>
inline
auto ReceivingSocket<S>::getReceiveBufferSize() const -> int
{
    return (S::template getSocketOption<int>(ZMQ_RCVBUF));
}

template <typename S>
inline
auto ReceivingSocket<S>::getReceiveHighWaterMark() const -> int
{
    return (S::template getSocketOption<int>(ZMQ_RCVHWM));
}

template <typename S>
inline
auto ReceivingSocket<S>::getReceiveTimeout() const -> int
{
    return (S::template getSocketOption<int>(ZMQ_RCVTIMEO));
}

template <typename S>
inline
auto ReceivingSocket<S>::setMaxInboundMessageSize(const int size) -> void
{
    S::template setSocketOption(ZMQ_MAXMSGSIZE, size);
}

template <typename S>
inline
auto ReceivingSocket<S>::setReceiveBufferSize(const int size) -> void
{
    S::template setSocketOption(ZMQ_RCVBUF, size);
}

template <typename S>
inline
auto ReceivingSocket<S>::setReceiveHighWaterMark(const int hwm) -> void
{
    S::template setSocketOption(ZMQ_RCVHWM, hwm);
}

template <typename S>
inline
auto ReceivingSocket<S>::setReceiveTimeout(const int timeout) -> void
{
    S::template setSocketOption(ZMQ_RCVTIMEO, timeout);
}

template <typename S>
inline
ReceivingSocket<S>::ReceivingSocket(void* context, int type)
    : S(context, type)
{
}

}
}
