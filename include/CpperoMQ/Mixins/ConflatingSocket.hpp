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
class ConflatingSocket : public S
{
public:
    ConflatingSocket() = delete;
    virtual ~ConflatingSocket() = default;
    ConflatingSocket(const ConflatingSocket& other) = delete;
    ConflatingSocket(ConflatingSocket&& other);
    ConflatingSocket& operator=(ConflatingSocket& other) = delete;
    ConflatingSocket& operator=(ConflatingSocket&& other);

    auto setConflate(const bool conflate) -> void;

protected:
    ConflatingSocket(void* context, int type);
};

template <typename S>
inline
ConflatingSocket<S>::ConflatingSocket(ConflatingSocket<S>&& other)
    : S(std::move(other))
{
}

template <typename S>
inline
ConflatingSocket<S>& ConflatingSocket<S>::operator=(ConflatingSocket<S>&& other)
{
    S::operator=(std::move(other));
    return (*this);
}

template <typename S>
inline
auto ConflatingSocket<S>::setConflate(bool conflate) -> void
{
    S::template setSocketOption(ZMQ_CONFLATE, (conflate) ? 1 : 0);
}


template <typename S>
inline
ConflatingSocket<S>::ConflatingSocket(void* context, int type)
    : S(context, type)
{
}

}
}
