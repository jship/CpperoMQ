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
class RequestingSocket : public S
{
public:
    RequestingSocket() = delete;
    virtual ~RequestingSocket() = default;
    RequestingSocket(const RequestingSocket& other) = delete;
    RequestingSocket(RequestingSocket&& other);
    RequestingSocket& operator=(RequestingSocket& other) = delete;
    RequestingSocket& operator=(RequestingSocket&& other);

    auto setRequestCorrelation(bool correlate) -> void;
    auto setRelaxed(bool relax)                -> void;

protected:
    RequestingSocket(void* context, int type);
};

template <typename S>
inline
RequestingSocket<S>::RequestingSocket(RequestingSocket<S>&& other)
    : S(std::move(other))
{
}

template <typename S>
inline
RequestingSocket<S>& RequestingSocket<S>::operator=(RequestingSocket<S>&& other)
{
    S::operator=(std::move(other));
    return (*this);
}
    
template <typename S>
inline
auto RequestingSocket<S>::setRequestCorrelation(bool correlate) -> void
{
    S::template setSocketOption(ZMQ_REQ_CORRELATE, (correlate) ? 1 : 0);
}

template <typename S>
inline
auto RequestingSocket<S>::setRelaxed(bool relax) -> void
{
    S::template setSocketOption(ZMQ_REQ_RELAXED, (relax) ? 1 : 0);
}

template <typename S>
inline
RequestingSocket<S>::RequestingSocket(void* context, int type)
    : S(context, type)
{
}

}
}
