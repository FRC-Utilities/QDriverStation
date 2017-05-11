/*
 * The Driver Station Library (LibDS)
 * Copyright (c) 2015-2017 Alex Spataru <alex_spataru@outlook>
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

#include "DS_Utils.h"
#include "DS_Socket.h"

#include <socky.h>
#include <assert.h>

#define SPRINTF_S snprintf
#ifdef _WIN32
    #ifndef __MINGW32__
        #undef  SPRINTF_S
        #define SPRINTF_S sprintf_s
    #endif
#endif

/**
 * Copies the received data from the socket in its data buffer
 */
static void read_socket (DS_Socket* ptr)
{
    /* Check arguments */
    assert (ptr);

    /* Initialize temporary buffer */
    int read = -1;
    char data [4096] = {0};

    /* Read TCP socket */
    if (ptr->type == DS_SOCKET_TCP)
        read = recv (ptr->info.sock_in, data, sizeof (data), 0);

    /* Read UDP socket */
    if (ptr->type == DS_SOCKET_UDP) {
        read = udp_recvfrom (ptr->info.sock_in, data, sizeof (data),
                             ptr->address, ptr->info.in_service, 0);
    }

    /* We received some data, copy it to socket's buffer */
    if (read > 0) {
        ptr->info.buffer_size = read;
        memset (ptr->info.buffer, 0, ptr->info.buffer_size);

        int i;
        for (i = 0; i < read; ++i)
            ptr->info.buffer [i] = data [i];
    }
}

/**
 * Runs the server socket loop, which uses the \c select() function
 * to copy received data into the socket's buffer only when the
 * operating system detects that the socket received some data.
 *
 * \param ptr a pointer to a \c DS_Socket structure
 */
static void server_loop (DS_Socket* ptr)
{
    /* Check arguments */
    assert (ptr);

    /* Initialize variables for select */
    int rc, fd;
    fd_set set;
    struct timeval tv;

    /* Disable socket blocking */
#ifndef _WIN32
    set_socket_block (ptr->info.sock_in, 0);
#endif

    /* Run the server while the socket is valid */
    while (ptr->info.server_init && ptr->info.sock_in > 0) {
        tv.tv_sec = 0;
        tv.tv_usec = 5000 * 100;

        FD_ZERO (&set);
        FD_SET (ptr->info.sock_in, &set);

#if defined _WIN32
        fd = 0;
#else
        fd = ptr->info.sock_in + 1;
#endif

        rc = select (fd, &set, NULL, NULL, &tv);
        if (rc > 0 && FD_ISSET (ptr->info.sock_in, &set))
            read_socket (ptr);
    }
}

/**
 * Initializes the given socket structure
 *
 * \param data raw pointer to a \c DS_Socket structure
 */
static void* create_socket (void* data)
{
    /* Check arguments */
    assert (data);
    DS_Socket* ptr = (DS_Socket*) data;

    /* Ensure that buffer and service strings are set to 0 */
    memset (ptr->info.buffer, 0, sizeof (ptr->info.buffer));
    memset (ptr->info.in_service, 0, sizeof (ptr->info.in_service));
    memset (ptr->info.out_service, 0, sizeof (ptr->info.out_service));

    /* Set service strings */
    int len = sizeof (ptr->info.in_service);
    SPRINTF_S (ptr->info.in_service, len, "%d", ptr->in_port);
    SPRINTF_S (ptr->info.out_service, len, "%d", ptr->out_port);

    /* Open TCP socket */
    if (ptr->type == DS_SOCKET_TCP) {
        ptr->info.sock_in = create_server_tcp (ptr->info.in_service, SOCKY_IPv4, 0);
        ptr->info.sock_out = create_client_tcp (ptr->address, ptr->info.out_service, SOCKY_IPv4, 0);
    }

    /* Open UDP socket */
    else if (ptr->type == DS_SOCKET_UDP) {
        ptr->info.sock_out = create_client_udp (SOCKY_IPv4, 0);
        ptr->info.sock_in = create_server_udp (ptr->info.in_service, SOCKY_IPv4, 0);
    }

    /* Update initialized states */
    ptr->info.server_init = (ptr->info.sock_in > 0);
    ptr->info.client_init = (ptr->info.sock_out > 0);

    /* Start server loop */
    server_loop (ptr);

    /* Exit */
    return NULL;
}

/**
 * Returns an empty socket for safe initialization
 */
DS_Socket* DS_SocketEmpty (void)
{
    /* Initialize a new socket */
    DS_Socket* socket = (DS_Socket*) calloc (1, sizeof (DS_Socket));

    /* Fill basic data */
    socket->in_port = 0;
    socket->out_port = 0;
    socket->disabled = 0;
    socket->broadcast = 0;
    socket->type = DS_SOCKET_UDP;

    /* Fill socket info structure */
    socket->info.sock_in = 0;
    socket->info.sock_out = 0;
    socket->info.buffer_size = 0;
    socket->info.server_init = 0;
    socket->info.client_init = 0;

    /* Fill strings with 0 */
    memset (socket->address, 0, sizeof (socket->address));
    memset (socket->info.buffer, 0, sizeof (socket->info.buffer));
    memset (socket->info.in_service, 0, sizeof (socket->info.in_service));
    memset (socket->info.out_service, 0, sizeof (socket->info.out_service));

    /* Return the socket data */
    return socket;
}

