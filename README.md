# CpperoMQ
CpperoMQ is a C++11 binding for the [0MQ (libzmq)][1] C library.

## Features
CpperoMQ offers several advantages over using the [libzmq][1] C library directly and some of the other currently available C++ 0MQ bindings:
1. Highly composable multipart messaging
2. Performance close to using libzmq directly (via a good optimizer)
3. Type safety

## Using the Library
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
OutgoingMessage request("Hello");
pub.send(request);
// pub.receive(request); /* Compiler error! */
```

An `IncomingMessage` is default-constructed only.  Instances can be received but not sent:

```cpp
IncomingMessage request;
socket.receive(request);
// pub.send(request); /* Compiler error! */
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
class MessageHeader : public Sendable, public Receivable
{
    // ...
};

class Employee : public Sendable, public Receivable
{
    // ...
};

class Department : public Sendable, public Receivable
{
    // ...
};
```

Now the user can freely send and receive any combinations of CpperoMQ's `OutgoingMessage` and `IncomingMessage` as well as the above user-defined `MessageHeader`, `Employee`, and `Department`.

For a somewhat contrived example, imagine publishing a department update message where we broadcast that an employee now belongs to a different department:

```cpp
// From the publishing executable...
pub.send( MessageHeader(/* ... */) // header to identify message
        , Department(/* ... */)    // old department
        , Department(/* ... */)    // new department
        , Employee(/* ... */) );   // employee

// From the subscribing executable...
MessageHeader msgHeader;
Department oldDept;
Department newDept;
Employee employee;

sub.receive(msgHeader, oldDept, newDept, employee);
```

The above is sent as a single, multipart message that is guaranteed by 0MQ to be either received in full or not received at all by a connected receiving socket.  Note that at any point in the list of parameters for `send` we could have incorporated some `OutgoingMessage` objects too.  Likewise, for `receive` we could have included some `IncomingMessage` objects.  The message combination/mix-and-match capabilities are what makes the multipart messaging of CpperoMQ highly composable.

To see zguide examples implemented with CpperoMQ as well as some additional examples, se ethe [CpperoMQ-examples][9] repository.

**Disclaimer:** None of the above code checked for errors.  Real code should check the boolean result of each relevant library function.  CpperoMQ also can throw a CpperoMQ::Error exception, so that should be caught too.

## Drawbacks
CpperoMQ currently does not implement the following:
1. CURVE security
2. PAIR sockets
3. Some uncommon socket options (workaround by passing socket handle directly to [libzmq][1])

Feel free to [contribute](#Contributing) implementations for the above items.  The author may also add these as needed.

## Alternatives
The below 0MQ C++ bindings are alternatives to CpperoMQ:
1. [cppzmq][6]
2. [zmqpp][7]
3. [azmq][8]

## Requirements
CpperoMQ requires the following:
1. A compiler with [C++11 support][2]
2. libzmq's [zmq.hpp header][3] is available on the include path
3. Binaries compiled with CpperoMQ must be linked with [libzmq][1].

The specific C++11 features used are listed below.  Note that the source code is more likely to change than this README, so the list may be outdated.
```
auto
Defaulted functions
Delegating constructors
Deleted functions
final
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

The above are the environments and compilers available to the author.  Please [update this table directly](#Contributing) or notify the author upon confirming additional compiler support.

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
