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
    IncomingMessage(IncomingMessage& other) = default;
    IncomingMessage(IncomingMessage&& other);
    IncomingMessage& operator=(const IncomingMessage& other);
    IncomingMessage& operator=(IncomingMessage&& other);

    const size_t size() const;
    const void * data() const;

    virtual bool receive(Socket& socket, bool& moreToReceive) override;
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
IncomingMessage& IncomingMessage::operator=(const IncomingMessage& other)
{
    Message::operator=(other);
    return (*this);
}

inline
IncomingMessage& IncomingMessage::operator=(IncomingMessage&& other)
{
    Message::operator=(std::move(other));
    return (*this);
}

inline
const size_t IncomingMessage::size() const
{
    CPPEROMQ_ASSERT(nullptr != getInternalMessage().get());
    return (zmq_msg_size(const_cast<zmq_msg_t*>(getInternalMessage().get())));
}

inline
const void * IncomingMessage::data() const
{
    CPPEROMQ_ASSERT(nullptr != getInternalMessage().get());
    return (zmq_msg_data(const_cast<zmq_msg_t*>(getInternalMessage().get())));
}

inline
bool IncomingMessage::receive(Socket& socket, bool& moreToReceive)
{
    CPPEROMQ_ASSERT(nullptr != getInternalMessage().get());
    CPPEROMQ_ASSERT(nullptr != socket.mSocket);

    if (0 != zmq_msg_close(getInternalMessage().get()))
    {
        throw Error();
    }

    if (0 != zmq_msg_init(getInternalMessage().get()))
    {
        throw Error();
    }

    moreToReceive = false;

    const int flags = 0;
    if (zmq_msg_recv(getInternalMessage().get(), socket.mSocket, flags) >= 0)
    {
        moreToReceive = (0 != zmq_msg_more(getInternalMessage().get()));
        return true;
    }

    if (zmq_errno() == EAGAIN)
    {
        return false;
    }

    throw Error();
}

}
