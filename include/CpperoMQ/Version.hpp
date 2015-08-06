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

#include <tuple>

namespace CpperoMQ
{

class Version
{
public:
    Version();

    auto getTuple() const -> std::tuple<int, int, int>;
    auto getMajor() const -> int;
    auto getMinor() const -> int;
    auto getPatch() const -> int;

private:
    int mMajor;
    int mMinor;
    int mPatch;
};

inline
Version::Version()
    : mMajor(0)
    , mMinor(0)
    , mPatch(0)
{
    zmq_version(&mMajor, &mMinor, &mPatch);
}

inline
auto Version::getTuple() const -> std::tuple<int, int, int>
{
    return std::make_tuple(mMajor, mMinor, mPatch);
}

inline
auto Version::getMajor() const -> int
{
    return mMajor;
}

inline
auto Version::getMinor() const -> int
{
    return mMinor;
}

inline
auto Version::getPatch() const -> int
{
    return mPatch;
}

}
