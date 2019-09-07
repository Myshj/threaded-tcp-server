# Simplest implementation of the threaded web server

Windows-only.

Uses `winsock2`.

Does not use heap. Stack-only.

Nothing is dynamically allocated, except worker threads.

Configured with (in `server.c`):

1. `BACKLOG_SIZE`: max count of pending connections (default `32`)

2. `MAX_THREADS`: max count of worker threads (default `2`)

3. `address` (default `INADDR_ANY`)

4. `port` (default `8888`)

5. `IP version` (default `IPv4`)

Once started, may be stopped by killing the process only.

The public interface consists of one function only, called `runServer`.

This function receives pointer to callback executed each time client connection is accepted.

The callback receives pointer to `ClientConnection` struct with `address` and `socket` fields.

The `ClientConnection` is guaranteed to be valid for the time serving thread is running.

It is also automatically freed after the callback execution.

# Possible improvements

## Make the server cross-platform

Should not be very difficult as Unix and Windows sockets are very close to each other.

## Use thread pool

Currently, each new connection is served by the separate thread.

With thread pool, connection may be established faster.

## Check unhappy paths

I'm not sure all of them are handled by now.

## Configure better

E.g. parameters for worker threads.

Or even run the server as daemon.
