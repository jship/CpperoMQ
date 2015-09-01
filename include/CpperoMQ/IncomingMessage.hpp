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

#include <CpperoMQ/Message.hpp>
#include <CpperoMQ/Receivable.hpp>
#include <CpperoMQ/Socket.hpp>

namespace CpperoMQ
{

class IncomingMessage final : public Message, public Receivable
{
public:
    IncomingMessage();
    virtual ~IncomingMessage() = default;
    IncomingMessage(const IncomingMessage& other) = delete;
    IncomingMessage(IncomingMessage&& other);
    IncomingMessage& operator=(const IncomingMessage& other) = delete;
    IncomingMessage& operator=(IncomingMessage&& other);

    auto size() const -> size_t;
    auto data() const -> const void*;
    auto charData() const -> const char*;

    virtual auto receive(Socket& socket, bool& moreToReceive) -> bool override;
};

inline
IncomingMessage::IncomingMessage()
    : Message()
{
}

inline
IncomingMessage::IncomingMessage(IncomingMessage&& other)
    : Message(std::move(other))
{
}

inline
IncomingMessage& IncomingMessage::operator=(IncomingMessage&& other)
{
    Message::operator=(std::move(other));
    return (*this);
}

inline
auto IncomingMessage::size() const -> size_t
{
    const zmq_msg_t* const msgPtr = getInternalMessage();
    CPPEROMQ_ASSERT(nullptr != msgPtr);
    return (zmq_msg_size(const_cast<zmq_msg_t*>(msgPtr)));
}

inline
auto IncomingMessage::data() const -> const void*
{
    const zmq_msg_t* const msgPtr = getInternalMessage();
    CPPEROMQ_ASSERT(nullptr != msgPtr);
    return (zmq_msg_data(const_cast<zmq_msg_t*>(msgPtr)));
}

inline
auto IncomingMessage::charData() const -> const char*
{
    return (static_cast<const char*>(data()));
}

inline
auto IncomingMessage::receive(Socket& socket, bool& moreToReceive) -> bool
{
    zmq_msg_t* msgPtr = getInternalMessage();

    CPPEROMQ_ASSERT(nullptr != msgPtr);
    CPPEROMQ_ASSERT(nullptr != socket.mSocket);

    if (0 != zmq_msg_close(msgPtr))
    {
        throw Error();
    }

    if (0 != zmq_msg_init(msgPtr))
    {
        throw Error();
    }

    moreToReceive = false;

    const int flags = 0;
    if (zmq_msg_recv(msgPtr, socket.mSocket, flags) >= 0)
    {
        moreToReceive = (0 != zmq_msg_more(msgPtr));
        return true;
    }

    if (zmq_errno() == EAGAIN)
    {
        return false;
    }

    throw Error();
}

}
