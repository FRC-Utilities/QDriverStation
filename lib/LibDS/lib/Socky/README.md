# Socky

Socky is a simple library that eases cross-platform socket programming. It is inspired by the C implementation of [libsocket](https://github.com/dermesser/libsocket).

## Features

This library supports the following things:

- IPv4 (client, server)
- IPv6 (client, server)
- TCP and UDP (client, server)
- Error reporting

Creating sockets, connecting them and checking for errors is already done for you.

Most socket programming functions are wrapped by Socky, e.g:

- `sendto` and `recvfrom`
- `connect` and `bind` (both in one function)
- `accept`

All Socky functions work with the raw socket file descriptors, this allows you to do your own socket work in par with Socky.

## Platforms

Socky has been tested on the following platforms:

- Mac OSX
- GNU/Linux
- Microsoft Windows

Socky should work with other UNIX platforms, such as FreeBSD and SunOS, however, I haven't done any testing on those platforms.

## Error Reporting

To enable error reporting, just define 'VERBOSE' in your project:

```c
#define VERBOSE
```