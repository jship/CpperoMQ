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

#include <algorithm>
#include <array>

namespace CpperoMQ
{

class Socket
{
    friend class IncomingMessage;
    friend class OutgoingMessage;

public:
    Socket() = delete;
    virtual ~Socket();
    Socket(const Socket& other) = delete;
    Socket(Socket&& other);
    Socket& operator=(Socket& other) = delete;
    Socket& operator=(Socket&& other);

    void bind(const char* address);
    void unbind(const char* address);

    void connect(const char* address);
    void disconnect(const char* address);

protected:
    Socket(void* context, int type);

private:
    void* mSocket;
};

inline
Socket::~Socket()
{
    if (mSocket != nullptr)
    {
        int result = zmq_close(mSocket);
        CPPEROMQ_ASSERT(result == 0);
        mSocket = 0 ;
    }
}

inline
Socket::Socket(Socket&& other)
    : mSocket(other.mSocket)
{
    other.mSocket = nullptr;
}

inline
Socket& Socket::operator=(Socket&& other)
{
    using std::swap;
    swap(mSocket, other.mSocket);
    return (*this);
}

inline
void Socket::bind(const char* address)
{
    if (0 != zmq_bind(mSocket, address))
    {
        throw Error();
    }
}

inline
void Socket::unbind(const char* address)
{
    if (0 != zmq_unbind(mSocket, address))
    {
        throw Error();
    }
}

inline
void Socket::connect(const char* address)
{
    if (0 != zmq_connect(mSocket, address))
    {
        throw Error();
    }
}

inline
void Socket::disconnect(const char* address)
{
    if (0 != zmq_disconnect(mSocket, address))
    {
        throw Error();
    }
}

inline
Socket::Socket(void* context, int type)
    : mSocket(nullptr)
{
    CPPEROMQ_ASSERT(context != nullptr);

    mSocket = zmq_socket(context, type);
    if (mSocket == NULL)
    {
        throw Error();
    }
}

}
