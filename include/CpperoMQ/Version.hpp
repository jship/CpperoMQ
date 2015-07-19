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

    const std::tuple<int, int, int> getTuple() const;
    const int getMajor() const;
    const int getMinor() const;
    const int getPatch() const;

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
const std::tuple<int, int, int> Version::getTuple() const
{
    return std::make_tuple(mMajor, mMinor, mPatch);
}

inline
const int Version::getMajor() const
{
    return mMajor;
}

inline
const int Version::getMinor() const
{
    return mMinor;
}

inline
const int Version::getPatch() const
{
    return mPatch;
}

}
