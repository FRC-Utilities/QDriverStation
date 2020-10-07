/*
 * Copyright (C) 2016 Alex Spataru <alex_spataru@outlook>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "socky.h"

#include <assert.h>
#include <stdlib.h>
#include <pthread.h>

#if defined _WIN32
    static WSADATA WSA_DATA;
    #define GET_ERR WSAGetLastError()
#else
    #define GET_ERR errno
#endif

/**
 * Returns \c 0 if the given socket file descriptor is invalid
 *
 * \param sfd the socket file descriptor
 */
static int valid_sfd (int sfd)
{
    return (sfd > 0);
}

/**
 * Prints a detailed error message if \c VERBOSE is defined
 */
static void print_error (int sfd, const char* message, int error)
{
#if defined SOCKY_VERBOSE
#if defined _WIN32
    const char* string = gai_strerrorA (error);
#else
    const char* string = strerror (error);
#endif

    fprintf (stderr,
             "Socket %d:\n"
             "\t Message: %s\n"
             "\t Error Code: %d\n"
             "\t Error Desc: %s\n",
             sfd, message, error, string);
#else
    (void) sfd;
    (void) error;
    (void) message;
#endif
}

/**
 * Returns a valid address family. Input values can be:
 *    - \c SOCKY_IPv4
 *    - \c SOCKY_IPv6
 *    - \c SOCKY_ANY
 *
 * If any invalid value is set to the \a flag parameter, then this function
 * shall return \c AF_UNSPEC (let the OS decide an address family)
 */
static int get_family (int flag)
{
    assert (flag == SOCKY_IPv4 ||
            flag == SOCKY_IPv6 ||
            flag == SOCKY_ANY);

    switch (flag) {
    case SOCKY_IPv4:
        return AF_INET;
        break;
    case SOCKY_IPv6:
        return AF_INET6;
        break;
    case SOCKY_ANY:
        return AF_UNSPEC;
        break;
    default:
        return -1;
        break;
    }
}

/**
 * Returns a valid socket type. Input values can be:
 *    - \c SOCKY_TCP
 *    - \c SOCKY_UDP
 *
 * If any invalid value is set to the \a flag parameter, then this function
 * shall return \c SOCK_STREAM (a TCP socket)
 */
static int get_socktype (int flag)
{
    assert (flag == SOCKY_TCP ||
            flag == SOCKY_UDP);

    switch (flag) {
    case SOCKY_TCP:
        return SOCK_STREAM;
        break;
    case SOCKY_UDP:
        return SOCK_DGRAM;
        break;
    default:
        return -1;
        break;
    }
}

/**
 * Sets the \c SO_REUSEADDR option to the given socket
 */
static int set_socket_options (int sfd)
{
    if (valid_sfd (sfd)) {
        /* Initialize variables */
        int err = 1;
        int val = 1;

        /* Set options according to each platform */
        {
#if defined _WIN32
            err = setsockopt (sfd,
                              SOL_SOCKET,
                              SO_REUSEADDR,
                              (const char*) &val, sizeof (val));
#else
#ifndef __ANDROID__
            err *= setsockopt (sfd,
                               SOL_SOCKET,
                               SO_REUSEPORT,
                               &val, sizeof (val));
#endif
            err *= setsockopt (sfd,
                               SOL_SOCKET,
                               SO_REUSEADDR,
                               &val, sizeof (val));
#endif
        }

        /* Setting the options failed */
        if (err != 0) {
            print_error (sfd, "cannot set socket options", GET_ERR);
            return -1;
        }

        /* Options set correctly */
        return 0;
    }

    /* Socket is invalid, return -1 */
    return -1;
}

/**
 * Configures a new server socket with the given properties
 *
 * \param port the port/service string to bind to
 * \param family the address family (IPv4 or IPv6)
 * \param socktype the socket type (UDP or TCP)
 * \param flags any additional flags that you may need to use
 *
 * \returns -1 on error, socket file descriptor on success
 */
