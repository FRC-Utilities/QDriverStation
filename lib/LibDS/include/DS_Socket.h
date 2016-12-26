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

#ifndef _LIB_DS_SOCKET_H
#define _LIB_DS_SOCKET_H

#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>
#include <bstrlib.h>

#include "DS_Types.h"

/**
 * Holds all the private (erm, dirty) variables that the sockets module needs
 * to operate with the data provided by a \c DS_Socket structure
 */
typedef struct {
    bstring buffer;          /**< Holds received data */
    bstring in_service;      /**< Input port string */
    bstring out_service;     /**< Output port string */
    int sock_in;             /**< Server socket file descriptor */
    int sock_out;            /**< Client socket file descriptor */
    int client_init;         /**< Set to \c 1 if client socket is initialized */
    int server_init;         /**< Set to \c 1 if server socket is initialized */
} DS_SocketInfo;

/**
 * Holds all the 'public' variables of a socket, these variables can be used
 * both the the networking module and the rest of the application.
 */
typedef struct {
    bstring address;    /**< Holds the remote address */
    int disabled;       /**< Disables/enables the socket */
    int broadcast;      /**< Enables/disables broadcasting */
    int in_port;        /**< Sets the server/sender port */
    int out_port;       /**< Sets the client/sender port */
    DS_SocketType type; /**< Socket type (UDP or TCP) */
    DS_SocketInfo info; /**< Holds private socket information */
} DS_Socket;

/* For socket initialization */
extern DS_Socket* DS_SocketEmpty();

/* Module functions */
extern void Sockets_Init();
extern void Sockets_Close();

/* Socket initializer and destructor functions */
extern void DS_SocketOpen (DS_Socket* ptr);
extern void DS_SocketClose (DS_Socket* ptr);

/* I/O functions */
extern bstring DS_SocketRead (DS_Socket* ptr);
extern int DS_SocketSend (DS_Socket* ptr, const bstring data);
extern void DS_SocketChangeAddress (DS_Socket* ptr, const char* address);

#ifdef __cplusplus
}
#endif

#endif
