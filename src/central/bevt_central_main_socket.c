/*
The MIT License (MIT)

Copyright (c) 2013 Vincent de RIBOU <belzo2005-dolphin@yahoo.fr>

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/*!
 * \file        bevt_central_main_socket.c
 * \brief       BozEvent relay client executable.
 * \version     0.1.0
 * \date        2013/01/14
 * \author      Vincent de RIBOU.
 * \copyright   Aquaplouf Land.
 *
 * \brief Implements client relay to central node.
 *
 */

#include <errno.h>
#include <sys/socket.h>
#include <skalibs/webipc.h>

#include "bevt_central_p.h"

#define MAIN_SOCKET_PATH "/tmp/bevt_centrals"

int main_socket_open(void) {
    register int r, fd;

    fd = ipc_stream_nb();
    if(fd<0) return fd;

    r = ipc_bind(fd, MAIN_SOCKET_PATH);
    if(r<0) return r;

    r = ipc_listen(fd, BEVT_CENTRAL_MAX_CONNS);
    if(r<0) return r;

    return (errno=0, fd);
}

int main_socket_close(const int mfd) {
    if(shutdown(mfd, SHUT_RDWR)<0) return -1;
    if(fd_close(mfd)<0) return -1;
    if(rm_rf(MAIN_SOCKET_PATH)<0) return -1;
    return (errno=0, 0);
}

int main_socket_accept(const int mfd) {
    int trunc;
    return ipc_accept_nb(mfd, 0, 0, &trunc);
}
    
