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

#include "DS_Timer.h"
#include "DS_Array.h"
#include "DS_Utils.h"
#include "DS_Socket.h"

#include <socky.h>

/**
 * Holds the sockets in a dynamic array (for automatic closing)
 */
static DS_Array sockets;

/**
 * Returns the given \a number as a string
 */
static sds itc (int number)
{
    return sdscatprintf (sdsempty(), "%d", number);
}

/**
 * Copies the received data from the socket in its data buffer
 */
static void read_socket (DS_Socket* ptr)
{
    if (!ptr)
        return;

    /* Initialize temporary buffer */
    int read = -1;
    sds data = sdsnewlen (NULL, 1024);

    /* Read TCP socket */
    if (ptr->type == DS_SOCKET_TCP)
        read = recv (ptr->info.sock_in, data, sdslen (data), 0);

    /* Read UDP socket */
    if (ptr->type == DS_SOCKET_UDP)
        read = udp_recvfrom (ptr->info.sock_in,
                             data, sdslen (data),
                             ptr->address, ptr->info.in_service, 0);

    /* We received some data, copy it to socket's buffer */
    if (read > 0) {
        DS_FREESTR (ptr->info.buffer);
        ptr->info.buffer = sdsnewlen (NULL, read);

        int i;
        for (i = 0; i < read; ++i)
            ptr->info.buffer [i] = data [i];
    }

    /* De-allocate temporary data */
    DS_FREESTR (data);
}

/**
 * Runs the server socket loop, which uses the \c select() function
 * to copy received data into the socket's buffer only when the
 * operating system detects that the socket received some data.
 *
 * Some people would use a socket timeout in this situation.
 * However, we don't need that since each socket needs to have its
 * own thread (because FRC networking is relatively intensive).
 * Since each socket has its own dedicated thread, we can affort to
 * use a blocking \c select() call, which also reduces the overall
 * CPU usage of the application.
 *
 * \param ptr a pointer to a \c DS_Socket structure
 */
