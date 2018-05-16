# ConnectionPool
A simple ConnectionPool in C++

## Introduction
We need a TCP Connection pool

## Feature
- Fast
- Thread safe
- Generic (TCP connection pool using C++/Other Connections can use this too)
- Pre-opens arbitrary number of connections
- Used connections can be returned for immediate reuse
- Unreturned connections are automatically closed and replaced (thanks to shared_ptr reference counting)
- Apache 2.0 license

## Example
You can find in test.cpp

```cpp
#include "TcpConnectionPool.h"
#include "ConnectionPool.h"
using namespace emc;

// Create a pool of 2 TCP connections
std::shared_ptr<TCPConnectionFactory> connection_factory(new TCPConnectionFactory("10.249.248.192", 9069));
std::shared_ptr<ConnectionPool<TCPConnection> > pool(new ConnectionPool<TCPConnection>(2, connection_factory));   
// Borrow a connection and then you can do anything
std::shared_ptr<TCPConnection> conn=pool->borrow();
pool->unborrow(conn);

```

## Design notes
Connections are stored as a std::deque of std::shared_ptr so we can pop from the front and push from back; this makes sure all connections get cycled through as fast as possible (so we don't accidentally hang onto any dead ones for a long time).

We managed to get all of this WITHOUT a separate curator thread.  Calling borrow() should only block very briefly while we access the pool deque, etc.  If we have to replace a dead connection with a new one, borrow() will additionally block while the new connection is set up.  If we are still unable to serve a live connection, borrow() will throw.

The use of boost::shared_ptr for tracking connections yeilds some nice side effects. If you have a problem with a connection, just let the shared_ptr fall out of scope and the connection will automagically be closed (immediately) and then replaced with a brand new connection (later, when we need one). In this way, it promotes 'safe' handling of connections without the need to manually ping() (or whatever) each connection periodically.
