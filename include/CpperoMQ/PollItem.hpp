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
#include <CpperoMQ/PublishSocket.hpp>
#include <CpperoMQ/PullSocket.hpp>
#include <CpperoMQ/PushSocket.hpp>
#include <CpperoMQ/ReplySocket.hpp>
#include <CpperoMQ/RequestSocket.hpp>
#include <CpperoMQ/RouterSocket.hpp>
#include <CpperoMQ/SubscribeSocket.hpp>

#include <functional>
#include <type_traits>

namespace CpperoMQ
{

template <typename S>
class PollItem
{
    static_assert( std::is_base_of<Socket, S>::value
                 , "Socket must be base of template parameter 'S'." );

public:
    using Callback = std::function<void(void)>;

    virtual ~PollItem() = default;
    PollItem(const PollItem& other) = delete;
    PollItem(PollItem&& other);
    PollItem& operator=(const PollItem& other) = delete;
    PollItem& operator=(PollItem&& other) = delete;
    
    int getEvents() const;

    const S* getSocket() const;
    S* getSocket();

    const Callback getCallback() const;
    Callback getCallback();

protected:
    PollItem(int events, S* socket, Callback callable);

private:
    int mEvents;
    S* mSocketPtr;
    Callback mCallable;
};

template <typename S>
inline
PollItem<S>::PollItem(PollItem&& other)
    : PollItem(0, nullptr, Callback())
{
    using std::swap;
    swap(mEvents,    other.mEvents);
    swap(mSocketPtr, other.mSocketPtr);
    swap(mCallable,  other.mCallable);
}

template <typename S>
inline
PollItem<S>::PollItem(int events, S* socket, Callback callable)
    : mEvents(events)
    , mSocketPtr(socket)
    , mCallable(callable)
{
}

template <typename S>
inline
int PollItem<S>::getEvents() const
{
    return mEvents;
}

template <typename S>
inline
const S* PollItem<S>::getSocket() const
{
    return mSocketPtr;
}

template <typename S>
inline
S* PollItem<S>::getSocket()
{
    return mSocketPtr;
}

template <typename S>
inline
const typename PollItem<S>::Callback PollItem<S>::getCallback() const
{
    return mCallable;
}

template <typename S>
inline
typename PollItem<S>::Callback PollItem<S>::getCallback()
{
    return mCallable;
}

template <typename S>
class IsReceiveReady : public PollItem<S>
{
    // This is ugly, but mixins make it tough to use std::is_base_of.
    static_assert( std::is_same<DealerSocket,    S>::value ||
                   std::is_same<PullSocket,      S>::value ||
                   std::is_same<ReplySocket,     S>::value ||
                   std::is_same<RequestSocket,   S>::value ||
                   std::is_same<RouterSocket,    S>::value ||
                   std::is_same<SubscribeSocket, S>::value
                 , "Template parameter 'S' must inherit ReceivingSocket mixin." );

public:
    IsReceiveReady(S& socket, Callback callable);
    virtual ~IsReceiveReady() = default;
    IsReceiveReady(const IsReceiveReady& other) = delete;
    IsReceiveReady(IsReceiveReady&& other);
    IsReceiveReady& operator=(const IsReceiveReady& other) = delete;
    IsReceiveReady& operator=(IsReceiveReady&& other) = delete;
};

template <typename S>
inline
IsReceiveReady<S>::IsReceiveReady(S& socket, Callback callable)
    : PollItem(ZMQ_POLLIN, &socket, callable)
{
}

template <typename S>
inline
IsReceiveReady<S>::IsReceiveReady(IsReceiveReady&& other)
    : PollItem<S>(std::move(other))
{
}

template <typename S>
class IsSendReady : public PollItem<S>
{
    // This is ugly, but mixins make it tough to use std::is_base_of.
    static_assert( std::is_same<DealerSocket,    S>::value ||
                   std::is_same<PublishSocket,   S>::value ||
                   std::is_same<PushSocket,      S>::value ||
                   std::is_same<ReplySocket,     S>::value ||
                   std::is_same<RequestSocket,   S>::value ||
                   std::is_same<RouterSocket,    S>::value
                 , "Template parameter 'S' must inherit SendingSocket mixin." );

public:
    IsSendReady(S& socket, Callback callable);
    virtual ~IsSendReady() = default;
    IsSendReady(const IsSendReady& other) = delete;
    IsSendReady(IsSendReady&& other);
    IsSendReady& operator=(const IsSendReady& other) = delete;
    IsSendReady& operator=(IsSendReady&& other) = delete;
};

template <typename S>
inline
IsSendReady<S>::IsSendReady(S& socket, Callback callable)
    : PollItem(ZMQ_POLLOUT, &socket, callable)
{
}

template <typename S>
inline
IsSendReady<S>::IsSendReady(IsSendReady&& other)
    : PollItem<S>(std::move(other))
{
}

template <typename S>
class IsSendOrReceiveReady : public PollItem<S>
{
    // This is ugly, but mixins make it tough to use std::is_base_of.
    static_assert( std::is_same<DealerSocket,  S>::value ||
                   std::is_same<ReplySocket,   S>::value ||
                   std::is_same<RequestSocket, S>::value ||
                   std::is_same<RouterSocket,  S>::value
                 , "Template parameter 'S' must inherit ReceivingSocket and SendingSocket mixins." );

public:
    IsSendOrReceiveReady(S& socket, Callback callable);
    virtual ~IsSendOrReceiveReady() = default;
    IsSendOrReceiveReady(const IsSendOrReceiveReady& other) = delete;
    IsSendOrReceiveReady(IsSendOrReceiveReady&& other);
    IsSendOrReceiveReady& operator=(const IsSendOrReceiveReady& other) = delete;
    IsSendOrReceiveReady& operator=(IsSendOrReceiveReady&& other) = delete;
};

template <typename S>
inline
IsSendOrReceiveReady<S>::IsSendOrReceiveReady(S& socket, Callback callable)
    : PollItem(ZMQ_POLLIN | ZMQ_POLLOUT, &socket, callable)
{
}

template <typename S>
inline
IsSendOrReceiveReady<S>::IsSendOrReceiveReady(IsSendOrReceiveReady&& other)
    : PollItem<S>(std::move(other))
{
}

template <typename S>
inline
IsReceiveReady<S> isReceiveReady(S& socket, typename PollItem<S>::Callback callable)
{
    IsReceiveReady<S> receiveReady(socket, callable);
    return (receiveReady);
}

template <typename S>
inline
IsSendReady<S> isSendReady(S& socket, typename PollItem<S>::Callback callable)
{
    IsSendReady<S> sendReady(socket, callable);
    return (sendReady);
}

template <typename S>
inline
IsSendOrReceiveReady<S> isSendOrReceiveReady(S& socket, typename PollItem<S>::Callback callable)
{
    IsSendOrReceiveReady<S> sendOrReceiveReady(socket, callable);
    return (sendOrReceiveReady);
}

}
