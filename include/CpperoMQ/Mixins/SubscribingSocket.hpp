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

namespace CpperoMQ
{
namespace Mixins
{

template <typename S>
class SubscribingSocket : public S
{
public:
    SubscribingSocket() = delete;
    virtual ~SubscribingSocket() = default;
    SubscribingSocket(const SubscribingSocket& other) = delete;
    SubscribingSocket(SubscribingSocket&& other);
    SubscribingSocket& operator=(SubscribingSocket& other) = delete;
    SubscribingSocket& operator=(SubscribingSocket&& other);

    auto subscribe(const char* buffer = "")           -> void;
    auto subscribe(size_t length, const char* buffer) -> void;

    auto unsubscribe(const char* buffer)                -> void;
    auto unsubscribe(size_t length, const char* buffer) -> void;

protected:
    SubscribingSocket(void* context, int type);
};

template <typename S>
inline
SubscribingSocket<S>::SubscribingSocket(SubscribingSocket<S>&& other)
    : S(std::move(other))
{
}

template <typename S>
inline
SubscribingSocket<S>& SubscribingSocket<S>::operator=(SubscribingSocket<S>&& other)
{
    S::operator=(std::move(other));
    return (*this);
}

template <typename S>
inline
auto SubscribingSocket<S>::subscribe(const char* buffer) -> void
{
    CPPEROMQ_ASSERT(buffer != nullptr);
    subscribe(std::strlen(buffer), buffer);
}

template <typename S>
inline
auto SubscribingSocket<S>::subscribe(size_t length, const char* buffer) -> void
{
    CPPEROMQ_ASSERT(buffer != nullptr);
    S::setSocketOption(ZMQ_SUBSCRIBE, buffer, length);
}

template <typename S>
inline
auto SubscribingSocket<S>::unsubscribe(const char* buffer) -> void
{
    CPPEROMQ_ASSERT(buffer != nullptr);
    unsubscribe(std::strlen(buffer), buffer);
}

template <typename S>
inline
auto SubscribingSocket<S>::unsubscribe( size_t length
                                      , const char* buffer ) -> void
{
    CPPEROMQ_ASSERT(buffer != nullptr);
    S::setSocketOption(ZMQ_UNSUBSCRIBE, buffer, length);
}

template <typename S>
inline
SubscribingSocket<S>::SubscribingSocket(void* context, int type)
    : S(context, type)
{
}

}
}