/**
 * Initializes the sockets module and its event system
 */
void Sockets_Init (void)
{
    sockets_init (1);
}

/**
 * Stops the event loops and closes all socket structures
 */
void Sockets_Close (void)
{
    sockets_exit();
}

/**
 * Initializes and configures the given socket
 *
 * \note The socket will be initialzed in another thread to avoid blocking
 *       the main thread of the application
 */
void DS_SocketOpen (DS_Socket* ptr)
{
    /* Check arguments */
    assert (ptr);

    /* Socket is disabled */
    if (ptr->disabled)
        return;

    /* Initialize the socket in another thread */
    pthread_t thread;
    int error = pthread_create (&thread, NULL,
                                &create_socket, (void*) ptr);

    /* Warn the user when the socket cannot start */
    if (error) {
        DS_String caption = DS_StrNew ("LibDS");
        DS_String message = DS_StrNew ("Cannot start socket thread!");
        DS_ShowMessageBox (&caption, &message, DS_ICON_ERROR);
        DS_StrRmBuf (&caption);
        DS_StrRmBuf (&message);
    }

    /* Quit if socket cannot start */
    assert (!error);
}

/**
 * Closes the socket file descriptors of the given socket structure
 * and resets the structure's information.
 *
 * \param ptr pointer to the \c DS_Socket to close
 */
void DS_SocketClose (DS_Socket* ptr)
{
    /* Check arguments */
    assert (ptr);

    /* Reset socket properties */
    ptr->info.server_init = 0;
    ptr->info.client_init = 0;

    /* Close sockets */
#if defined (__ANDROID__)
    socket_close_threaded (ptr->info.sock_in);
    socket_close_threaded (ptr->info.sock_out);
#else
    socket_close (ptr->info.sock_in);
    socket_close (ptr->info.sock_out);
#endif

    /* Reset socket information structure */
    ptr->info.sock_in = -1;
    ptr->info.sock_out = -1;
    ptr->info.buffer_size = 0;

    /* Reset strings */
    memset (ptr->info.buffer, 0, sizeof (ptr->info.buffer));
    memset (ptr->info.in_service, 0, sizeof (ptr->info.in_service));
    memset (ptr->info.out_service, 0, sizeof (ptr->info.out_service));
}

/**
 * Returns any data received by the given socket
 *
 * \param ptr pointer to a \c DS_Socket structure
 */
DS_String DS_SocketRead (DS_Socket* ptr)
{
    /* Check arguments */
    assert (ptr);

    /* Socket is disabled or uninitialized */
    if ((ptr->info.server_init == 0) || (ptr->disabled == 1))
        return DS_StrNewLen (0);

    /* Copy the current buffer and clear it */
    if (ptr->info.buffer_size > 0) {
        DS_String buffer = DS_StrNewLen (ptr->info.buffer_size);

        /* Copy buffer to string */
        int i;
        for (i = 0; i < (int) ptr->info.buffer_size; ++i)
            DS_StrSetChar (&buffer, i, ptr->info.buffer [i]);

        /* Clear buffer info */
        memset (ptr->info.buffer, 0, ptr->info.buffer_size);
        ptr->info.buffer_size = 0;

        /* Return copied buffer */
        return buffer;
    }

    return DS_StrNewLen (0);
}


/**
 * Sends the given \a data using the given socket
 *
 * \param data the data buffer to send
 * \param ptr pointer to the socket to use to send the given \a data
 *
 * \returns number of bytes written on success, -1 on failure
 */
int DS_SocketSend (const DS_Socket* ptr, const DS_String* data)
{
    /* Check arguments */
    assert (ptr);
    assert (data);

    /* Socket is disabled or uninitialized */
    if ((ptr->info.client_init == 0) || ptr->disabled)
        return -1;

    /* Data is empty */
    if (DS_StrEmpty (data))
        return 0;

    /* Initialize variables*/
    int bytes_written = 0;
    int len = DS_StrLen (data);
    char* bytes = DS_StrToChar (data);

    /* Send data using TCP */
    if (ptr->type == DS_SOCKET_TCP)
        bytes_written = send (ptr->info.sock_out, bytes, len, 0);

    /* Send data using UDP */
    else if (ptr->type == DS_SOCKET_UDP) {
        bytes_written = udp_sendto (ptr->info.sock_out, bytes, len,
                                    ptr->address, ptr->info.out_service, 0);
    }

    /* Delete temp. buffer */
    DS_FREE (bytes);

    /* Return error code */
    return bytes_written;
}

/**
 * Changes the \a address of the given socket structre
 *
 * \param ptr pointer to a \c DS_Socket structure
 * \param address the new address to apply to the socket
 */
void DS_SocketChangeAddress (DS_Socket* ptr, const char* address)
{
    /* Check arguments */
    assert (ptr);

    /* Abort if address is NULL */
    if (!address)
        return;

    /* Re-assign the address */
    memset (ptr->address, 0, sizeof (ptr->address));
    memcpy (ptr->address, address, strlen (address));

    /* Re-open the socket */
    DS_SocketClose (ptr);
    DS_SocketOpen (ptr);
}