static int create_server (const char* port, const int family,
                          const int socktype, const int flags)
{
    int sfd;
    struct addrinfo* info = NULL;
    struct addrinfo* addr = get_address_info (NULL, port, socktype, family);

    /* Obtained address info is NULL */
    if (addr == NULL)
        return -1;

    /* Loop through found addresses until we establish a connection */
    for (info = addr; info != NULL; info = info->ai_next) {
        /* Open the socket */
        sfd = socket (info->ai_family,
                      info->ai_socktype | flags,
                      info->ai_protocol);


        /* Invalid socket, continue probing... */
        if (!valid_sfd (sfd) || (set_socket_options (sfd) == -1))
            continue;

        /* Bound without error, break loop */
        if (bind (sfd, info->ai_addr, info->ai_addrlen) == 0) {
            /* Configure the TCP listener */
            if (socktype == SOCKY_TCP) {
                if (listen (sfd, SOCKY_BACKLOG) == 0)
                    break;
            }

            /* UDP does not listen */
            else
                break;
        }

        /* Close temp. socket */
        socket_close (sfd);
    }

    /* Everything should be good, but let's check */
    if (info == NULL) {
        print_error (sfd, "cannot bind to any address!", GET_ERR);
        socket_close (sfd);
        return -1;
    }

    /* Server socket setup correctly */
    freeaddrinfo (info);
    return sfd;
}

/**
 * Casts the given \a data pointer into an int containing the
 * socket file descriptor and closes it
 */
static void* close_socket (void* data)
{
    assert (data);
    int* sfd = (int*) data;
    socket_close (*sfd);
    return NULL;
}

/**
 * If compiling on Windows, this function closes the WinSock API.
 * If you are using anything else, this function will do nothing.
 *
 * \returns 0 on success
 */
int sockets_exit (void)
{
#if defined _WIN32
    return WSACleanup();
#endif

    return 0;
}

/**
 * If compiling on Windows, this function starts the WinSock API.
 * If you are using anything else, this function will do nothing.
 *
 * \param exit_on_fail If set to 1, this function shall close your application
 *                     if the WinSock API fails to start
 *
 * \returns 0 on success, -1 on failure
 */
int sockets_init (const int exit_on_fail)
{
#if defined _WIN32
    if (WSAStartup (WINSOCK_VERSION, &WSA_DATA) != 0) {
        fprintf (stderr, "Cannot start WinSock API, error %d\n", GET_ERR);
        if (exit_on_fail == 1)
            exit (EXIT_FAILURE);

        return -1;
    }
#else
    (void) exit_on_fail;
#endif

    return 0;
}

/**
 * Makes the given socket blocking or not
 *
 * \param sfd the socket file descriptor
 * \param block determines if the socket shall be blocking or not
 */
int set_socket_block (const int sfd, const int block)
{
#if defined _WIN32
    u_long flags = block ? 1 : 0;
    return ioctlsocket (sfd, FIONBIO, &flags);
#else
    int flags = block ? 0 : O_NONBLOCK;
    return fcntl (sfd, F_SETFL, flags);
#endif
}

/**
 * Obtains the address information for the given \a host, \a service and
 * address \a family
 *
 * \param host the host name
 * \param service the service name or port string
 * \param socktype the socket type
 * \param family the address family (e.g. \c AF_INET or \c AF_INET6)
 */
struct addrinfo* get_address_info (const char* host,
                                   const char* service,
                                   int socktype, int family)
{
    struct addrinfo hints, *info;

    /* Fill the hints with zeroes */
    memset (&hints, 0, sizeof (hints));

    /* Set hints */
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = get_family (family);
    hints.ai_socktype = get_socktype (socktype);

    /* Get address info */
    int error = getaddrinfo (host, service, &hints, &info);

    /* Check if there was an error with the address */
    if (error) {
#if defined SOCKY_VERBOSE
        int code = GET_ERR;
        fprintf (stderr,
                 "Cannot obtain address info:\n"
                 "\t Address: %s\n"
                 "\t Service: %s\n"
                 "\t Error Code: %d\n"
                 "\t Error Desc: %s\n",
                 host, service, code, strerror (code));
#endif
        return NULL;
    }

    /* All good, return the obtained data */
    return info;
}