static void server_loop (DS_Socket* ptr)
{
    if (ptr) {
        /* Set a 5-ms timeout on Windows */
#if defined _WIN32
        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 5000;
#endif

        /* Make the socket non-blocking on Windows */
#if defined _WIN32
        set_socket_block (ptr->info.sock_in, 0);
#endif

        /* Periodicaly check if there is data available on the socket */
        while (ptr->info.server_init == 1) {
            int rc;

            /* Set the file descriptor */
            fd_set set;
            FD_ZERO (&set);
            FD_SET (ptr->info.sock_in, &set);

            /* Run select */
#if defined _WIN32
            rc = select (0, &set, NULL, NULL, &tv);
#else
            rc = select (ptr->info.sock_in + 1, &set, NULL, NULL, NULL);
#endif

            /* Data is available, read it */
            if (rc > 0) {
                if (FD_ISSET (ptr->info.sock_in, &set))
                    read_socket (ptr);
            }
        }
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
    if (!data)
        return NULL;

    /* Cast raw pointer to socket */
    DS_Socket* ptr = (DS_Socket*) data;

    /* Make the address 0.0.0.0 if it is empty */
    if (DS_StringIsEmpty (ptr->address) || ptr->broadcast == 1) {
        DS_FREESTR (ptr->address);
        ptr->address = sdsnew ("0.0.0.0");
    }

    /* Set service strings */
    ptr->info.in_service = itc (ptr->in_port);
    ptr->info.out_service = itc (ptr->out_port);

    /* Open TCP socket */
    if (ptr->type == DS_SOCKET_TCP) {
        ptr->info.sock_in = create_server_tcp (ptr->info.in_service,
                                               SOCKY_IPv4, 0);

        ptr->info.sock_out = create_client_tcp (ptr->address,
                                                ptr->info.out_service,
                                                SOCKY_IPv4, 0);
    }

    /* Open UDP socket */
    else if (ptr->type == DS_SOCKET_UDP) {
        ptr->info.sock_in = create_server_udp (ptr->info.in_service,
                                               SOCKY_IPv4, 0);

        ptr->info.sock_out = create_client_udp (SOCKY_IPv4, 0);
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
DS_Socket DS_SocketEmpty()
{
    DS_Socket socket;

    /* Fill basic data */
    socket.in_port = 0;
    socket.out_port = 0;
    socket.disabled = 0;
    socket.broadcast = 0;
    socket.address = NULL;
    socket.type = DS_SOCKET_UDP;

    /* Fill socket info structure */
    socket.info.sock_in = 0;
    socket.info.sock_out = 0;
    socket.info.buffer = NULL;
    socket.info.server_init = 0;
    socket.info.client_init = 0;
    socket.info.socket_thread = 0;
    socket.info.in_service = NULL;
    socket.info.out_service = NULL;

    return socket;
}

/**
 * Initializes the sockets module and its event system
 */
void Sockets_Init()
{
    sockets_init (1);
    DS_ArrayInit (&sockets, sizeof (DS_Socket) * 5);
}

/**
 * Stops the event loops and closes all socket structures
 */
void Sockets_Close()
{
    /* Close all sockets */
    int i = 0;
    for (i = 0; i < (int) sockets.used; ++i)
        DS_SocketClose ((DS_Socket*) sockets.data [i]);

    /* Free socket array */
    DS_ArrayFree (&sockets);

    /* Close the sockets API */
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
    if (!ptr)
        return;

    /* Socket is disabled */
    if (ptr->disabled)
        return;

    /* Initialize the socket in another thread */
    pthread_t thread;
    pthread_create (&thread, NULL, &create_socket, (void*) ptr);
    ptr->info.socket_thread = thread;
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
    if (!ptr)
        return;

    /* Reset socket properties */
    ptr->info.server_init = 0;
    ptr->info.client_init = 0;

    /* Close sockets */
    socket_close (ptr->info.sock_in);
    socket_close (ptr->info.sock_out);

    /* Stop threads */
    DS_StopThread (ptr->info.socket_thread);

    /* Clear data buffers */
    DS_FREESTR (ptr->info.buffer);
    DS_FREESTR (ptr->info.in_service);
    DS_FREESTR (ptr->info.out_service);

    /* Reset socket information structure */
    ptr->info.sock_in = -1;
    ptr->info.sock_out = -1;
    ptr->info.socket_thread = 0;
}

/**
 * Returns any data received by the given socket
 *
 * \param ptr pointer to a \c DS_Socket structure
 */
sds DS_SocketRead (DS_Socket* ptr)
{
    /* Invalid pointer */
    if (!ptr)
        return NULL;

    /* Socket is disabled or uninitialized */
    if ((ptr->info.server_init == 0) || (ptr->disabled == 1))
        return NULL;

    /* Copy the current buffer and clear it */
    if (sdslen (ptr->info.buffer) > 0) {
        sds buffer = sdsdup (ptr->info.buffer);
        DS_FREESTR (ptr->info.buffer);
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
int DS_SocketSend (DS_Socket* ptr, sds data)
{
    /* Invalid pointer and/or empty data buffer */
    if (!ptr || DS_StringIsEmpty (data))
        return -1;

    /* Socket is disabled or uninitialized */
    if ((ptr->info.client_init == 0) || (ptr->disabled == 1))
        return -1;

    /* Send data using TCP */
    if (ptr->type == DS_SOCKET_TCP)
        return send (ptr->info.sock_out, data, sdslen (data), 0);

    /* Send data using UDP */
    else if (ptr->type == DS_SOCKET_UDP) {
        return udp_sendto (ptr->info.sock_out,
                           data, sdslen (data),
                           ptr->address, ptr->info.out_service, 0);
    }

    /* Should not happen */
    return -1;
}

/**
 * Changes the \a address of the given socket structre
 *
 * \param ptr pointer to a \c DS_Socket structure
 * \param address the new address to apply to the socket
 */
void DS_SocketChangeAddress (DS_Socket* ptr, sds address)
{
    /* Check if pointer is NULL */
    if (!ptr || !address)
        return;

    /* Close the socket */
    DS_SocketClose (ptr);

    /* Re-assign the address */
    if (sdscmp (ptr->address, address) != 0) {
        DS_FREESTR (ptr->address);
        ptr->address = sdsdup (address);
    }

    /* Re-configure the socket */
    DS_SocketOpen (ptr);
}
