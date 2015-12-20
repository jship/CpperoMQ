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

#include <CpperoMQ/PollItem.hpp>

#include <array>

namespace CpperoMQ
{

class Poller
{
public:
    Poller(const long timeout = -1);

    auto getTimeout() const -> long;
    auto setTimeout(const long timeout) -> void;

    template <typename... PollItemTypes>
    auto poll(PollItem& pollItem, PollItemTypes&... pollItems) -> void;

private:
    template <size_t N, typename... PollItemTypes>
    auto poll( std::array<zmq_pollitem_t, N>& pollItemArray
             , std::array<PollItem::Callback, N>& callbackArray
             , PollItem& pollItem
             , PollItemTypes&... pollItems ) -> void;
    
    template <size_t N>
    auto poll( std::array<zmq_pollitem_t, N>& pollItemArray
             , std::array<PollItem::Callback, N>& callbackArray ) -> void;

    long mTimeout;
};

inline
Poller::Poller(const long timeout)
    : mTimeout(timeout)
{
}

inline
auto Poller::getTimeout() const -> long
{
    return mTimeout;
}

inline
auto Poller::setTimeout(const long timeout) -> void
{
    mTimeout = timeout;
}

template <typename... PollItemTypes>
inline
auto Poller::poll(PollItem& pollItem, PollItemTypes&... pollItems) -> void
{
    std::array<zmq_pollitem_t,     1 + sizeof...(pollItems)> pollItemArray;
    std::array<PollItem::Callback, 1 + sizeof...(pollItems)> callbackArray;

    poll(pollItemArray, callbackArray, pollItem, pollItems...);
}

template <size_t N, typename... PollItemTypes>
inline
auto Poller::poll( std::array<zmq_pollitem_t, N>& pollItemArray
                 , std::array<PollItem::Callback, N>& callbackArray
                 , PollItem& pollItem
                 , PollItemTypes&... pollItems ) -> void
{
    CPPEROMQ_ASSERT(nullptr != pollItem.getRawSocket());

    zmq_pollitem_t internalPollItem;
    internalPollItem.socket  = static_cast<void*>(pollItem.getRawSocket());
    internalPollItem.fd      = 0;
    internalPollItem.events  = pollItem.getEvents();
    internalPollItem.revents = 0;

    std::get<sizeof...(pollItems)>(pollItemArray) = internalPollItem;
    std::get<sizeof...(pollItems)>(callbackArray) = pollItem.getCallback();

    poll(pollItemArray, callbackArray, pollItems...);
}

template <size_t N>
inline
auto Poller::poll( std::array<zmq_pollitem_t, N>& pollItemArray
                 , std::array<PollItem::Callback, N>& callbackArray ) -> void
{
    if (zmq_poll(pollItemArray.data(), N, mTimeout) < 0)
    {
        throw Error();
    }

    for (size_t i = 0; i < N; ++i)
    {
        if (callbackArray[i])
        {
            if (pollItemArray[i].revents & ZMQ_POLLIN)
            {
                callbackArray[i]();
            }
            else if (pollItemArray[i].revents & ZMQ_POLLOUT)
            {
                callbackArray[i]();
            }
        }
    }
}

}
