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
#include <cstring>

namespace CpperoMQ
{
namespace Mixins
{

template <typename S>
class IdentifyingSocket : public S
{
public:
    IdentifyingSocket() = delete;
    virtual ~IdentifyingSocket() = default;
    IdentifyingSocket(const IdentifyingSocket& other) = delete;
    IdentifyingSocket(IdentifyingSocket&& other);
    IdentifyingSocket& operator=(IdentifyingSocket& other) = delete;
    IdentifyingSocket& operator=(IdentifyingSocket&& other);

    auto setIdentity(const char* buffer = "")           -> void;
    auto setIdentity(size_t length, const char* buffer) -> void;

    auto getIdentity(size_t length, const char* buffer) -> void;

protected:
    IdentifyingSocket(void* context, int type);
};

template <typename S>
inline
IdentifyingSocket<S>::IdentifyingSocket(IdentifyingSocket<S>&& other)
    : S(std::move(other))
{
}

template <typename S>
inline
IdentifyingSocket<S>& IdentifyingSocket<S>::operator=(IdentifyingSocket<S>&& other)
{
    S::operator=(std::move(other));
    return (*this);
}

template <typename S>
inline
auto IdentifyingSocket<S>::setIdentity(const char* buffer) -> void
{
    CPPEROMQ_ASSERT(buffer != nullptr);
    setIdentity(std::strlen(buffer), buffer);
}

template <typename S>
inline
auto IdentifyingSocket<S>::setIdentity(size_t length, const char* buffer) -> void
{
    CPPEROMQ_ASSERT(buffer != nullptr);
    S::setSocketOption(ZMQ_IDENTITY, buffer, length);
}

template <typename S>
inline
auto IdentifyingSocket<S>::getIdentity( size_t length
                                      , const char* buffer ) -> void
{
    CPPEROMQ_ASSERT(buffer != nullptr);
    return (S::getSocketOption(ZMQ_IDENTITY, buffer, length));
}

template <typename S>
inline
IdentifyingSocket<S>::IdentifyingSocket(void* context, int type)
    : S(context, type)
{
}

}
}
