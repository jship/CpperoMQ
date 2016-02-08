# CpperoMQ
CpperoMQ is a C++11 binding for the [0MQ (libzmq)][1] C library.

## Features
CpperoMQ offers several advantages over using the [libzmq][1] C library directly and some of the other currently available C++ 0MQ bindings:

1. Highly composable multipart messaging
2. Type safety
3. Performance close to using libzmq directly (via a good optimizer)

## Using the Library

To see zguide examples implemented with CpperoMQ as well as some additional examples, see the [CpperoMQ-examples][9] repository.

Just like 0MQ, the building blocks in CpperoMQ are contexts, sockets, and messages.

All CpperoMQ classes exist under the ```CpperoMQ``` namespace, so the following saves a few keystrokes for the remainder of this section:

```cpp
using namespace CpperoMQ;
```

A context encapsulates 0MQ's internal state.  A CpperoMQ `Context` can be initialized as follows:

```cpp
Context context;
```

A `Context` can be customized at construction and via mutators.  The most important responsibility of the Context is to create `Sockets`:

```cpp
RequestSocket requestSocket(context.createRequestSocket());
```

`Socket` objects are strongly typed and move-constructed from temporaries provided by the `Context`:

```cpp
auto dealer(context.createDealerSocket());
auto extPub(context.createExtendedPublishSocket());
auto extSub(context.createExtendedSubscribeSocket());
auto pub(context.createPublishSocket());
auto pull(context.createPullSocket());
auto push(context.createPushSocket());
auto reply(context.createReplySocket());
auto request(context.createRequestSocket());
auto router(context.createRouterSocket());
auto sub(context.createSubscribeSocket());
```

Depending on the socket type, a `Socket` can send `OutgoingMessage` objects and/or receive `IncomingMessage` objects.  An `OutgoingMessage` is initialized with data at construction only.  Instances can be sent but not received:

```cpp
OutgoingMessage message("Hello");
pub.send(message);
// pub.receive(message); /* Compiler error! */
```

An `IncomingMessage` is default-constructed only.  Instances can be received but not sent:

```cpp
IncomingMessage message;
sub.receive(message);
// sub.send(message); /* Compiler error! */
```

The primary goal of CpperoMQ is to provide users with extremely **composable multipart messaging**.  The `send` and `receive` methods on a `Socket` take any positive number of parameters, as long as each parameter implements the appropriate `Sendable` or `Receivable` interface.  All parameters in a single call to `send` or `receive` are treated as the individual message parts (a.k.a. frames) of a multipart message.

Long story short, users send a multipart message like this:

```cpp
// 'send' treats all of its parameters as a single, multipart message.
// No need to build up a list of message parts before-hand.
pub.send( OutgoingMessage("This")
        , OutgoingMessage("is")
        , OutgoingMessage("a")
        , OutgoingMessage("multipart")
        , OutgoingMessage("message") );
```

Users can then receive the above multipart message as its individual parts like this:

```cpp
// 'receive' treats all of its parameters as the parts of a single,
// multipart message.
IncomingMessage inMsg1, inMsg2, inMsg3, inMsg4, inMsg5;
sub.receive(inMsg1, inMsg2, inMsg3, inMsg4, inMsg5);
```

In many cases, it is undesirable to have to know up-front how many message parts are expected when receiving on a socket.  It is more convenient to send or receive complex objects directly on a socket.  Enter the `Sendable` and `Receivable` interfaces:

```cpp
class Employee : public Sendable, public Receivable
{
public:
    // ...
private:
    int mId;
    int mAge;
    std::string mName;
};

class Department : public Sendable, public Receivable
{
public:
    // ...
private:
    int mCode;
    std::string mName;
};
```

Now the user can freely send and receive any combinations of CpperoMQ's `OutgoingMessage` and `IncomingMessage` as well as the above user-defined `Employee` and `Department`.

For a somewhat contrived example, imagine publishing a department update message where we broadcast that an employee now belongs to a different department:

```cpp
// From the publishing executable...
pub.send( Department(/* ... */)    // old department
        , Department(/* ... */)    // new department
        , Employee(/* ... */) );   // employee
```

```cpp
// From the subscribing executable...
Department oldDept;
Department newDept;
Employee employee;

sub.receive(oldDept, newDept, employee);
```

The above is sent as a single, multipart message that is guaranteed by 0MQ to be either received in full or not received at all by a connected receiving socket.  Note that at any point in the list of parameters of `send` we could have incorporated some `OutgoingMessage` objects too.  Likewise, for `receive` we could have included some `IncomingMessage` objects.

Take a look at the implementation of the `Sendable` interface below for `Department`:

```cpp
virtual bool send(const CpperoMQ::Socket& socket, const bool moreToSend) const override
{
    using namespace CpperoMQ;

    OutgoingMessage idMsgPart(serializeNumber(mCode));
    if (!idMsgPart.send(socket, true))
        return false;

    OutgoingMessage nameMsgPart(mName.c_str());
    if (!nameMsgPart.send(socket, moreToSend))
        return false;

    return true;
}
```

Implementing this interface is what enables us to compose multipart messages containing `Department` objects.  Note that the caller and callee are flipped - instead of calling `send` on the socket, we call `send` on the `OutgoingMessage` objects, passing the `Socket` and a boolean as a parameter.  The boolean indicates whether or not there are more parts to send for the multipart message.

