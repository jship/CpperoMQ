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

#include <CpperoMQ/Common.hpp>

namespace CpperoMQ
{

class Message
{
public:
    virtual ~Message();
    Message(const Message& other) = delete;
    Message(Message&& other);
    Message& operator=(const Message& other) = delete;
    Message& operator=(Message&& other);

    friend auto swap(Message& lhs, Message& rhs) -> void;

protected:
    Message();
    Message(const size_t size, const void* sourceData);

    auto getInternalMessage() const -> const zmq_msg_t* const;
    auto getInternalMessage()       ->       zmq_msg_t*;

    auto shallowCopy(Message& dest) const -> void;

private:
    zmq_msg_t mMsg;
};

inline
Message::~Message()
{
    int result = zmq_msg_close(&mMsg);
    CPPEROMQ_ASSERT(0 == result);
}

inline
Message::Message(Message&& other)
    : Message()
{
    swap(*this, other);
}

inline
Message& Message::operator=(Message&& other)
{
    swap(*this, other);
    return (*this);
}

inline
Message::Message()
{
    if (0 != zmq_msg_init(&mMsg))
    {
        throw Error();
    }
}

inline
Message::Message(const size_t size, const void* sourceData)
{
    CPPEROMQ_ASSERT(nullptr != sourceData);

    if (0 != zmq_msg_init_size(&mMsg, size))
    {
        throw Error();
    }

    void* msgData = zmq_msg_data(&mMsg);
    CPPEROMQ_ASSERT(nullptr != msgData);
    memcpy(msgData, sourceData, size);
}

inline
auto Message::getInternalMessage() const -> const zmq_msg_t* const
{
    return (&mMsg);
}

inline
auto Message::getInternalMessage() -> zmq_msg_t*
{
    return (&mMsg);
}

inline
auto Message::shallowCopy(Message& dest) const -> void
{
    if (0 != zmq_msg_copy(&dest.mMsg, const_cast<zmq_msg_t*>(&mMsg)))
    {
        throw Error();
    }
}

inline
auto swap(Message& lhs, Message& rhs) -> void
{
    using std::swap;
    swap(lhs.mMsg, rhs.mMsg);
}

}
