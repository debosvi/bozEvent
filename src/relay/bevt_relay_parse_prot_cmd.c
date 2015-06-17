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
 * \file        bevt_relayd.c
 * \brief       BozEvent relay client executable.
 * \version     0.1.0
 * \date        2013/01/14
 * \author      Vincent de RIBOU.
 * \copyright   Aquaplouf Land.
 *
 * \brief Implements client relay to central node.
 *
 */

#include <unistd.h>
#include <errno.h>
#include <stdio.h>

#include "bevt_relay_p.h"

int bevt_relay_parse_prot_cmd(unixmessage_t const *m, void *context) {
    (void)context;

    BEVT_DEBUG_LOG_INFO("message received, len(%d)", m->len);
    {
        char msg[] = "recv ok";
        unixmessage_t am = { .s = msg, .len = 7, .fds = 0, .nfds = 0 } ;
        if (!unixmessage_put(unixmessage_sender_1, &am))
            BEVT_DEBUG_LOG_ERROR("unable to put ack");
        else if (!unixmessage_sender_flush(unixmessage_sender_1))
            BEVT_DEBUG_LOG_ERROR("unable to send ack");
        else
            BEVT_DEBUG_LOG_INFO("ack sent");
    }
    {
        bozmessage_t c = { .s = m->s, .len = m->len };
        if (!bozmessage_put(&central_sender, &c))
            BEVT_DEBUG_LOG_ERROR("unable to put to central");
    }
    return 1;
}