When implementing the `Sendable` interface, the requirement is to pass true for the boolean parameter for all internal send calls up to the final one.  The final send must pass the `moreToSend` parameter.  **Composing sendable, multipart messages containing complex objects in CpperoMQ _requires_ that this pattern is correctly followed.**

The implementation of the `Receivable` interface is similar:

```cpp
virtual bool receive(CpperoMQ::Socket& socket, bool& moreToReceive) override
{
    using namespace CpperoMQ;

    IncomingMessage idMsgPart;
    if (!idMsgPart.receive(socket, moreToReceive) || !moreToReceive)
        return false;

    IncomingMessage nameMsgPart;
    if (!nameMsgPart.receive(socket, moreToReceive))
        return false;

    mCode = deserializeNumber(idMsgPart);
    mName.assign(nameMsgPart.charData(), nameMsgPart.size());

    return true;
}
```

When implementing the `Receivable` interface, the requirement is to pass the moreToReceive boolean reference parameter to all internal receive calls.  After a receive is performed, this parameter's value indicates whether or not there exist more parts to receive for the multipart message.  Its value must be checked after all internal receive calls up to the last one.  **Composing receivable, multipart messages containing complex objects in CpperoMQ _requires_ that this pattern is correctly followed.**

We can go a step further with the above example by encapsulating the individual objects involved in a Department Update message:

```cpp
class DepartmentUpdate : public CpperoMQ::Sendable
                       , public CpperoMQ::Receivable
{
public:
    // ...

    virtual bool send(const CpperoMQ::Socket& socket, const bool moreToSend) const override
    {
        using namespace CpperoMQ;

        if (!mOldDept.send(socket, true))
            return false;

        if (!mNewDept.send(socket, true))
            return false;

        if (!mEmployee.send(socket, moreToSend))
            return false;

        return true;
    }

    virtual bool receive(CpperoMQ::Socket& socket, bool& moreToReceive) override
    {
        using namespace CpperoMQ;

        if (!mOldDept.receive(socket, moreToReceive) || !moreToReceive)
            return false;

        if (!mNewDept.receive(socket, moreToReceive) || !moreToReceive)
            return false;

        if (!mEmployee.receive(socket, moreToReceive))
            return false;

        return true;
    }

private:
    Department mOldDept;
    Department mNewDept;
    Employee mEmployee;
};
```

Now we can send `DepartmentUpdate` objects directly on `Sockets`.

**To see zguide examples implemented with CpperoMQ as well as some additional examples, see the [CpperoMQ-examples][9] repository.**

**Disclaimer:** Most of the above code did not check for errors.  Real code should check the boolean result of each relevant library function.  CpperoMQ can throw a CpperoMQ::Error exception, so that should be caught too.

## Drawbacks
CpperoMQ currently does not implement the following:

1. CURVE security
2. PAIR sockets
3. Some uncommon socket options (workaround by passing socket handle directly to [libzmq][1])

Feel free to contribute implementations for the above items.  The author may also add these as needed.  The workaround for 1 and 3 is to pass the socket handle to [libzmq][1].  2 requires direct use of [libzmq][1].

## Alternatives
The below 0MQ C++ bindings are alternatives to CpperoMQ:

1. [cppzmq][6]
2. [zmqpp][7]
3. [azmq][8]

## Requirements
CpperoMQ requires the following:

1. A compiler with [C++11 support][2]
2. [libzmq][1] 4.1 or greater.
3. libzmq's [zmq.hpp header][3] is available on the include path
4. Binaries compiled with CpperoMQ must be linked with [libzmq][1].

The specific C++11 features used are listed below.  Note that the source code is more likely to change than this README, so the list may be outdated.

```
auto
Defaulted functions
Delegating constructors
Deleted functions
final
function
noexcept (if not MSVC)
nullptr
override
Rvalue references
static_assert
Trailing return types
Variadic templates
```

## Compiler Support
CpperoMQ has been tested on the following operating systems and compilers:

| Operating System | Compiler   |
| ---------------- | ---------- |
| OS X 10.10       | Xcode 6.4  |
| Windows 8        | VS2013     |

The above are the environments and compilers available to the author.  Please update this table directly or notify the author upon confirming additional compiler support.

## Installation
CpperoMQ is a C++11 header-only library, making installation a painless two-step process:

1. Download CpperoMQ.
2. From the project root's 'include' directory, copy the CpperoMQ directory into a project's (or the system) include path.

## Contributing
Contributions to this binding via pull requests or bug reports are always welcome!  See the [0MQ contribution policy][4] page for details.

## Licensing
The CpperoMQ library is licensed under the MIT license.  The included [LICENSE](LICENSE) file and the comment header of each source file contain the full text of the [MIT license][5].

[1]: https://github.com/zeromq/libzmq
[2]: http://en.cppreference.com/w/cpp/compiler_support
[3]: https://github.com/zeromq/libzmq/blob/master/include/zmq.h
[4]: http://zeromq.org/docs:contributing
[5]: http://opensource.org/licenses/MIT
[6]: https://github.com/zeromq/cppzmq
[7]: https://github.com/zeromq/zmqpp
[8]: https://github.com/zeromq/azmq
[9]: https://github.com/jship/CpperoMQ-examples