/**
 * Creates a new UDP client socket using the given \a family and \a flags
 *
 * \param family the address family (\c SOCKY_IPv4 or \c SOCKY_IPv6)
 * \param flags the additional flags to use while creating the socket
 */
int create_client_udp (const int family, const int flags)
{
    int sfd = -1;

    /* Create the socket */
    switch (family) {
    case SOCKY_IPv4:
        sfd = socket (AF_INET, SOCK_DGRAM | flags, 0);
        break;
    case SOCKY_IPv6:
        sfd = socket (AF_INET6, SOCK_DGRAM | flags, 0);
        break;
    default:
        return -1;
    }

    /* Fuck, there was an error creating the socket */
    if (!valid_sfd (sfd)) {
        print_error (sfd, "cannot create UDP client socket", GET_ERR);
        return -1;
    }

    /* Set socket options */
    if (set_socket_options (sfd) == -1) {
        socket_close (sfd);
        return -1;
    }

    /* Return the socket file descriptor */
    return sfd;
}

/**
 * Creates a new TCP socket and connects it to the given \a host and \a port
 *
 * \param host the hostname to connect to
 * \param port the port/service to use
 * \param family the address family
 * \param flags any additional flags that you may want to use
 */
int create_client_tcp (const char* host, const char* port,
                       const int family, const int flags)
{
    int sfd = -1;

    /* Get address information */
    struct addrinfo* info = NULL;
    struct addrinfo* addr = get_address_info (host, port, SOCKY_TCP, family);

    /* Address information is NULL, abort */
    if (addr == NULL)
        return -1;

    /* Loop through found addresses until we establish a connection */
    for (info = addr; info != NULL; info = info->ai_next ) {
        /* Create new socket */
        sfd = socket (info->ai_family,
                      info->ai_socktype | flags,
                      info->ai_protocol);

        /* Invalid socket, continue probing... */
        if (!valid_sfd (sfd) || (set_socket_options (sfd) == -1))
            continue;

        /* Connected without error, break loop */
        if (connect (sfd, info->ai_addr, info->ai_addrlen) != -1)
            break;

        /* Close temp. socket */
        socket_close (sfd);
    }

    /* We should have established a connection, but let's check */
    if (info == NULL) {
        print_error (sfd, "cannot connect to any address!", GET_ERR);
        socket_close (sfd);
        return -1;
    }

    /* Yay! */
    freeaddrinfo (addr);
    freeaddrinfo (info);
    return sfd;
}

/**
 * Configures a new UDP server socket with the given properties
 *
 * \param port the port/service string to bind to
 * \param family the address family (IPv4 or IPv6)
 * \param flags any additional flags that you may need to use
 *
 * \returns -1 on error, socket file descriptor on success
 */
int create_server_udp (const char* port, const int family, const int flags)
{
    return create_server (port, family, SOCKY_UDP, flags);
}

/**
 * Configures a new TCP server socket with the given properties
 *
 * \param port the port/service string to bind to
 * \param family the address family (IPv4 or IPv6)
 * \param flags any additional flags that you may need to use
 *
 * \returns -1 on error, socket file descriptor on success
 */
int create_server_tcp (const char* port, const int family, const int flags)
{
    return create_server (port, family, SOCKY_TCP, flags);
}

/**
 * Closes the given socket using native implementations
 *
 * \param sfd the socket file descriptor to close
 *
 * \returns -1 on failure, 0 on success
 */
int socket_close (const int sfd)
{
    /* The socket FD is not valid */
    if (!valid_sfd (sfd))
        return -1;

    /* Disable I/O operations on the socket */
    shutdown (sfd, SOCKY_READ | SOCKY_WRITE);

    /* Close the socket */
    int error = 0;
#if defined _WIN32
    error = closesocket (sfd);
#else
    error = close (sfd);
#endif

    /* Return result */
    return error;
}

/**
 * Closes the given \a sfd in a different thread
 */
void socket_close_threaded (int sfd)
{
    /* Try to close the socket on different thread */
    pthread_t thread;
    int error = pthread_create (&thread, NULL,
                                &close_socket, (void*) &sfd);

    /* Close socket normally if there is an error */
    if (error)
        shutdown (sfd, SOCKY_READ | SOCKY_WRITE);
}

