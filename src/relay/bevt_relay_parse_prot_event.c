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
 * \file        bevt_relay_parse_prot_event.c
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

//*****************************************************************************
//*****************************************************************************
static void async_answer(char c) {
    unixmessage_t am = { .s = &c, .len = 1, .fds = 0, .nfds = 0 } ;
    if (!unixmessage_put(unixmessage_sender_x, &am)) {
        BEVT_DEBUG_LOG_ERROR("%s: unable to put ack", __PRETTY_FUNCTION__);
    }
    else if (!unixmessage_sender_flush(unixmessage_sender_x)) {
        BEVT_DEBUG_LOG_ERROR("%s: unable to send ack", __PRETTY_FUNCTION__);
    }
    else {
        BEVT_DEBUG_LOG_INFO("%s: ack sent", __PRETTY_FUNCTION__);
    }
}

//*****************************************************************************
//*****************************************************************************
int bevt_relay_parse_prot_event(unixmessage_t const *m, void *context) {
    int r;
    bevt_client_id_t id;
    uint32 lg = 0;
    (void)context;

    BEVT_DEBUG_LOG_INFO("%s: message received, len(%d)", m->len);
    for(r=0; r<(int)m->len; r++)
        BEVT_DEBUG_LOG_INFO("%s: %02x ", m->s[r]);

    if(!strncmp(m->s, bevt_relay_commands[BEVT_RELAY_OP_NOTIFY], BEVT_RELAY_COMMAND_OP_LEN)) {
        uint64_unpack_big(m->s+BEVT_RELAY_COMMAND_OP_LEN, &id);
        uint32_unpack_big(m->s+BEVT_RELAY_COMMAND_OP_LEN+8, &lg);
        BEVT_DEBUG_LOG_INFO("%s: notify command, id(%llu), lg(%u)", __PRETTY_FUNCTION__, (long long unsigned int)id, lg);
    }
    else {
        BEVT_DEBUG_LOG_INFO("%s: unknown command", __PRETTY_FUNCTION__);
        async_answer(ECANCELED);
    }

    if(!r) errno=0;
    return 1;
}
