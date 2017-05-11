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

#ifndef _LIB_DS_SOCKET_H
#define _LIB_DS_SOCKET_H

#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>

#include "DS_Types.h"
#include "DS_String.h"

/**
 * Holds all the private (erm, dirty) variables that the sockets module needs
 * to operate with the data provided by a \c DS_Socket structure
 */
typedef struct {
    int sock_in;           /**< Input socket file descriptor */
    int sock_out;          /**< Output socket file descriptor */
    int client_init;       /**< 1 if client is working, 0 if not */
    int server_init;       /**< 1 if server is working, 0 if not */
    size_t buffer_size;    /**< Holds the number of received bytes */
    char buffer [4096];    /**< Holds the received data buffer */
    char in_service [12];  /**< Holds the input port number as a string */
    char out_service [12]; /**< Holds the output port number as a string */
} DS_SocketInfo;

/**
 * Holds all the 'public' variables of a socket, these variables can be used
 * both the the networking module and the rest of the application.
 */
typedef struct {
    int in_port;           /**< Input port number */
    int out_port;          /**< Output port number */
    int disabled;          /**< 1 if socket shall not send or receive data */
    int broadcast;         /**< 1 if socket shall send or receive broadcasts */
    char address [512];    /**< Address of remote host */
    DS_SocketType type;    /**< Type of socket (UDP/TCP) */
    DS_SocketInfo info;    /**< Ugly data about the socket */
} DS_Socket;

/* For socket initialization */
extern DS_Socket* DS_SocketEmpty (void);

/* Module functions */
extern void Sockets_Init (void);
extern void Sockets_Close (void);

/* Socket initializer and destructor functions */
extern void DS_SocketOpen (DS_Socket* ptr);
extern void DS_SocketClose (DS_Socket* ptr);

/* I/O functions */
extern DS_String DS_SocketRead (DS_Socket* ptr);
extern int DS_SocketSend (const DS_Socket* ptr, const DS_String* data);
extern void DS_SocketChangeAddress (DS_Socket* ptr, const char* address);

#ifdef __cplusplus
}
#endif

#endif
