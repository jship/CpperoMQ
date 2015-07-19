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
    bool send(Sendable& sendable, SendableTypes&... sendables);

protected:
    SendingSocket(void* context, int type);

private:
    // Terminating function for variadic member template.
    bool send() { return true; }
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
bool SendingSocket<S>::send(Sendable& sendable, SendableTypes&... sendables)
{
    if (!sendable.send(*this, (sizeof...(sendables) > 0)))
    {
        return false;
    }

    return (send(sendables...));
}

template <typename S>
inline
SendingSocket<S>::SendingSocket(void* context, int type)
    : S(context, type)
{
}

}
}
