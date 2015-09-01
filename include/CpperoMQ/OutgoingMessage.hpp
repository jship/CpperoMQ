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
#include <CpperoMQ/Sendable.hpp>
#include <CpperoMQ/Socket.hpp>

namespace CpperoMQ
{

class OutgoingMessage final : public Message, public Sendable
{
public:
    OutgoingMessage(const size_t size, const void* sourceData);
    OutgoingMessage(const size_t size, const char* sourceData);
    OutgoingMessage(const char* sourceData);
    OutgoingMessage(); // for empty frames
    virtual ~OutgoingMessage() = default;
    OutgoingMessage(const OutgoingMessage& other) = delete;
    OutgoingMessage(OutgoingMessage&& other);
    OutgoingMessage& operator=(const OutgoingMessage& other) = delete;
    OutgoingMessage& operator=(OutgoingMessage&& other);

    virtual auto send(const Socket& socket, const bool moreToSend) const -> bool override;
};

inline
OutgoingMessage::OutgoingMessage(const size_t size, const void* sourceData)
    : Message(size, sourceData)
{
}

inline
OutgoingMessage::OutgoingMessage(const size_t size, const char* sourceData)
    : Message(size, sourceData)
{
}

inline
OutgoingMessage::OutgoingMessage(const char* sourceData)
    : Message(std::strlen(sourceData), sourceData)
{
}

inline
OutgoingMessage::OutgoingMessage()
    : Message()
{
}

inline
OutgoingMessage::OutgoingMessage(OutgoingMessage&& other)
    : Message(std::move(other))
{
}

inline
OutgoingMessage& OutgoingMessage::operator=(OutgoingMessage&& other)
{
    Message::operator=(std::move(other));
    return (*this);
}

inline
auto OutgoingMessage::send(const Socket& socket, const bool moreToSend) const -> bool
{
    zmq_msg_t* msgPtr = const_cast<zmq_msg_t*>(getInternalMessage());

    CPPEROMQ_ASSERT(nullptr != msgPtr);
    CPPEROMQ_ASSERT(nullptr != socket.mSocket);

    OutgoingMessage message;
    shallowCopy(message);
    msgPtr = message.getInternalMessage();

    CPPEROMQ_ASSERT(nullptr != msgPtr);

    const int flags = (moreToSend) ? ZMQ_SNDMORE : 0;
    if (zmq_msg_send(msgPtr, socket.mSocket, flags) >= 0)
    {
        return true;
    }

    if (zmq_errno() == EAGAIN)
    {
        return false;
    }

    throw Error();
}

}
