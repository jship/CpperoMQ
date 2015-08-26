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
#include <CpperoMQ/ExtendedPublishSocket.hpp>
#include <CpperoMQ/ExtendedSubscribeSocket.hpp>
#include <CpperoMQ/PullSocket.hpp>
#include <CpperoMQ/PushSocket.hpp>
#include <CpperoMQ/RouterSocket.hpp>

#include <type_traits>

namespace CpperoMQ
{

class Proxy
{
public:
    Proxy();
    ~Proxy() = default;
    Proxy(const Proxy& other) = delete;
    Proxy(Proxy&& other);
    Proxy& operator=(const Proxy& other) = delete;
    Proxy& operator=(Proxy&& other);
    
    friend auto swap(Proxy& lhs, Proxy& rhs) -> void;
    
    template <typename S1, typename S2>
    auto run(S1& frontend, S2& backend) -> bool;
    
    template <typename S>
    auto setCaptureSocket(S& socket) -> void;
    
    template <typename S>
    auto setControlSocket(S& socket) -> void;

private:
    void* mCaptureSocketPtr;
    void* mControlSocketPtr;
};

inline
Proxy::Proxy()
    : mCaptureSocketPtr(nullptr)
    , mControlSocketPtr(nullptr)
{
}

inline
Proxy::Proxy(Proxy&& other)
    : Proxy()
{
    swap(*this, other);
}

inline
Proxy& Proxy::operator=(Proxy&& other)
{
    swap(*this, other);
    return (*this);
}

template <typename S1, typename S2>
inline
auto Proxy::run(S1& frontend, S2& backend) -> bool
{
    static_assert( (std::is_same<RouterSocket, S1>::value && std::is_same<DealerSocket, S2>::value) ||
                   (std::is_same<DealerSocket, S1>::value && std::is_same<RouterSocket, S2>::value) ||

                   (std::is_same<ExtendedSubscribeSocket, S1>::value && std::is_same<ExtendedPublishSocket,   S2>::value) ||
                   (std::is_same<ExtendedPublishSocket,   S1>::value && std::is_same<ExtendedSubscribeSocket, S2>::value) ||
    
                   (std::is_same<PullSocket, S1>::value && std::is_same<PushSocket, S2>::value) ||
                   (std::is_same<PushSocket, S1>::value && std::is_same<PullSocket, S2>::value)

                 , "Template parameters 'S1' and 'S2' must be Router/Dealer, ExtendedSubscribe/ExtendedPublish, "
                   "or Pull/Push." );

    int result = zmq_proxy_steerable(
        static_cast<void*>(frontend),
        static_cast<void*>(backend),
        mCaptureSocketPtr,
        mControlSocketPtr
    );

    return (mControlSocketPtr) ? (result == 0) : true;
}

template <typename S>
inline
auto Proxy::setCaptureSocket(S& socket) -> void
{
    static_assert( std::is_same<DealerSocket,  S>::value ||
                   std::is_same<PublishSocket, S>::value ||
                   std::is_same<PushSocket,    S>::value
                 , "Template parameter 'S' must be DealerSocket, "
                   "PublishSocket, or PushSocket." );

    mCaptureSocketPtr = static_cast<void*>(socket);
}

template <typename S>
inline
auto Proxy::setControlSocket(S& socket) -> void
{
    static_assert( std::is_same<PullSocket,      S>::value ||
                   std::is_same<SubscribeSocket, S>::value
                 , "Template parameter 'S' must be PullSocket or "
                   "SubscribeSocket." );

    mControlSocketPtr = static_cast<void*>(socket);
}

inline
auto swap(Proxy& lhs, Proxy& rhs) -> void
{
    using std::swap;
    swap(lhs.mCaptureSocketPtr, rhs.mCaptureSocketPtr);
    swap(lhs.mControlSocketPtr, rhs.mControlSocketPtr);
}

}
