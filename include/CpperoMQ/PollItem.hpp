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

class PollItem
{
public:
    using Callback = std::function<void(void)>;

    virtual ~PollItem() = default;
    PollItem(const PollItem& other) = delete;
    PollItem(PollItem&& other);
    PollItem& operator=(const PollItem& other) = delete;
    PollItem& operator=(PollItem&& other) = delete;
    
    auto getEvents() const -> int;

    auto getRawSocket() const -> const void*;
    auto getRawSocket()       ->       void*;

    auto getCallback() -> Callback;

protected:
    PollItem(int events, Socket& socket, Callback callable = Callback());

private:
    PollItem(int events, Socket* socket, Callback callable = Callback());

    int mEvents;
    Socket* mSocketPtr;
    Callback mCallable;
};

inline
PollItem::PollItem(PollItem&& other)
    : PollItem(0, nullptr, Callback())
{
    using std::swap;
    swap(mEvents,    other.mEvents);
    swap(mSocketPtr, other.mSocketPtr);
    swap(mCallable,  other.mCallable);
}

inline
PollItem::PollItem(int events, Socket& socket, Callback callable)
    : PollItem(events, &socket, callable)
{
}

inline
PollItem::PollItem(int events, Socket* socket, Callback callable)
    : mEvents(events)
    , mSocketPtr(socket)
    , mCallable(callable)
{
}

inline
auto PollItem::getEvents() const -> int
{
    return mEvents;
}

inline
auto PollItem::getRawSocket() const -> const void*
{
    return (mSocketPtr) ? static_cast<void*>(*mSocketPtr) : nullptr;
}

inline
auto PollItem::getRawSocket() -> void*
{
    return (mSocketPtr) ? static_cast<void*>(*mSocketPtr) : nullptr;
}

inline
auto PollItem::getCallback() -> PollItem::Callback
{
    return mCallable;
}

template <typename S>
class IsReceiveReady : public PollItem
{
    // This is ugly, but mixins make it tough to use std::is_base_of.
    static_assert( std::is_same<DealerSocket,            S>::value ||
                   std::is_same<ExtendedSubscribeSocket, S>::value ||
                   std::is_same<PullSocket,              S>::value ||
                   std::is_same<ReplySocket,             S>::value ||
                   std::is_same<RequestSocket,           S>::value ||
                   std::is_same<RouterSocket,            S>::value ||
                   std::is_same<SubscribeSocket,         S>::value
                 , "Template parameter 'S' must inherit ReceivingSocket mixin." );

public:
    IsReceiveReady(S& socket, Callback callable = Callback());
    virtual ~IsReceiveReady() = default;
    IsReceiveReady(const IsReceiveReady& other) = delete;
    IsReceiveReady(IsReceiveReady&& other);
    IsReceiveReady& operator=(const IsReceiveReady& other) = delete;
    IsReceiveReady& operator=(IsReceiveReady&& other) = delete;
};

template <typename S>
inline
IsReceiveReady<S>::IsReceiveReady(S& socket, Callback callable)
    : PollItem(ZMQ_POLLIN, socket, callable)
{
}

template <typename S>
inline
IsReceiveReady<S>::IsReceiveReady(IsReceiveReady&& other)
    : PollItem(std::move(other))
{
}

template <typename S>
class IsSendReady : public PollItem
{
    // This is ugly, but mixins make it tough to use std::is_base_of.
    static_assert( std::is_same<DealerSocket,          S>::value ||
                   std::is_same<ExtendedPublishSocket, S>::value ||
                   std::is_same<PublishSocket,         S>::value ||
                   std::is_same<PushSocket,            S>::value ||
                   std::is_same<ReplySocket,           S>::value ||
                   std::is_same<RequestSocket,         S>::value ||
                   std::is_same<RouterSocket,          S>::value
                 , "Template parameter 'S' must inherit SendingSocket mixin." );

public:
    IsSendReady(S& socket, Callback callable = Callback());
    virtual ~IsSendReady() = default;
    IsSendReady(const IsSendReady& other) = delete;
    IsSendReady(IsSendReady&& other);
    IsSendReady& operator=(const IsSendReady& other) = delete;
    IsSendReady& operator=(IsSendReady&& other) = delete;
};

template <typename S>
inline
IsSendReady<S>::IsSendReady(S& socket, Callback callable)
    : PollItem(ZMQ_POLLOUT, socket, callable)
{
}

template <typename S>
inline
IsSendReady<S>::IsSendReady(IsSendReady&& other)
    : PollItem(std::move(other))
{
}

template <typename S>
class IsSendOrReceiveReady : public PollItem
{
    // This is ugly, but mixins make it tough to use std::is_base_of.
    static_assert( std::is_same<DealerSocket,  S>::value ||
                   std::is_same<ReplySocket,   S>::value ||
                   std::is_same<RequestSocket, S>::value ||
                   std::is_same<RouterSocket,  S>::value
                 , "Template parameter 'S' must inherit ReceivingSocket and SendingSocket mixins." );

public:
    IsSendOrReceiveReady(S& socket, Callback callable = Callback());
    virtual ~IsSendOrReceiveReady() = default;
    IsSendOrReceiveReady(const IsSendOrReceiveReady& other) = delete;
    IsSendOrReceiveReady(IsSendOrReceiveReady&& other);
    IsSendOrReceiveReady& operator=(const IsSendOrReceiveReady& other) = delete;
    IsSendOrReceiveReady& operator=(IsSendOrReceiveReady&& other) = delete;
};

template <typename S>
inline
IsSendOrReceiveReady<S>::IsSendOrReceiveReady(S& socket, Callback callable)
    : PollItem(ZMQ_POLLIN | ZMQ_POLLOUT, socket, callable)
{
}

template <typename S>
inline
IsSendOrReceiveReady<S>::IsSendOrReceiveReady(IsSendOrReceiveReady&& other)
    : PollItem(std::move(other))
{
}

template <typename S>
inline
auto isReceiveReady(S& socket, PollItem::Callback callable = PollItem::Callback()) -> IsReceiveReady<S>
{
    IsReceiveReady<S> receiveReady(socket, callable);
    return (receiveReady);
}

template <typename S>
inline
auto isSendReady(S& socket, PollItem::Callback callable = PollItem::Callback()) -> IsSendReady<S>
{
    IsSendReady<S> sendReady(socket, callable);
    return (sendReady);
}

template <typename S>
inline
auto isSendOrReceiveReady(S& socket, PollItem::Callback callable = PollItem::Callback()) -> IsSendOrReceiveReady<S>
{
    IsSendOrReceiveReady<S> sendOrReceiveReady(socket, callable);
    return (sendOrReceiveReady);
}

}
