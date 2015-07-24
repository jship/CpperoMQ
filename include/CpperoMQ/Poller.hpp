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

#include <algorithm>
#include <array>

namespace CpperoMQ
{

class Poller
{
public:
    Poller(const long timeout);

    long getTimeout() const;
    void setTimeout(const long timeout);

    template <typename PollItemType, typename... PollItemTypes>
    auto poll(PollItemType& pollItem, PollItemTypes&... pollItems) -> void;

private:
    template <size_t N, typename PollItemType, typename... PollItemTypes>
    auto poll( std::array<zmq_pollitem_t, N>& pollItemArray
             , std::array<std::function<void(void)>, N>& callbackArray
             , PollItemType& pollItem
             , PollItemTypes&... pollItems ) -> void;
    
    template <size_t N>
    auto poll( std::array<zmq_pollitem_t, N>& pollItemArray
             , std::array<std::function<void(void)>, N>& callbackArray ) -> void;

    long mTimeout;
};

Poller::Poller(const long timeout)
    : mTimeout(timeout)
{
}

long Poller::getTimeout() const
{
    return mTimeout;
}

void Poller::setTimeout(const long timeout)
{
    mTimeout = timeout;
}

template <typename PollItemType, typename... PollItemTypes>
inline
auto Poller::poll(PollItemType& pollItem, PollItemTypes&... pollItems) -> void
{
    std::array<zmq_pollitem_t,            1 + sizeof...(pollItems)> pollItemArray;
    std::array<std::function<void(void)>, 1 + sizeof...(pollItems)> callbackArray;

    poll(pollItemArray, callbackArray, pollItem, pollItems...);
}

template <size_t N, typename PollItemType, typename... PollItemTypes>
auto Poller::poll( std::array<zmq_pollitem_t, N>& pollItemArray
                 , std::array<std::function<void(void)>, N>& callbackArray
                 , PollItemType& pollItem
                 , PollItemTypes&... pollItems ) -> void
{
    zmq_pollitem_t internalPollItem;
    internalPollItem.socket  = static_cast<void*>(pollItem.getSocket());
    internalPollItem.fd      = 0;
    internalPollItem.events  = pollItem.getEvents();
    internalPollItem.revents = 0;

    std::get<sizeof...(pollItems)>(pollItemArray) = internalPollItem;
    std::get<sizeof...(pollItems)>(callbackArray) = pollItem.getCallback();

    poll(pollItemArray, callbackArray, pollItems...);
}

template <size_t N>
auto Poller::poll( std::array<zmq_pollitem_t, N>& pollItemArray
                 , std::array<std::function<void(void)>, N>& callbackArray ) -> void
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
