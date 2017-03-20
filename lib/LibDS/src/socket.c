/*
 * The Driver Station Library (LibDS)
 * Copyright (C) 2015-2016 Alex Spataru <alex_spataru@outlook>
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
#include <bstraux.h>

/**
 * Copies the received data from the socket in its data buffer
 */
static void read_socket (DS_Socket* ptr)
{
    if (!ptr) {
        fprintf (stderr, "read_socket: received NULL parameter\n");
        return;
    }

    /* Initialize temporary buffer */
    int read = -1;
    bstring data = DS_GetEmptyString (4096);

    /* Read TCP socket */
    if (ptr->type == DS_SOCKET_TCP)
        read = recv (ptr->info.sock_in, (char*) data->data, blength (data), 0);

    /* Read UDP socket */
    if (ptr->type == DS_SOCKET_UDP) {
        char* addr = bstr2cstr (ptr->address, 0);

        read = udp_recvfrom (ptr->info.sock_in, (char*) data->data, blength (data),
                             addr, ptr->info.in_service, 0);

        DS_FREE (addr);
    }

    /* We received some data, copy it to socket's buffer */
    if (read > 0) {
        ptr->info.buffer_size = read;
        memset (ptr->info.buffer, 0, ptr->info.buffer_size);

        int i;
        for (i = 0; i < read; ++i)
            ptr->info.buffer [i] = data->data [i];
    }

    /* De-allocate temporary data */
    DS_FREESTR (data);
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
    if (!ptr) {
        fprintf (stderr, "server_loop: received NULL parameter\n");
        return;
    }

    int rc, fd;
    fd_set set;
    struct timeval tv;
    set_socket_block (ptr->info.sock_in, 0);

    while (ptr->info.server_init == 1 && ptr->info.sock_in > 0) {
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
    /* Data pointer is NULL */
    if (!data) {
        pthread_exit (NULL);
        return NULL;
    }

    /* Cast raw pointer to socket */
    DS_Socket* ptr = (DS_Socket*) data;

    /* Load fallback address if input address is invalid */
    if (DS_StringIsEmpty (ptr->address) || ptr->broadcast == 1) {
        DS_FREESTR (ptr->address);
        ptr->address = DS_FallBackAddress;
    }

    /* Ensure that buffer and service strings are set to 0 */
    memset (ptr->info.buffer, 0, sizeof (ptr->info.buffer));
    memset (ptr->info.in_service, 0, sizeof (ptr->info.in_service));
    memset (ptr->info.out_service, 0, sizeof (ptr->info.out_service));

    /* Set service strings */
    sprintf (ptr->info.in_service, "%d", ptr->in_port);
    sprintf (ptr->info.out_service, "%d", ptr->out_port);

    /* Open TCP socket */
    if (ptr->type == DS_SOCKET_TCP) {
        char* addr = bstr2cstr (ptr->address, 0);

        ptr->info.sock_in = create_server_tcp (ptr->info.in_service, SOCKY_IPv4, 0);
        ptr->info.sock_out = create_client_tcp (addr, ptr->info.out_service, SOCKY_IPv4, 0);

        DS_FREE (addr);
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
    pthread_exit (NULL);
}

/**
 * Returns an empty socket for safe initialization
 */
DS_Socket* DS_SocketEmpty (void)
{
    DS_Socket* socket = calloc (1, sizeof (DS_Socket));

    /* Fill basic data */
    socket->in_port = 0;
    socket->out_port = 0;
    socket->disabled = 0;
    socket->broadcast = 0;
    socket->address = NULL;
    socket->type = DS_SOCKET_UDP;

    /* Fill socket info structure */
    socket->info.thread = 0;
    socket->info.sock_in = 0;
    socket->info.sock_out = 0;
    socket->info.buffer_size = 0;
    socket->info.server_init = 0;
    socket->info.client_init = 0;

    /* Fill strings with 0 */
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
    /* Pointer is NULL */
    if (!ptr) {
        fprintf (stderr, "DS_SocketOpen: received NULL parameter\n");
        return;
    }

    /* Socket is disabled */
    if (ptr->disabled) {
        fprintf (stderr, "DS_SocketOpen: Socket %p is disabled, aborting\n", ptr);
        return;
    }

    /* Stop the current thread (if any) */
    if (ptr->info.thread)
        DS_StopThread (&ptr->info.thread);

    /* Initialize the socket in another thread */
    ptr->info.thread = 0;
    pthread_create (&ptr->info.thread, NULL, &create_socket, (void*) ptr);
}

/**
 * Closes the socket file descriptors of the given socket structure
 * and resets the structure's information.
 *
 * \param ptr pointer to the \c DS_Socket to close
 */
void DS_SocketClose (DS_Socket* ptr)
{
    /* Check for NULL pointer */
    if (!ptr) {
        fprintf (stderr, "DS_SocketClose: received NULL parameter\n");
        return;
    }

    /* Reset socket properties */
    ptr->info.server_init = 0;
    ptr->info.client_init = 0;

    /* Close sockets */
#ifndef __ANDROID
    socket_close_threaded (ptr->info.sock_in, NULL);
    socket_close_threaded (ptr->info.sock_out, NULL);
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

    /* Stop the socket thread (if any) */
    DS_StopThread (&ptr->info.thread);
    ptr->info.thread = 0;
}

/**
 * Returns any data received by the given socket
 *
 * \param ptr pointer to a \c DS_Socket structure
 */
bstring DS_SocketRead (DS_Socket* ptr)
{
    /* Invalid pointer */
    if (!ptr)
        return NULL;

    /* Socket is disabled or uninitialized */
    if ((ptr->info.server_init == 0) || (ptr->disabled == 1))
        return NULL;

    /* Copy the current buffer and clear it */
    if (ptr->info.buffer_size > 0) {
        bstring buffer = DS_GetEmptyString (ptr->info.buffer_size);

        /* Copy buffer to string */
        int i;
        for (i = 0; i < ptr->info.buffer_size; ++i) {
            buffer->data [i] = ptr->info.buffer [i];
            ptr->info.buffer [i] = 0;
        }

        /* Clear buffer info */
        ptr->info.buffer_size = 0;

        /* Return copied buffer */
        return buffer;
    }

    return NULL;
}


/**
 * Sends the given \a data using the given socket
 *
 * \param data the data buffer to send
 * \param ptr pointer to the socket to use to send the given \a data
 *
 * \returns number of bytes written on success, -1 on failure
 */
int DS_SocketSend (DS_Socket* ptr, const bstring data)
{
    /* Invalid pointer and/or empty data buffer */
    if (!ptr || DS_StringIsEmpty (data))
        return -1;

    /* Socket is disabled or uninitialized */
    if ((ptr->info.client_init == 0) || ptr->disabled)
        return -1;

    /* Get raw data */
    int error = 0;
    int len = blength (data);
    char* bytes = bstr2cstr (data, 0);

    /* Send data using TCP */
    if (ptr->type == DS_SOCKET_TCP)
        error = send (ptr->info.sock_out, bytes, len, 0);

    /* Send data using UDP */
    else if (ptr->type == DS_SOCKET_UDP) {
        char* addr = bstr2cstr (ptr->address, 0);
        error = udp_sendto (ptr->info.sock_out, bytes, len,
                            addr, ptr->info.out_service, 0);
        DS_FREE (addr);
    }

    /* Free temp. buffer */
    DS_FREE (bytes);

    /* Return error code */
    return error;
}

/**
 * Changes the \a address of the given socket structre
 *
 * \param ptr pointer to a \c DS_Socket structure
 * \param address the new address to apply to the socket
 */
void DS_SocketChangeAddress (DS_Socket* ptr, const bstring address)
{
    /* Pointers are invalid */
    if (!ptr || !address) {
        fprintf (stderr, "DS_SocketChangeAddress: received NULL parameter(s)\n");
        return;
    }

    /* Re-assign the address */
    DS_FREESTR (ptr->address);
    ptr->address = bstrcpy (address);

    /* Re-open the socket */
    DS_SocketClose (ptr);
    DS_SocketOpen (ptr);
}