/**
 * Calls the system's \c shutdown() function on the given socket
 *
 * \param sfd the socket file descriptor
 * \param method the shutdown method (READ | WRITE | BOTH)
 *
 * \returns -1 on failure, 0 on success
 */
int socket_shutdown (const int sfd, const int method)
{
    if (valid_sfd (sfd))
        return shutdown (sfd, method);

    return -1;
}

/**
 * Accepts a new TCP connection and writes remote host information
 * to the provided parameters.
 *
 * \param sfd the server socket
 * \param host the string in which to write host name
 * \param host_len the expected length of the host string
 * \param service the string in which to write the service
 * \param service_len the expected length of the service string
 * \param flags any additional flags that you may need to use
 *
 * \returns a new socket file descriptor on success, -1 of failure
 */
int tcp_accept (const int sfd, char* host, const int host_len,
                char* service, const int service_len, const int flags)
{
    int client_sfd;
    struct sockaddr_storage client_info;
    socklen_t addrlen = sizeof (struct sockaddr_storage);

    /* Accept the connection */
    client_sfd = accept (sfd, (struct sockaddr*) &client_info, &addrlen);

    /* Check if new socket is valid */
    if (!valid_sfd (client_sfd)) {
        print_error (sfd, "cannot create client socket durring accept()", GET_ERR);
        return -1;
    }

    /* Write information to provided parameters */
    int err = getnameinfo ((struct sockaddr*) &client_info,
                           sizeof (struct sockaddr_storage),
                           host, host_len, service, service_len, flags);

    /* Check if there was an error obtaining remote information */
    if (err != 0)
        print_error (sfd, "cannot obtain remote host information", GET_ERR);

    /* Return new socket */
    return client_sfd;
}

/**
 * Re-implements the \c sendto function
 *
 * \param sfd the socket descriptor
 * \param buf the data buffer to send
 * \param buf_len the length of the data buffer
 * \param host the host in which to send the data
 * \param service the remote service/port string
 * \param flags any additional flags that you may need to use
 */
int udp_sendto (const int sfd,
                const char* buf, const int buf_len,
                const char* host, const char* service, const int flags)
{
    /* Check if socket and buffer are valid */
    if (!valid_sfd (sfd) || buf == NULL || buf_len <= 0)
        return -1;

    /* Get address info */
    struct addrinfo* info = get_address_info (host, service,
                                              SOCKY_UDP, SOCKY_ANY);

    /* Invalid address info */
    if (!info)
        return -1;

    /* Send datagram */
    int bytes = sendto (sfd, buf, buf_len, flags,
                        info->ai_addr, (int) info->ai_addrlen);

    /* Free address information */
    freeaddrinfo (info);

    /* Return number of bytes written */
    return bytes;
}

/**
 * Re-implements the \c recvfrom function
 *
 * \param sfd the socket file descriptor
 * \param buf the data buffer in which to write the data into
 * \param buf_len the length of the data buffer
 * \param host the remote host from which to receive data
 * \param service the local service/port from which to receive data
 * \param flags any additional flags that you may need to use
 */
int udp_recvfrom (const int sfd, char* buf, const int buf_len,
                  const char* host, const char* service, const int flags)
{
    /* Check if socket and buffer length are valid */
    if (!valid_sfd (sfd) || buf_len <= 0)
        return -1;

    /* Get address info */
    struct addrinfo* info = get_address_info (host, service,
                                              SOCKY_UDP, SOCKY_ANY);

    /* Invalid address info */
    if (info == NULL)
        return -1;

    /* Receive remote data */
#if defined _WIN32
    int bytes = recvfrom (sfd, buf, buf_len, flags,
                          info->ai_addr, (int*) &info->ai_addrlen);
#else
    int bytes = recvfrom (sfd, buf, buf_len, flags,
                          info->ai_addr, &info->ai_addrlen);
#endif

    /* Return the number of bytes received */
    freeaddrinfo (info);
    return bytes;
}
