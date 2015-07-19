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

#include <memory>

namespace CpperoMQ
{

class Message
{
public:
    virtual ~Message();
    Message(const Message& other);
    Message(Message&& other);
    Message& operator=(Message other);

    friend void swap(Message& lhs, Message& rhs);

protected:
    Message();
    Message(const size_t size, const void* sourceData);

    const std::unique_ptr<zmq_msg_t>& getInternalMessage() const;

    void shallowCopy(Message& dest);

private:
    std::unique_ptr<zmq_msg_t> mMsg;
};

inline
Message::~Message()
{
    CPPEROMQ_ASSERT(nullptr != mMsg.get());
    CPPEROMQ_ASSERT(0 == zmq_msg_close(mMsg.get()));
}

inline
Message::Message(const Message& other)
    : mMsg(new zmq_msg_t)
{
    CPPEROMQ_ASSERT(nullptr != other.mMsg.get());

    const size_t otherSize = zmq_msg_size(other.mMsg.get());
    if (0 != zmq_msg_init_size(mMsg.get(), otherSize))
    {
        throw Error();
    }

    if (otherSize > 0)
    {
        void* msgData = zmq_msg_data(mMsg.get());
        CPPEROMQ_ASSERT(nullptr != msgData);

        void* otherData = zmq_msg_data(other.mMsg.get());
        CPPEROMQ_ASSERT(nullptr != otherData);

		memcpy(msgData, otherData, otherSize);
    }
}

inline
Message::Message(Message&& other)
    : Message()
{
    swap(*this, other);
}

inline
Message& Message::operator=(Message other)
{
    swap(*this, other);
    return (*this);
}

inline
Message::Message()
    : mMsg(new zmq_msg_t)
{
    if (0 != zmq_msg_init(mMsg.get()))
    {
        throw Error();
    }
}

inline
Message::Message(const size_t size, const void* sourceData)
    : mMsg(new zmq_msg_t)
{
    CPPEROMQ_ASSERT(nullptr != sourceData);

    if (0 != zmq_msg_init_size(mMsg.get(), size))
    {
        throw Error();
    }

    void* msgData = zmq_msg_data(mMsg.get());
    CPPEROMQ_ASSERT(nullptr != msgData);
    memcpy(msgData, sourceData, size);
}

inline
const std::unique_ptr<zmq_msg_t>& Message::getInternalMessage() const
{
    return (mMsg);
}

inline
void Message::shallowCopy(Message& dest)
{
    CPPEROMQ_ASSERT(nullptr != mMsg.get());
    CPPEROMQ_ASSERT(nullptr != dest.mMsg.get());

    if (0 != zmq_msg_copy(dest.mMsg.get(), mMsg.get()))
    {
        throw Error();
    }
}

inline
void swap(Message& lhs, Message& rhs)
{
    using std::swap;
    swap(lhs.mMsg, rhs.mMsg);
}

}
